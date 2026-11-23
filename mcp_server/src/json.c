/*
 * This file is part of the LogicAnalyzer project.
 *
 * Copyright (C) 2026 WeAct Studio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "json.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	const char *start;
	const char *p;
	const char *end;
	char *err;
	size_t errlen;
	int depth;
} parser;

#define JSON_MAX_DEPTH 64

static void set_error(parser *ps, const char *message)
{
	if (ps->err && ps->errlen && !ps->err[0])
		snprintf(ps->err, ps->errlen, "%s at offset %ld", message,
			 (long)(ps->p - ps->start));
}

static void skip_ws(parser *ps)
{
	while (ps->p < ps->end && (*ps->p == ' ' || *ps->p == '\t' ||
				   *ps->p == '\n' || *ps->p == '\r'))
		ps->p++;
}

static json_value *value_new(json_type type)
{
	json_value *v = calloc(1, sizeof(*v));

	if (!v) {
		fputs("out of memory\n", stderr);
		exit(1);
	}
	v->type = type;
	return v;
}

void json_free(json_value *value)
{
	size_t i;

	if (!value)
		return;
	switch (value->type) {
	case JSON_STRING:
		free(value->u.string);
		break;
	case JSON_ARRAY:
		for (i = 0; i < value->u.array.count; i++)
			json_free(value->u.array.items[i]);
		free(value->u.array.items);
		break;
	case JSON_OBJECT:
		for (i = 0; i < value->u.object.count; i++) {
			free(value->u.object.keys[i]);
			json_free(value->u.object.values[i]);
		}
		free(value->u.object.keys);
		free(value->u.object.values);
		break;
	default:
		break;
	}
	free(value);
}

static void append_utf8(char **buf, size_t *len, size_t *cap, unsigned cp)
{
	unsigned char out[4];
	size_t n;

	if (cp < 0x80) {
		out[0] = (unsigned char)cp;
		n = 1;
	} else if (cp < 0x800) {
		out[0] = (unsigned char)(0xC0 | (cp >> 6));
		out[1] = (unsigned char)(0x80 | (cp & 0x3F));
		n = 2;
	} else if (cp < 0x10000) {
		out[0] = (unsigned char)(0xE0 | (cp >> 12));
		out[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
		out[2] = (unsigned char)(0x80 | (cp & 0x3F));
		n = 3;
	} else {
		out[0] = (unsigned char)(0xF0 | (cp >> 18));
		out[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
		out[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
		out[3] = (unsigned char)(0x80 | (cp & 0x3F));
		n = 4;
	}
	if (*len + n + 1 > *cap) {
		size_t want = *cap ? *cap : 32;
		char *grown;

		while (want < *len + n + 1)
			want *= 2;
		grown = realloc(*buf, want);
		if (!grown) {
			fputs("out of memory\n", stderr);
			exit(1);
		}
		*buf = grown;
		*cap = want;
	}
	memcpy(*buf + *len, out, n);
	*len += n;
	(*buf)[*len] = '\0';
}

static char *parse_string_raw(parser *ps)
{
	char *buf = NULL;
	size_t len = 0, cap = 0;

	ps->p++;	/* opening quote */
	for (;;) {
		unsigned cp = 0;
		char c;

		if (ps->p >= ps->end) {
			set_error(ps, "unterminated string");
			free(buf);
			return NULL;
		}
		c = *ps->p++;
		if (c == '"')
			break;
		if (c != '\\') {
			append_utf8(&buf, &len, &cap, (unsigned char)c);
			continue;
		}
		if (ps->p >= ps->end) {
			set_error(ps, "unterminated escape");
			free(buf);
			return NULL;
		}
		switch (*ps->p++) {
		case '"':  append_utf8(&buf, &len, &cap, '"');  break;
		case '\\': append_utf8(&buf, &len, &cap, '\\'); break;
		case '/':  append_utf8(&buf, &len, &cap, '/');  break;
		case 'b':  append_utf8(&buf, &len, &cap, '\b'); break;
		case 'f':  append_utf8(&buf, &len, &cap, '\f'); break;
		case 'n':  append_utf8(&buf, &len, &cap, '\n'); break;
		case 'r':  append_utf8(&buf, &len, &cap, '\r'); break;
		case 't':  append_utf8(&buf, &len, &cap, '\t'); break;
		case 'u':
			if (ps->end - ps->p < 4) {
				set_error(ps, "short \\u escape");
				free(buf);
				return NULL;
			}
			for (int i = 0; i < 4; i++) {
				char h = ps->p[i];

				cp <<= 4;
				if (h >= '0' && h <= '9')
					cp |= (unsigned)(h - '0');
				else if (h >= 'a' && h <= 'f')
					cp |= (unsigned)(h - 'a' + 10);
				else if (h >= 'A' && h <= 'F')
					cp |= (unsigned)(h - 'A' + 10);
				else {
					set_error(ps, "bad \\u escape");
					free(buf);
					return NULL;
				}
			}
			ps->p += 4;
			/* Surrogate pairs are rare in requests; a lone half is
			 * turned into U+FFFD rather than failing the request. */
			if (cp >= 0xD800 && cp <= 0xDBFF) {
				unsigned low = 0;

				if (ps->end - ps->p >= 6 && ps->p[0] == '\\' &&
				    ps->p[1] == 'u') {
					for (int i = 0; i < 4; i++) {
						char h = ps->p[2 + i];

						low <<= 4;
						if (h >= '0' && h <= '9')
							low |= (unsigned)(h - '0');
						else if (h >= 'a' && h <= 'f')
							low |= (unsigned)(h - 'a' + 10);
						else if (h >= 'A' && h <= 'F')
							low |= (unsigned)(h - 'A' + 10);
					}
					if (low >= 0xDC00 && low <= 0xDFFF) {
						cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
						ps->p += 6;
					}
				}
			}
			append_utf8(&buf, &len, &cap, cp);
			break;
		default:
			set_error(ps, "unknown escape");
			free(buf);
			return NULL;
		}
	}
	if (!buf)
		return calloc(1, 1);
	return buf;
}

