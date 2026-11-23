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
 * Running u3logicanalyzer-cli and collecting its output. u3logicanalyzer-cli is the only
 * interface to libsigrok here: the MCP server stays a plain executable with
 * no library dependencies, so it can be shipped on every platform we build.
 */
#ifndef U3_MCP_PROC_H
#define U3_MCP_PROC_H

#include <stddef.h>

#include "util.h"

typedef struct {
	int started;	/* 0 when the process could not be created */
	int exit_code;	/* -1 when it was killed or the code is unknown */
	int timed_out;
	strbuf out;
	strbuf err;
} proc_result;

void proc_result_init(proc_result *result);
void proc_result_free(proc_result *result);

/*
 * Runs argv[0] (looked up in PATH) with the given arguments and captures
 * both streams. Returns 0 when the process was created and waited for, -1
 * when it could not be created; a non-zero exit code is not a failure here,
 * the caller decides what it means.
 */
int proc_run(char *const argv[], const char *workdir, int timeout_sec,
	     proc_result *result, char *err, size_t errlen);

#endif
