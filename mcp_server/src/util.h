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
 * Small helpers shared by the MCP server: a growable byte buffer and the
 * string/path routines it needs. Kept separate from the JSON code so the
 * parser stays dependency free.
 */
#ifndef U3_MCP_UTIL_H
#define U3_MCP_UTIL_H

#include <stddef.h>

typedef struct {
	char *data;
	size_t len;
	size_t cap;
} strbuf;

void sb_init(strbuf *sb);
void sb_free(strbuf *sb);
void sb_reset(strbuf *sb);
void sb_putc(strbuf *sb, char c);
void sb_put(strbuf *sb, const char *text);
void sb_putn(strbuf *sb, const char *text, size_t len);
void sb_printf(strbuf *sb, const char *fmt, ...);

/* Writes a JSON string literal, quotes and escapes included. */
void sb_json_string(strbuf *sb, const char *text);

/* Returns a newly allocated copy, never NULL. */
char *xstrdup(const char *text);

/* Trims leading and trailing whitespace in place, returns the new start. */
char *str_trim(char *text);

/* Directory part of a path without the trailing separator, "" when there is none. */
char *path_dirname(const char *path);

/* Joins two path components; the separator is only added when needed. */
char *path_join(const char *dir, const char *name);

/* Directory of the running executable, NULL when it cannot be determined. */
char *path_exe_dir(void);

#endif