static json_value *parse_value(parser *ps);

static json_value *parse_array(parser *ps)
{
	json_value *array = value_new(JSON_ARRAY);
	size_t cap = 0;

	ps->p++;
	skip_ws(ps);
	if (ps->p < ps->end && *ps->p == ']') {
		ps->p++;
		return array;
	}
	for (;;) {
		json_value *item = parse_value(ps);

		if (!item) {
			json_free(array);
			return NULL;
		}
		if (array->u.array.count == cap) {
			size_t want = cap ? cap * 2 : 4;
			json_value **grown = realloc(array->u.array.items,
						     want * sizeof(*grown));

			if (!grown) {
				fputs("out of memory\n", stderr);
				exit(1);
			}
			array->u.array.items = grown;
			cap = want;
		}
		array->u.array.items[array->u.array.count++] = item;

		skip_ws(ps);
		if (ps->p >= ps->end) {
			set_error(ps, "unterminated array");
			json_free(array);
			return NULL;
		}
		if (*ps->p == ',') {
			ps->p++;
			continue;
		}
		if (*ps->p == ']') {
			ps->p++;
			return array;
		}
		set_error(ps, "expected ',' or ']'");
		json_free(array);
		return NULL;
	}
}

static json_value *parse_object(parser *ps)
{
	json_value *object = value_new(JSON_OBJECT);
	size_t cap = 0;

	ps->p++;
	skip_ws(ps);
	if (ps->p < ps->end && *ps->p == '}') {
		ps->p++;
		return object;
	}
	for (;;) {
		char *key;
		json_value *item;

		skip_ws(ps);
		if (ps->p >= ps->end || *ps->p != '"') {
			set_error(ps, "expected a member name");
			json_free(object);
			return NULL;
		}
		key = parse_string_raw(ps);
		if (!key) {
			json_free(object);
			return NULL;
		}
		skip_ws(ps);
		if (ps->p >= ps->end || *ps->p != ':') {
			set_error(ps, "expected ':'");
			free(key);
			json_free(object);
			return NULL;
		}
		ps->p++;
		item = parse_value(ps);
		if (!item) {
			free(key);
			json_free(object);
			return NULL;
		}
		if (object->u.object.count == cap) {
			size_t want = cap ? cap * 2 : 4;
			char **keys = realloc(object->u.object.keys, want * sizeof(*keys));
			json_value **values = realloc(object->u.object.values,
						      want * sizeof(*values));

			if (!keys || !values) {
				fputs("out of memory\n", stderr);
				exit(1);
			}
			object->u.object.keys = keys;
			object->u.object.values = values;
			cap = want;
		}
		object->u.object.keys[object->u.object.count] = key;
		object->u.object.values[object->u.object.count] = item;
		object->u.object.count++;

		skip_ws(ps);
		if (ps->p >= ps->end) {
			set_error(ps, "unterminated object");
			json_free(object);
			return NULL;
		}
		if (*ps->p == ',') {
			ps->p++;
			continue;
		}
		if (*ps->p == '}') {
			ps->p++;
			return object;
		}
		set_error(ps, "expected ',' or '}'");
		json_free(object);
		return NULL;
	}
}

