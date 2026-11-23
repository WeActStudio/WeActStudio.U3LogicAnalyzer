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
 * u3logicanalyzer-mcp-server: a Model Context Protocol server over stdio that
 * lets an agent drive the analyzer through u3logicanalyzer-cli.
 *
 * Protocol and tool set follow KenosInc/sigrok-mcp-server (MIT licence,
 * https://github.com/KenosInc/sigrok-mcp-server); this is an independent C
 * rewrite, see doc/mcp-server.md for the differences.
 *
 * JSON-RPC 2.0, one object per line on stdin and stdout. Everything that is
 * not protocol traffic goes to stderr, because a client that reads a stray
 * line as a response fails outright.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "tools.h"
#include "util.h"

#define SERVER_NAME "u3logicanalyzer-mcp-server"
#ifndef SERVER_VERSION
#error "SERVER_VERSION comes from CMake; build through mcp_server/CMakeLists.txt"
#endif
/* Echo the version the client asked for; clients are forgiving about newer
 * ones and this keeps older clients working. */
#define PROTOCOL_FALLBACK "2025-06-18"

#define JSONRPC_PARSE_ERROR (-32700)
#define JSONRPC_INVALID_REQUEST (-32600)
#define JSONRPC_METHOD_NOT_FOUND (-32601)
#define JSONRPC_INVALID_PARAMS (-32602)

static const char *const INSTRUCTIONS =
	"Drives the logic analyzer attached to this machine through "
	"u3logicanalyzer-cli, which ships next to this server. The board is "
	"usually a WeAct CH32H417, listed as wch-ch32h417 (CH375DLL backend) or "
	"wch-ch32h417-libusb (libusb backend); fx2lafw-compatible boards are "
	"listed as fx2lafw. Always call scan_devices first and pass on the driver "
	"identifier it reports. Typical order: scan_devices, show_driver_details "
	"for the sample rates and options of that driver, then capture_data or "
	"capture_and_decode, then decode_protocol for further analysis. Device "
	"options go in config, colon separated, for example "
	"samplerate=4m:voltage_threshold=1.2; range options need low-high. "
	"list_supported_decoders takes a filter to find a decoder quickly and "
	"show_decoder_details lists its channels, options and annotations. "
	"Captures are written to .sr files that the desktop application opens.";

static void send_text(const char *text)
{
	fputs(text, stdout);
	fputc('\n', stdout);
	fflush(stdout);
}

static void send_error(const char *id_text, int code, const char *message)
{
	strbuf out;

	sb_init(&out);
	sb_printf(&out, "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":",
		  id_text, code);
	sb_json_string(&out, message);
	sb_put(&out, "}}");
	send_text(out.data);
	sb_free(&out);
}

static void send_result(const char *id_text, const char *result_json)
{
	strbuf out;

	sb_init(&out);
	sb_printf(&out, "{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":", id_text);
	sb_put(&out, result_json);
	sb_putc(&out, '}');
	send_text(out.data);
	sb_free(&out);
}

/* One text item, which is what every MCP client can render. */
static void send_tool_content(const char *id_text, const char *text, int is_error)
{
	strbuf result, text_json;

	sb_init(&text_json);
	sb_json_string(&text_json, text);

	sb_init(&result);
	sb_put(&result, "{\"content\":[{\"type\":\"text\",\"text\":");
	sb_put(&result, text_json.data);
	sb_printf(&result, "}],\"isError\":%s}", is_error ? "true" : "false");
	send_result(id_text, result.data);
	sb_free(&result);
	sb_free(&text_json);
}

static void handle_initialize(const char *id_text, const json_value *params)
{
	const char *wanted = json_get_string(params, "protocolVersion", NULL);
	strbuf result;

	sb_init(&result);
	sb_put(&result, "{\"protocolVersion\":");
	sb_json_string(&result, wanted && *wanted ? wanted : PROTOCOL_FALLBACK);
	sb_put(&result, ",\"capabilities\":{\"tools\":{\"listChanged\":false}},"
			"\"serverInfo\":{\"name\":\"" SERVER_NAME "\",\"version\":"
			"\"" SERVER_VERSION "\"},\"instructions\":");
	sb_json_string(&result, INSTRUCTIONS);
	sb_putc(&result, '}');
	send_result(id_text, result.data);
	sb_free(&result);
}

static void handle_tools_list(const char *id_text)
{
	strbuf result;
	int first = 1;

	sb_init(&result);
	sb_put(&result, "{\"tools\":[");
	for (const mcp_tool *tool = tools_table(); tool->name; tool++) {
		if (!first)
			sb_putc(&result, ',');
		first = 0;
		sb_put(&result, "{\"name\":");
		sb_json_string(&result, tool->name);
		sb_put(&result, ",\"description\":");
		sb_json_string(&result, tool->description);
		sb_put(&result, ",\"inputSchema\":");
		sb_put(&result, tool->schema);
		sb_putc(&result, '}');
	}
	sb_put(&result, "]}");
	send_result(id_text, result.data);
	sb_free(&result);
}

static void handle_tools_call(const char *id_text, const json_value *params)
{
	const char *name = json_get_string(params, "name", NULL);
	const json_value *arguments = json_member(params, "arguments");
	const mcp_tool *tool;
	strbuf compact, pretty;
	char err[1024];
	int rc;

	if (!name || !*name) {
		send_error(id_text, JSONRPC_INVALID_PARAMS, "tools/call needs a tool name");
		return;
	}
	tool = tool_find(name);
	if (!tool) {
		snprintf(err, sizeof(err), "unknown tool '%s'", name);
		send_error(id_text, JSONRPC_INVALID_PARAMS, err);
		return;
	}
	/* Arguments may be absent for tools that take none. */
	if (arguments && arguments->type != JSON_OBJECT) {
		send_error(id_text, JSONRPC_INVALID_PARAMS, "arguments must be an object");
		return;
	}

	err[0] = '\0';
	sb_init(&compact);
	rc = tool->run(arguments, &compact, err, sizeof(err));
	if (rc != 0) {
		sb_free(&compact);
		send_tool_content(id_text, err[0] ? err : "the tool failed without a message", 1);
		return;
	}

	sb_init(&pretty);
	json_pretty(compact.data, compact.len, &pretty);
	send_tool_content(id_text, pretty.data, 0);
	sb_free(&pretty);
	sb_free(&compact);
}

static void handle_message(const char *line, size_t len)
{
	char err[256];
	char id_text[256];
	json_value *request;
	const char *method;
	const json_value *params;
	const json_value *id;

	request = json_parse(line, len, err, sizeof(err));
	if (!request) {
		snprintf(id_text, sizeof(id_text), "null");
		send_error(id_text, JSONRPC_PARSE_ERROR, err[0] ? err : "invalid JSON");
		return;
	}
	if (request->type != JSON_OBJECT) {
		json_free(request);
		send_error("null", JSONRPC_INVALID_REQUEST, "a request must be an object");
		return;
	}

	id = json_member(request, "id");
	json_id_text(id, id_text, sizeof(id_text));
	method = json_get_string(request, "method", NULL);
	params = json_member(request, "params");

	if (!method) {
		/* A response from the client; nothing to do. */
		if (!id) {
			json_free(request);
			return;
		}
		json_free(request);
		send_error(id_text, JSONRPC_INVALID_REQUEST, "missing method");
		return;
	}

	/* Notifications carry no id and are never answered. */
	int is_notification = (id == NULL);

	if (!strcmp(method, "initialize")) {
		if (!is_notification)
			handle_initialize(id_text, params);
	} else if (!strcmp(method, "tools/list")) {
		if (!is_notification)
			handle_tools_list(id_text);
	} else if (!strcmp(method, "tools/call")) {
		if (!is_notification)
			handle_tools_call(id_text, params);
	} else if (!strcmp(method, "ping")) {
		if (!is_notification)
			send_result(id_text, "{}");
	} else if (!strcmp(method, "resources/list")) {
		if (!is_notification)
			send_result(id_text, "{\"resources\":[]}");
	} else if (!strcmp(method, "resources/templates/list")) {
		if (!is_notification)
			send_result(id_text, "{\"resourceTemplates\":[]}");
	} else if (!strcmp(method, "prompts/list")) {
		if (!is_notification)
			send_result(id_text, "{\"prompts\":[]}");
	} else if (!strcmp(method, "logging/setLevel")) {
		if (!is_notification)
			send_result(id_text, "{}");
	} else if (!is_notification) {
		snprintf(err, sizeof(err), "unsupported method '%s'", method);
		send_error(id_text, JSONRPC_METHOD_NOT_FOUND, err);
	}

	json_free(request);
}

int main(void)
{
	char *line = NULL;
	size_t cap = 0, len = 0;
	int c;
	char *cli = tool_cli_path();

	fprintf(stderr, "%s %s: driving %s\n", SERVER_NAME, SERVER_VERSION, cli);
	free(cli);

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF)
			break;
		if (c == '\n') {
			if (len) {
				line[len] = '\0';
				handle_message(line, len);
				len = 0;
			}
			continue;
		}
		if (c == '\r')
			continue;
		if (len + 2 > cap) {
			size_t want = cap ? cap * 2 : 4096;
			char *grown = realloc(line, want);

			if (!grown) {
				fputs("out of memory\n", stderr);
				free(line);
				return 1;
			}
			line = grown;
			cap = want;
		}
		line[len++] = (char)c;
	}
	/* A last message without a trailing newline still counts. */
	if (len) {
		line[len] = '\0';
		handle_message(line, len);
	}
	free(line);
	return 0;
}
