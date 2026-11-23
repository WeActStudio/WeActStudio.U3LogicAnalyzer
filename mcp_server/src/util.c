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

#include "util.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
# define PATH_SEP '\\'
# define IS_SEP(c) ((c) == '\\' || (c) == '/')
#else
# define PATH_SEP '/'
# define IS_SEP(c) ((c) == '/')
#endif

static void *xmalloc(size_t size)
{
	void *p = malloc(size);

	if (!p) {
		fputs("out of memory\n", stderr);
		exit(1);
	}
	return p;
}

void sb_init(strbuf *sb)
{
	sb->cap = 256;
	sb->len = 0;
	sb->data = xmalloc(sb->cap);
	sb->data[0] = '\0';
}

void sb_free(strbuf *sb)
{
	free(sb->data);
	sb->data = NULL;
	sb->len = sb->cap = 0;
}

void sb_reset(strbuf *sb)
{
	sb->len = 0;
	if (sb->data)
		sb->data[0] = '\0';
}

static void sb_reserve(strbuf *sb, size_t extra)
{
	size_t want;
	char *grown;

	if (sb->len + extra + 1 <= sb->cap)
		return;
	want = sb->cap ? sb->cap : 256;
	while (want < sb->len + extra + 1)
		want *= 2;
	grown = realloc(sb->data, want);
	if (!grown) {
		fputs("out of memory\n", stderr);
		exit(1);
	}
	sb->data = grown;
	sb->cap = want;
}

void sb_putc(strbuf *sb, char c)
{
	sb_reserve(sb, 1);
	sb->data[sb->len++] = c;
	sb->data[sb->len] = '\0';
}

void sb_putn(strbuf *sb, const char *text, size_t len)
{
	if (!len)
		return;
	sb_reserve(sb, len);
	memcpy(sb->data + sb->len, text, len);
	sb->len += len;
	sb->data[sb->len] = '\0';
}

void sb_put(strbuf *sb, const char *text)
{
	if (text)
		sb_putn(sb, text, strlen(text));
}

void sb_printf(strbuf *sb, const char *fmt, ...)
{
	va_list ap, copy;
	char stack[512];
	int need;

	va_start(ap, fmt);
	va_copy(copy, ap);
	need = vsnprintf(stack, sizeof(stack), fmt, ap);
	va_end(ap);
	if (need < 0) {
		va_end(copy);
		return;
	}
	if ((size_t)need < sizeof(stack)) {
		sb_putn(sb, stack, (size_t)need);
		va_end(copy);
		return;
	}

	sb_reserve(sb, (size_t)need);
	vsnprintf(sb->data + sb->len, (size_t)need + 1, fmt, copy);
	sb->len += (size_t)need;
	va_end(copy);
}

void sb_json_string(strbuf *sb, const char *text)
{
	const unsigned char *p = (const unsigned char *)(text ? text : "");

	sb_putc(sb, '"');
	for (; *p; p++) {
		switch (*p) {
		case '"':  sb_put(sb, "\\\""); break;
		case '\\': sb_put(sb, "\\\\"); break;
		case '\n': sb_put(sb, "\\n"); break;
		case '\r': sb_put(sb, "\\r"); break;
		case '\t': sb_put(sb, "\\t"); break;
		case '\b': sb_put(sb, "\\b"); break;
		case '\f': sb_put(sb, "\\f"); break;
		default:
			/* Control characters have to be escaped, everything from
			 * 0x20 up is valid UTF-8 as it arrives. */
			if (*p < 0x20)
				sb_printf(sb, "\\u%04x", *p);
			else
				sb_putc(sb, (char)*p);
		}
	}
	sb_putc(sb, '"');
}

char *xstrdup(const char *text)
{
	size_t len = text ? strlen(text) : 0;
	char *copy = xmalloc(len + 1);

	if (len)
		memcpy(copy, text, len);
	copy[len] = '\0';
	return copy;
}

char *str_trim(char *text)
{
	char *end;

	while (*text && isspace((unsigned char)*text))
		text++;
	end = text + strlen(text);
	while (end > text && isspace((unsigned char)end[-1]))
		end--;
	*end = '\0';
	return text;
}

char *path_dirname(const char *path)
{
	const char *slash = NULL, *p;
	size_t len;
	char *dir;

	if (!path)
		return xstrdup("");
	for (p = path; *p; p++) {
		if (IS_SEP(*p))
			slash = p;
	}
	if (!slash)
		return xstrdup("");
	if (slash == path)
		return xstrdup("/");

	len = (size_t)(slash - path);
	dir = xmalloc(len + 1);
	memcpy(dir, path, len);
	dir[len] = '\0';
	return dir;
}

char *path_join(const char *dir, const char *name)
{
	size_t dlen = dir ? strlen(dir) : 0;
	size_t nlen = name ? strlen(name) : 0;
	int need_sep = dlen > 0 && !IS_SEP(dir[dlen - 1]);
	char *out = xmalloc(dlen + nlen + 2);

	if (dlen)
		memcpy(out, dir, dlen);
	if (need_sep)
		out[dlen] = PATH_SEP;
	if (nlen)
		memcpy(out + dlen + need_sep, name, nlen);
	out[dlen + need_sep + nlen] = '\0';
	return out;
}

#ifdef _WIN32
# include <windows.h>

char *path_exe_dir(void)
{
	wchar_t wide[4096];
	DWORD len;
	char *utf8;
	int need;

	len = GetModuleFileNameW(NULL, wide, (DWORD)(sizeof(wide) / sizeof(wide[0])));
	if (len == 0 || len >= sizeof(wide) / sizeof(wide[0]))
		return NULL;
	need = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
	if (need <= 0)
		return NULL;
	utf8 = malloc((size_t)need);
	if (!utf8)
		return NULL;
	if (!WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, need, NULL, NULL)) {
		free(utf8);
		return NULL;
	}
	{
		char *dir = path_dirname(utf8);

		free(utf8);
		return dir;
	}
}

#else

# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# ifdef __APPLE__
#  include <mach-o/dyld.h>
# endif

char *path_exe_dir(void)
{
	char buffer[PATH_MAX];
	char resolved[PATH_MAX];

# ifdef __APPLE__
	uint32_t size = (uint32_t)sizeof(buffer);

	if (_NSGetExecutablePath(buffer, &size) != 0)
		return NULL;
	if (!realpath(buffer, resolved))
		return NULL;
	return path_dirname(resolved);
# else
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

	if (len <= 0)
		return NULL;
	buffer[len] = '\0';
	return path_dirname(buffer);
# endif
}

#endif