static json_value *parse_value(parser *ps)
{
	skip_ws(ps);
	if (ps->p >= ps->end) {
		set_error(ps, "unexpected end of input");
		return NULL;
	}
	if (++ps->depth > JSON_MAX_DEPTH) {
		set_error(ps, "nesting too deep");
		return NULL;
	}

	json_value *result = NULL;

	switch (*ps->p) {
	case '{':
		result = parse_object(ps);
		break;
	case '[':
		result = parse_array(ps);
		break;
	case '"': {
		char *text = parse_string_raw(ps);

		if (!text)
			return NULL;
		result = value_new(JSON_STRING);
		result->u.string = text;
		break;
	}
	case 't':
		if ((size_t)(ps->end - ps->p) >= 4 && !strncmp(ps->p, "true", 4)) {
			ps->p += 4;
			result = value_new(JSON_BOOL);
			result->u.boolean = 1;
		}
		break;
	case 'f':
		if ((size_t)(ps->end - ps->p) >= 5 && !strncmp(ps->p, "false", 5)) {
			ps->p += 5;
			result = value_new(JSON_BOOL);
			result->u.boolean = 0;
		}
		break;
	case 'n':
		if ((size_t)(ps->end - ps->p) >= 4 && !strncmp(ps->p, "null", 4)) {
			ps->p += 4;
			result = value_new(JSON_NULL);
		}
		break;
	default: {
		char *endp = NULL;
		double number = strtod(ps->p, &endp);

		if (endp && endp > ps->p) {
			ps->p = endp;
			result = value_new(JSON_NUMBER);
			result->u.number = number;
		}
		break;
	}
	}

	ps->depth--;
	if (!result)
		set_error(ps, "invalid value");
	return result;
}

json_value *json_parse(const char *text, size_t len, char *err, size_t errlen)
{
	parser ps;

	if (err && errlen)
		err[0] = '\0';
	ps.start = text;
	ps.p = text;
	ps.end = text + len;
	ps.err = err;
	ps.errlen = errlen;
	ps.depth = 0;

	json_value *value = parse_value(&ps);

	if (!value)
		return NULL;
	skip_ws(&ps);
	if (ps.p != ps.end) {
		set_error(&ps, "trailing data");
		json_free(value);
		return NULL;
	}
	return value;
}

const json_value *json_member(const json_value *object, const char *key)
{
	size_t i;

	if (!object || object->type != JSON_OBJECT)
		return NULL;
	for (i = 0; i < object->u.object.count; i++) {
		if (!strcmp(object->u.object.keys[i], key))
			return object->u.object.values[i];
	}
	return NULL;
}

const char *json_get_string(const json_value *object, const char *key, const char *fallback)
{
	const json_value *value = json_member(object, key);

	return (value && value->type == JSON_STRING) ? value->u.string : fallback;
}

double json_get_number(const json_value *object, const char *key, double fallback)
{
	const json_value *value = json_member(object, key);

	return (value && value->type == JSON_NUMBER) ? value->u.number : fallback;
}

int json_get_bool(const json_value *object, const char *key, int fallback)
{
	const json_value *value = json_member(object, key);

	return (value && value->type == JSON_BOOL) ? value->u.boolean : fallback;
}

int json_id_text(const json_value *value, char *out, size_t outlen)
{
	if (!value) {
		snprintf(out, outlen, "null");
		return 0;
	}
	switch (value->type) {
	case JSON_NUMBER:
		/* Ids are integers in practice; keep them integral to avoid
		 * echoing 1 as 1.000000. */
		if (value->u.number == (double)(long long)value->u.number)
			snprintf(out, outlen, "%lld", (long long)value->u.number);
		else
			snprintf(out, outlen, "%.17g", value->u.number);
		return 1;
	case JSON_STRING: {
		size_t i = 0;

		out[i++] = '"';
		for (const char *p = value->u.string; *p && i + 7 < outlen; p++) {
			if (*p == '"' || *p == '\\')
				out[i++] = '\\';
			out[i++] = *p;
		}
		out[i++] = '"';
		out[i] = '\0';
		return 1;
	}
	case JSON_NULL:
		snprintf(out, outlen, "null");
		return 0;
	default:
		snprintf(out, outlen, "null");
		return 0;
	}
}

void json_pretty(const char *text, size_t len, strbuf *out)
{
	int depth = 0;
	int in_string = 0;

	for (size_t i = 0; i < len; i++) {
		char c = text[i];

		if (in_string) {
			sb_putc(out, c);
			if (c == '\\' && i + 1 < len)
				sb_putc(out, text[++i]);
			else if (c == '"')
				in_string = 0;
			continue;
		}

		switch (c) {
		case '"':
			in_string = 1;
			sb_putc(out, c);
			break;
		case '{':
		case '[': {
			char close = (c == '{') ? '}' : ']';

			sb_putc(out, c);
			/* "{}" and "[]" stay on one line. */
			size_t j = i + 1;

			while (j < len && (text[j] == ' ' || text[j] == '\t' ||
					   text[j] == '\n' || text[j] == '\r'))
				j++;
			if (j < len && text[j] == close) {
				sb_putc(out, close);
				i = j;
				break;
			}
			depth++;
			sb_putc(out, '\n');
			for (int k = 0; k < depth; k++)
				sb_put(out, "  ");
			break;
		}
		case '}':
		case ']':
			depth--;
			sb_putc(out, '\n');
			for (int k = 0; k < depth; k++)
				sb_put(out, "  ");
			sb_putc(out, c);
			break;
		case ',':
			sb_putc(out, c);
			sb_putc(out, '\n');
			for (int k = 0; k < depth; k++)
				sb_put(out, "  ");
			break;
		case ':':
			sb_put(out, ": ");
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			break;
		default:
			sb_putc(out, c);
		}
	}
}
