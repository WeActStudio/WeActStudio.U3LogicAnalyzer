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

/*
 * Minimal JSON reader for MCP requests: enough of RFC 8259 to walk a request
 * and pick out the few fields a tool takes. Building responses is done with
 * the strbuf helpers instead, where the shape is known and escaping is the
 * only thing that matters.
 */
#ifndef U3_MCP_JSON_H
#define U3_MCP_JSON_H

#include <stddef.h>

#include "util.h"

typedef enum {
	JSON_NULL,
	JSON_BOOL,
	JSON_NUMBER,
	JSON_STRING,
	JSON_ARRAY,
	JSON_OBJECT
} json_type;

typedef struct json_value json_value;

struct json_value {
	json_type type;
	union {
		int boolean;
		double number;
		char *string;
		struct {
			json_value **items;
			size_t count;
		} array;
		struct {
			char **keys;
			json_value **values;
			size_t count;
		} object;
	} u;
};

/* Parses one complete value. NULL on error, with a message in err. */
json_value *json_parse(const char *text, size_t len, char *err, size_t errlen);
void json_free(json_value *value);

/* Member lookup on an object, NULL when the object has no such member. */
const json_value *json_member(const json_value *object, const char *key);

/* Typed accessors with a fallback for a missing or wrongly typed value. */
const char *json_get_string(const json_value *object, const char *key, const char *fallback);
double json_get_number(const json_value *object, const char *key, double fallback);
int json_get_bool(const json_value *object, const char *key, int fallback);

/* For an id that may be a number, a string or null. Fills out with its JSON text. */
int json_id_text(const json_value *value, char *out, size_t outlen);

/* Re-indents a compact document for a human or an agent to read. */
void json_pretty(const char *text, size_t len, strbuf *out);

#endif
