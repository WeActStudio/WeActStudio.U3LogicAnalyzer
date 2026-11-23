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
 * The tool table exposed over MCP, and the u3logicanalyzer-cli invocations behind it.
 *
 * The set of tools follows KenosInc/sigrok-mcp-server (MIT) so an agent that
 * knows that server can talk to this one unchanged; the tools marked "extra"
 * are additions for the U3LogicAnalyzer: one round trip for capture + decode,
 * a way to apply device options without capturing, a filter for the decoder
 * list, and a firmware check that also looks where this package puts the
 * files.
 */
#ifndef U3_MCP_TOOLS_H
#define U3_MCP_TOOLS_H

#include <stddef.h>

#include "json.h"
#include "util.h"

typedef struct {
	const char *name;
	const char *description;
	const char *schema;	/* JSON text of the inputSchema */
	int (*run)(const json_value *args, strbuf *result, char *err, size_t errlen);
} mcp_tool;

/* NULL terminated. */
const mcp_tool *tools_table(void);

/* Lookup by name, NULL when there is no such tool. */
const mcp_tool *tool_find(const char *name);

/* Absolute path of the u3logicanalyzer-cli this server drives, for diagnostics. */
char *tool_cli_path(void);

#endif
