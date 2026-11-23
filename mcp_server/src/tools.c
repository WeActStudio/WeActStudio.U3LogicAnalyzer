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

#include "tools.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "proc.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <dirent.h>
# include <strings.h>
# include <sys/stat.h>
#endif

#define DEFAULT_TIMEOUT_SECONDS 30
#define MAX_TEXT_OUTPUT (1024 * 1024)

static char *dup_range(const char *start, size_t len)
{
	char *copy = malloc(len + 1);

	if (!copy) {
		fputs("out of memory\n", stderr);
		exit(1);
	}
	memcpy(copy, start, len);
	copy[len] = '\0';
	return copy;
}

#ifdef _WIN32
/* The ANSI APIs mis-read anything outside the system code page, and a capture
 * path may well be Chinese or contain spaces the user typed at another level. */
static int utf8_to_wide(const char *text, wchar_t *out, int count)
{
	int need = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);

	if (need <= 0 || need > count)
		return 0;
	return MultiByteToWideChar(CP_UTF8, 0, text, -1, out, count) > 0;
}
#endif

static long long file_size(const char *path)
{
#ifdef _WIN32
	wchar_t wide[4096];
	WIN32_FILE_ATTRIBUTE_DATA info;

	if (!utf8_to_wide(path, wide, (int)(sizeof(wide) / sizeof(wide[0]))) ||
	    !GetFileAttributesExW(wide, GetFileExInfoStandard, &info))
		return 0;
	return ((long long)info.nFileSizeHigh << 32) | info.nFileSizeLow;
#else
	struct stat st;

	if (stat(path, &st) != 0)
		return 0;
	return (long long)st.st_size;
#endif
}

/* ==========================================================================
 * Driving u3logicanalyzer-cli
 * ========================================================================== */

/* Has to answer for directories too: the decoder sources are looked up as
 * directories, and fopen() fails on those under Windows. */
static int file_exists(const char *path)
{
#ifdef _WIN32
	wchar_t wide[4096];

	return utf8_to_wide(path, wide, (int)(sizeof(wide) / sizeof(wide[0]))) &&
	       GetFileAttributesW(wide) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat st;

	return stat(path, &st) == 0;
#endif
}

char *tool_cli_path(void)
{
	const char *env = getenv("U3LOGICANALYZER_CLI_PATH");
	char *exe_dir, *candidate;

	if (env && *env)
		return xstrdup(env);

	exe_dir = path_exe_dir();
	if (!exe_dir)
		return xstrdup("u3logicanalyzer-cli");

	/* Shipped next to this server, exactly like the application does it. */
#ifdef _WIN32
	candidate = path_join(exe_dir, "u3logicanalyzer-cli.exe");
#else
	candidate = path_join(exe_dir, "u3logicanalyzer-cli");
#endif
	free(exe_dir);
	if (file_exists(candidate))
		return candidate;
	free(candidate);
	return xstrdup("u3logicanalyzer-cli");
}

static int run_cli(const char *const *args, proc_result *result, char *err, size_t errlen)
{
	const char *workdir = getenv("U3LOGICANALYZER_WORKING_DIR");
	const char *timeout_env = getenv("U3LOGICANALYZER_TIMEOUT_SECONDS");
	int timeout = DEFAULT_TIMEOUT_SECONDS;
	char *cli = tool_cli_path();
	char **argv;
	size_t count = 0;
	int rc;

	if (timeout_env && *timeout_env) {
		int parsed = atoi(timeout_env);

		if (parsed > 0)
			timeout = parsed;
	}
	while (args[count])
		count++;
	argv = calloc(count + 2, sizeof(*argv));
	if (!argv) {
		free(cli);
		snprintf(err, errlen, "out of memory");
		return -1;
	}
	argv[0] = cli;
	for (size_t i = 0; i < count; i++)
		argv[i + 1] = (char *)args[i];
	argv[count + 1] = NULL;

	proc_result_init(result);
	rc = proc_run(argv, (workdir && *workdir) ? workdir : NULL, timeout, result,
		      err, errlen);
	free(argv);
	free(cli);
	if (rc != 0) {
		proc_result_free(result);
		return -1;
	}
	return 0;
}

/* Turns a failed run into a message an agent can act on. */
static void cli_failure(const proc_result *result, char *err, size_t errlen)
{
	const char *detail = result->err.len ? result->err.data : result->out.data;
	char buffer[900];
	char *trimmed;

	snprintf(buffer, sizeof(buffer), "%s", detail ? detail : "");
	trimmed = str_trim(buffer);
	if (result->timed_out)
		snprintf(err, errlen, "u3logicanalyzer-cli timed out (raise U3LOGICANALYZER_TIMEOUT_SECONDS): %s",
			 trimmed);
	else if (result->exit_code == 127)
		snprintf(err, errlen, "u3logicanalyzer-cli could not be started; set U3LOGICANALYZER_CLI_PATH: %s",
			 trimmed);
	else
		snprintf(err, errlen, "u3logicanalyzer-cli exited with %d: %s", result->exit_code,
			 trimmed[0] ? trimmed : "(no output)");
}

static const char *first_of(const proc_result *result)
{
	return result->err.len ? result->err.data : result->out.data;
}

/* ==========================================================================
 * Parsing the text output of u3logicanalyzer-cli -L
 * ========================================================================== */

/*
 * Copies one "  id  description" line into id and description. The caller
 * passes a writable line; the id is terminated in place.
 */
static void split_list_line(char *line, char **id, char **description)
{
	char *p = line;

	while (*p && !isspace((unsigned char)*p))
		p++;
	*id = line;
	if (*p)
		*p++ = '\0';
	while (*p && isspace((unsigned char)*p))
		p++;
	*description = p;
}

static int contains_ci(const char *haystack, const char *needle)
{
	size_t nlen = strlen(needle);

	if (!nlen)
		return 1;
	for (; *haystack; haystack++) {
		if (!strncasecmp(haystack, needle, nlen))
			return 1;
	}
	return 0;
}

/*
 * Emits the entries of one section of -L as JSON objects, separated by commas
 * but without the surrounding brackets. Returns the number of entries.
 */
static size_t list_section(const char *text, const char *header, const char *filter,
			   strbuf *items)
{
	const char *line = text;
	int inside = 0;
	size_t count = 0;

	while (line && *line) {
		const char *eol = strchr(line, '\n');
		size_t len = eol ? (size_t)(eol - line) : strlen(line);
		char buffer[1024];
		size_t copy = len < sizeof(buffer) ? len : sizeof(buffer) - 1;
		char *trimmed;

		memcpy(buffer, line, copy);
		buffer[copy] = '\0';
		trimmed = str_trim(buffer);

		if (!inside) {
			inside = !strcmp(trimmed, header);
		} else if (!*trimmed) {
			break;
		} else {
			char *id, *description;

			split_list_line(trimmed, &id, &description);
			if (*id && (!filter || !*filter ||
				    contains_ci(id, filter) || contains_ci(description, filter))) {
				if (count)
					sb_putc(items, ',');
				sb_put(items, "{\"id\":");
				sb_json_string(items, id);
				sb_put(items, ",\"description\":");
				sb_json_string(items, description);
				sb_putc(items, '}');
				count++;
			}
		}
		if (!eol)
			break;
		line = eol + 1;
	}
	return count;
}

/* Wraps the shared body of the four list_* tools. */
static int list_tool(const char *header, const char *field, const char *filter,
		     strbuf *result, char *err, size_t errlen)
{
	const char *argv[] = {"-L", NULL};
	proc_result run;
	strbuf items;
	size_t count;

	if (run_cli(argv, &run, err, errlen) != 0)
		return -1;
	if (run.exit_code != 0) {
		cli_failure(&run, err, errlen);
		proc_result_free(&run);
		return -1;
	}

	sb_init(&items);
	count = list_section(run.out.data, header, filter, &items);
	sb_printf(result, "{\"%s\":[%s],\"count\":%lu}", field, items.data,
		  (unsigned long)count);
	sb_free(&items);
	proc_result_free(&run);
	return 0;
}

/* ==========================================================================
 * list_* tools
 * ========================================================================== */

static int tool_list_hardware(const json_value *args, strbuf *result, char *err,
			      size_t errlen)
{
	(void)args;
	return list_tool("Supported hardware drivers:", "drivers", NULL, result, err,
			 errlen);
}

static int tool_list_decoders(const json_value *args, strbuf *result, char *err,
			      size_t errlen)
{
	const char *filter = json_get_string(args, "filter", NULL);

	return list_tool("Supported protocol decoders:", "decoders", filter, result,
			 err, errlen);
}

static int tool_list_input_formats(const json_value *args, strbuf *result, char *err,
				   size_t errlen)
{
	(void)args;
	return list_tool("Supported input formats:", "formats", NULL, result, err,
			 errlen);
}

static int tool_list_output_formats(const json_value *args, strbuf *result, char *err,
				    size_t errlen)
{
	(void)args;
	return list_tool("Supported output formats:", "formats", NULL, result, err,
			 errlen);
}

/* ==========================================================================
 * show_version
 * ========================================================================== */

/* Value that follows a label at the start of a line, e.g. "- libsigrok 0.6.0". */
static void copy_after(const char *line, const char *label, char *out, size_t outlen)
{
	const char *p = strstr(line, label);

	*out = '\0';
	if (!p)
		return;
	p += strlen(label);
	while (*p && isspace((unsigned char)*p))
		p++;
	for (size_t i = 0; *p && !isspace((unsigned char)*p) && i + 1 < outlen; i++, p++)
		out[i] = *p;
	out[outlen - 1] = '\0';
}

static int tool_show_version(const json_value *args, strbuf *result, char *err,
			     size_t errlen)
{
	const char *argv[] = {"--version", NULL};
	proc_result run;
	char cli_version[128] = "", sr[128] = "", srd[128] = "";

	(void)args;
	if (run_cli(argv, &run, err, errlen) != 0)
		return -1;

	for (const char *line = run.out.data; line && *line; ) {
		const char *eol = strchr(line, '\n');
		char buffer[512];
		size_t len = eol ? (size_t)(eol - line) : strlen(line);

		const char *space;

		if (len >= sizeof(buffer))
			len = sizeof(buffer) - 1;
		memcpy(buffer, line, len);
		buffer[len] = '\0';
		space = strchr(buffer, ' ');
		/* First line is "<program> <version>", whatever the program is called. */
		if (!*cli_version && space && strncmp(buffer, "libsigrok", 9)) {
			snprintf(cli_version, sizeof(cli_version), "%s", space + 1);
			/* Windows ends the line with CRLF. */
			str_trim(cli_version);
		}
		else if (strstr(buffer, "libsigrok ") && !strstr(buffer, "libsigrokdecode"))
			copy_after(buffer, "libsigrok ", sr, sizeof(sr));
		else if (strstr(buffer, "libsigrokdecode "))
			copy_after(buffer, "libsigrokdecode ", srd, sizeof(srd));
		if (!eol)
			break;
		line = eol + 1;
	}

	sb_printf(result, "{\"cli_version\":");
	sb_json_string(result, cli_version);
	sb_put(result, ",\"libsigrok_version\":");
	sb_json_string(result, sr);
	sb_put(result, ",\"libsigrokdecode_version\":");
	sb_json_string(result, srd);
	sb_put(result, ",\"raw_output\":");
	sb_json_string(result, run.out.data);
	sb_putc(result, '}');
	proc_result_free(&run);
	return 0;
}

/* ==========================================================================
 * scan_devices
 * ========================================================================== */

static int tool_scan_devices(const json_value *args, strbuf *result, char *err,
			     size_t errlen)
{
	const char *driver = json_get_string(args, "driver", NULL);
	const char *conn = json_get_string(args, "conn", NULL);
	char *spec = NULL;
	const char *argv[5];
	int argc = 0;
	proc_result run;
	size_t count = 0;

	if (driver && *driver) {
		spec = conn && *conn ? NULL : xstrdup(driver);
		if (!spec) {
			size_t need = strlen(driver) + strlen(conn) + 8;

			spec = malloc(need);
			if (!spec) {
				snprintf(err, errlen, "out of memory");
				return -1;
			}
			snprintf(spec, need, "%s:conn=%s", driver, conn);
		}
		argv[argc++] = "-d";
		argv[argc++] = spec;
	}
	argv[argc++] = "--scan";
	argv[argc] = NULL;

	if (run_cli(argv, &run, err, errlen) != 0) {
		free(spec);
		return -1;
	}
	free(spec);

	sb_put(result, "{\"devices\":[");
	for (const char *line = run.out.data; line && *line; ) {
		const char *eol = strchr(line, '\n');
		size_t len = eol ? (size_t)(eol - line) : strlen(line);
		char buffer[1024];
		size_t copy = len < sizeof(buffer) ? len : sizeof(buffer) - 1;
		char *trimmed, *dash;

		memcpy(buffer, line, copy);
		buffer[copy] = '\0';
		trimmed = str_trim(buffer);
		/* "driver - description"; the header line has no " - ". */
		dash = strstr(trimmed, " - ");
		if (*trimmed && dash && strncmp(trimmed, "The following", 13)) {
			*dash = '\0';
			if (count)
				sb_putc(result, ',');
			sb_put(result, "{\"driver\":");
			sb_json_string(result, trimmed);
			sb_put(result, ",\"description\":");
			sb_json_string(result, str_trim(dash + 3));
			sb_putc(result, '}');
			count++;
		}
		if (!eol)
			break;
		line = eol + 1;
	}
	sb_printf(result, "],\"count\":%lu", (unsigned long)count);
	if (!count) {
		sb_put(result, ",\"hint\":\"No device answered. Check the cable, the "
			       "driver binding (Zadig on Windows, udev on Linux) and "
			       "that the board is not in bootloader mode.\"");
	} else if (run.err.len) {
		sb_put(result, ",\"warnings\":");
		sb_json_string(result, run.err.data);
	}
	sb_putc(result, '}');
	proc_result_free(&run);
	return 0;
}

/* ==========================================================================
 * show_driver_details / show device capabilities
 * ========================================================================== */

/* Collects the indented lines below one header into a JSON array. */
static void collect_block(const char *text, const char *header, strbuf *array)
{
	const char *line = text;
	int inside = 0;
	size_t count = 0;

	sb_putc(array, '[');
	while (line && *line) {
		const char *eol = strchr(line, '\n');
		size_t len = eol ? (size_t)(eol - line) : strlen(line);
		char buffer[1024];
		size_t copy = len < sizeof(buffer) ? len : sizeof(buffer) - 1;
		char *trimmed;
		int indented = copy >= 2 && (line[0] == ' ' || line[0] == '\t');

		memcpy(buffer, line, copy);
		buffer[copy] = '\0';
		trimmed = str_trim(buffer);

		if (!inside) {
			if (!strcmp(trimmed, header))
				inside = 1;
		} else if (!*trimmed) {
			break;
		} else if (indented) {
			if (count)
				sb_putc(array, ',');
			sb_json_string(array, trimmed);
			count++;
		} else {
			break;
		}
		if (!eol)
			break;
		line = eol + 1;
	}
	sb_putc(array, ']');
}

static int tool_show_driver_details(const json_value *args, strbuf *result, char *err,
				    size_t errlen)
{
	const char *driver = json_get_string(args, "driver", "");
	const char *conn = json_get_string(args, "conn", NULL);
	char *spec;
	const char *argv[4];
	proc_result run;
	strbuf functions, scan_options, channels, options;

	if (!*driver) {
		snprintf(err, errlen, "driver is required");
		return -1;
	}
	if (conn && *conn) {
		size_t need = strlen(driver) + strlen(conn) + 8;

		spec = malloc(need);
		if (!spec) {
			snprintf(err, errlen, "out of memory");
			return -1;
		}
		snprintf(spec, need, "%s:conn=%s", driver, conn);
	} else {
		spec = xstrdup(driver);
	}

	argv[0] = "-d";
	argv[1] = spec;
	argv[2] = "--show";
	argv[3] = NULL;
	if (run_cli(argv, &run, err, errlen) != 0) {
		free(spec);
		return -1;
	}
	free(spec);

	if (run.exit_code != 0) {
		cli_failure(&run, err, errlen);
		proc_result_free(&run);
		return -1;
	}

	sb_init(&functions);
	sb_init(&scan_options);
	sb_init(&channels);
	sb_init(&options);
	collect_block(run.out.data, "Driver functions:", &functions);
	collect_block(run.out.data, "Scan options:", &scan_options);
	collect_block(run.out.data, "Channel groups:", &channels);
	collect_block(run.out.data, "Supported configuration options across all "
				    "channel groups:", &options);

	sb_put(result, "{\"driver\":");
	sb_json_string(result, driver);
	sb_put(result, ",\"functions\":");
	sb_put(result, functions.data);
	sb_put(result, ",\"scan_options\":");
	sb_put(result, scan_options.data);
	sb_put(result, ",\"channel_groups\":");
	sb_put(result, channels.data);
	sb_put(result, ",\"options\":");
	sb_put(result, options.data);
	sb_put(result, ",\"raw_output\":");
	sb_json_string(result, run.out.data);
	sb_putc(result, '}');

	sb_free(&functions);
	sb_free(&scan_options);
	sb_free(&channels);
	sb_free(&options);
	proc_result_free(&run);
	return 0;
}

/* ==========================================================================
 * show_decoder_details, read from the decoder sources
 * ========================================================================== */

static char *decoder_dir(void)
{
	static const char *const candidates[] = {
		"share/libsigrokdecode/decoders",
		"decoders",
		"../share/libsigrokdecode/decoders",
		"../Resources/decoders",
	};
	const char *env = getenv("SIGROKDECODE_DIR");
	char *exe_dir;
	char *path;

	if (env && *env)
		return xstrdup(env);

	exe_dir = path_exe_dir();
	if (!exe_dir)
		return NULL;
	for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
		path = path_join(exe_dir, candidates[i]);
		if (file_exists(path)) {
			free(exe_dir);
			return path;
		}
		free(path);
	}
	free(exe_dir);
	return NULL;
}

static char *read_file(const char *path)
{
	FILE *f = fopen(path, "rb");
	long size;
	char *data;

	if (!f)
		return NULL;
	if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) < 0) {
		fclose(f);
		return NULL;
	}
	rewind(f);
	data = malloc((size_t)size + 1);
	if (!data) {
		fclose(f);
		return NULL;
	}
	if (fread(data, 1, (size_t)size, f) != (size_t)size) {
		free(data);
		fclose(f);
		return NULL;
	}
	data[size] = '\0';
	fclose(f);
	return data;
}

/* Text between the first pair of ''' delimiters, which is the docstring. */
static char *python_docstring(const char *source)
{
	const char *start = strstr(source, "'''");
	const char *end;
	char *doc, *trimmed;

	if (!start)
		return NULL;
	start += 3;
	end = strstr(start, "'''");
	if (!end)
		return NULL;
	doc = dup_range(start, (size_t)(end - start));
	/* The decoder sources are checked out with CRLF on Windows; the docstring
	 * reads better normalised. */
	for (char *p = doc; *p; p++) {
		if (p[0] == '\r' && p[1] == '\n') {
			memmove(p, p + 1, strlen(p + 1) + 1);
			p--;
		}
	}
	trimmed = str_trim(doc);
	if (trimmed != doc)
		memmove(doc, trimmed, strlen(trimmed) + 1);
	return doc;
}

/* Value that follows "'key':" in one of the decoder's dict literals. */
static char *python_value(const char *chunk, const char *key)
{
	char pattern[64];
	const char *p;
	strbuf value;

	snprintf(pattern, sizeof(pattern), "'%s':", key);
	p = strstr(chunk, pattern);
	if (!p)
		return NULL;
	p += strlen(pattern);
	while (*p == ' ' || *p == '\t' || *p == '\n')
		p++;

	sb_init(&value);
	if (*p == '\'' || *p == '"') {
		char quote = *p++;

		while (*p && *p != quote)
			sb_putc(&value, *p++);
	} else {
		while (*p && *p != ',' && *p != '}' && *p != '\n')
			sb_putc(&value, *p++);
		{
			char *trimmed = str_trim(value.data);

			if (trimmed != value.data)
				memmove(value.data, trimmed, strlen(trimmed) + 1);
			value.len = strlen(value.data);
		}
	}
	return value.data;
}

/*
 * Turns the dict literals of a tuple into a JSON array of {id, desc, default},
 * which is what an agent can act on without reading Python.
 */
static size_t python_dicts_to_json(const char *block, strbuf *array)
{
	const char *p = block;
	size_t count = 0;

	sb_putc(array, '[');
	while ((p = strchr(p, '{')) != NULL) {
		const char *end = strchr(p, '}');
		char *chunk, *id, *description, *def;

		if (!end)
			break;
		chunk = dup_range(p, (size_t)(end - p) + 1);
		id = python_value(chunk, "id");
		description = python_value(chunk, "desc");
		def = python_value(chunk, "default");
		if (id) {
			if (count)
				sb_putc(array, ',');
			sb_put(array, "{\"id\":");
			sb_json_string(array, id);
			sb_put(array, ",\"desc\":");
			sb_json_string(array, description ? description : "");
			sb_put(array, ",\"default\":");
			sb_json_string(array, def ? def : "");
			sb_putc(array, '}');
			count++;
		}
		free(chunk);
		free(id);
		free(description);
		free(def);
		p = end + 1;
	}
	sb_putc(array, ']');
	return count;
}

/*
 * The tuple that declares a decoder's meta data, e.g. "options = (". Returns
 * the text of the assignment, or NULL.
 */
static char *python_block(const char *source, const char *name)
{
	const char *start = strstr(source, name);
	strbuf block;
	int depth = 0;

	if (!start)
		return NULL;
	start = strchr(start, '(');
	if (!start)
		return NULL;
	sb_init(&block);
	for (const char *p = start; *p; p++) {
		if (*p == '(')
			depth++;
		else if (*p == ')')
			depth--;
		sb_putc(&block, *p);
		if (depth == 0)
			return block.data;
	}
	sb_free(&block);
	return NULL;
}

static int tool_show_decoder_details(const json_value *args, strbuf *result, char *err,
				     size_t errlen)
{
	const char *decoder = json_get_string(args, "decoder", "");
	char *dir = NULL, *init_path = NULL, *pd_path = NULL;
	char *init_src = NULL, *pd_src = NULL, *doc = NULL, *options = NULL;
	char *channels = NULL, *annotations = NULL;
	const char *argv[] = {"-L", NULL};
	proc_result run;
	strbuf items;
	size_t count;

	if (!*decoder) {
		snprintf(err, errlen, "decoder is required");
		return -1;
	}

	/* Confirm the id and take its one-line description from u3logicanalyzer-cli. */
	if (run_cli(argv, &run, err, errlen) == 0) {
		sb_init(&items);
		count = list_section(run.out.data, "Supported protocol decoders:", decoder,
				     &items);
		proc_result_free(&run);
		if (!count) {
			sb_free(&items);
			snprintf(err, errlen, "no protocol decoder called '%s'", decoder);
			return -1;
		}
		sb_free(&items);
	}

	dir = decoder_dir();
	if (!dir) {
		snprintf(err, errlen, "decoder sources not found; set SIGROKDECODE_DIR");
		return -1;
	}
	{
		char *sub = path_join(dir, decoder);
		char *name = path_join(sub, "__init__.py");

		init_path = name;
		name = path_join(sub, "pd.py");
		pd_path = name;
		free(sub);
	}

	init_src = read_file(init_path);
	pd_src = read_file(pd_path);
	if (!init_src && !pd_src) {
		snprintf(err, errlen, "no sources for decoder '%s' in %s", decoder, dir);
		free(init_path);
		free(pd_path);
		free(dir);
		return -1;
	}
	if (init_src)
		doc = python_docstring(init_src);
	if (pd_src) {
		options = python_block(pd_src, "options = (");
		channels = python_block(pd_src, "channels = (");
		if (!channels)
			channels = python_block(pd_src, "optional_channels = (");
		annotations = python_block(pd_src, "annotations = (");
	}

	sb_put(result, "{\"decoder\":");
	sb_json_string(result, decoder);
	sb_put(result, ",\"doc\":");
	sb_json_string(result, doc ? doc : "");
	{
		strbuf options_json, channels_json;

		sb_init(&options_json);
		sb_init(&channels_json);
		python_dicts_to_json(options ? options : "", &options_json);
		python_dicts_to_json(channels ? channels : "", &channels_json);
		sb_put(result, ",\"options\":");
		sb_put(result, options_json.data);
		sb_put(result, ",\"channels\":");
		sb_put(result, channels_json.data);
		sb_free(&options_json);
		sb_free(&channels_json);
	}
	sb_put(result, ",\"annotations\":");
	sb_json_string(result, annotations ? annotations : "");
	sb_put(result, ",\"source\":");
	sb_json_string(result, pd_path ? pd_path : init_path);
	sb_putc(result, '}');

	free(doc);
	free(options);
	free(channels);
	free(annotations);
	free(init_src);
	free(pd_src);
	free(init_path);
	free(pd_path);
	free(dir);
	return 0;
}

/* ==========================================================================
 * capture_data
 * ========================================================================== */

static void timestamp_name(char *out, size_t outlen, const char *extension)
{
	time_t now = time(NULL);
	struct tm parts;

#ifdef _WIN32
	localtime_s(&parts, &now);
#else
	localtime_r(&now, &parts);
#endif
	snprintf(out, outlen, "capture-%04d%02d%02d-%02d%02d%02d.%s",
		 parts.tm_year + 1900, parts.tm_mon + 1, parts.tm_mday,
		 parts.tm_hour, parts.tm_min, parts.tm_sec, extension);
}

static int is_absolute_path(const char *path)
{
#ifdef _WIN32
	if (isalpha((unsigned char)path[0]) && path[1] == ':')
		return 1;
	return path[0] == '\\' && path[1] == '\\';
#else
	return path[0] == '/';
#endif
}

/* The CLI runs with U3LOGICANALYZER_WORKING_DIR as its current directory, so a
 * bare name is resolved there, not in this process. Handing an absolute path
 * back keeps the size check and the agent's next call pointing at the file. */
static char *absolute_in_workdir(const char *path)
{
	const char *workdir = getenv("U3LOGICANALYZER_WORKING_DIR");

	if (!path || !*path)
		return xstrdup(path ? path : "");
	if (!workdir || !*workdir || is_absolute_path(path))
		return xstrdup(path);
	return path_join(workdir, path);
}

/*
 * Turns what a caller wrote into the form u3logicanalyzer-cli expects.
 *
 * Two things differ from the obvious guess. Options are separated by ':',
 * while a comma is what everybody writes first and the resulting error names
 * the wrong option ("Invalid value: '4m,voltage_threshold=...' for option
 * 'samplerate'"), so a comma-only string is accepted as well. And an option
 * declared as SR_T_DOUBLE_RANGE ("voltage_threshold") takes "low-high" and
 * rejects a bare number, so a single value is widened into "v-v"; other types
 * are left alone, otherwise "logic_channels=8" would become "8-8".
 *
 * Returns a new string; the caller frees it.
 */
static const char *const range_keys[] = {
	"voltage_threshold",
	"spl_meas_range",
};

static int is_bare_number(const char *text, size_t len)
{
	int seen_digit = 0, seen_dot = 0;
	size_t i = 0;

	if (!len)
		return 0;
	if (text[0] == '-' || text[0] == '+')
		i++;
	for (; i < len; i++) {
		if (text[i] >= '0' && text[i] <= '9')
			seen_digit = 1;
		else if (text[i] == '.' && !seen_dot)
			seen_dot = 1;
		else
			return 0;
	}
	return seen_digit;
}

static int is_range_key(const char *key, size_t len)
{
	size_t i;

	for (i = 0; i < sizeof(range_keys) / sizeof(range_keys[0]); i++) {
		if (strlen(range_keys[i]) == len && !strncmp(range_keys[i], key, len))
			return 1;
	}
	return 0;
}

static char *prepare_config(const char *config)
{
	char separator = strchr(config, ':') ? ':' : ',';
	strbuf out;
	const char *p = config;

	sb_init(&out);
	while (*p) {
		const char *end = strchr(p, separator);
		size_t len = end ? (size_t)(end - p) : strlen(p);
		const char *eq = memchr(p, '=', len);

		if (eq && is_range_key(p, (size_t)(eq - p))) {
			const char *value = eq + 1;
			size_t value_len = len - (size_t)(value - p);

			if (is_bare_number(value, value_len)) {
				sb_putn(&out, p, (size_t)(value - p));
				sb_putn(&out, value, value_len);
				sb_putc(&out, '-');
				sb_putn(&out, value, value_len);
			} else {
				sb_putn(&out, p, len);
			}
		} else {
			sb_putn(&out, p, len);
		}
		if (!end)
			break;
		sb_putc(&out, ':');
		p = end + 1;
	}
	return out.data;
}

/* driver with an optional conn= suffix appended. */
static char *driver_spec(const char *driver, const char *conn)
{
	size_t need;
	char *spec;

	if (!conn || !*conn)
		return xstrdup(driver);
	need = strlen(driver) + strlen(conn) + 8;
	spec = malloc(need);
	if (!spec) {
		fputs("out of memory\n", stderr);
		exit(1);
	}
	snprintf(spec, need, "%s:conn=%s", driver, conn);
	return spec;
}

static int do_capture(const json_value *args, char **output_file, strbuf *capture_log,
		      char *err, size_t errlen)
{
	const char *driver = json_get_string(args, "driver", NULL);
	const char *conn = json_get_string(args, "conn", NULL);
	const char *config = json_get_string(args, "config", NULL);
	const char *chfields = json_get_string(args, "channels", NULL);
	const char *triggers = json_get_string(args, "triggers", NULL);
	int wait_trigger = json_get_bool(args, "wait_trigger", 0);
	double samples = json_get_number(args, "samples", 0);
	double time_ms = json_get_number(args, "time", 0);
	const char *given_file = json_get_string(args, "output_file", NULL);
	char generated[64];
	char amount[32];
	const char *argv[20];
	int argc = 0;
	proc_result run;
	char *spec;
	char *config_expanded;

	if (!driver || !*driver) {
		snprintf(err, errlen, "driver is required (use scan_devices to find one)");
		return -1;
	}
	if (samples <= 0 && time_ms <= 0) {
		snprintf(err, errlen, "either samples or time is required");
		return -1;
	}

	if (given_file && *given_file) {
		*output_file = absolute_in_workdir(given_file);
	} else {
		timestamp_name(generated, sizeof(generated), "sr");
		*output_file = absolute_in_workdir(generated);
	}

	spec = driver_spec(driver, conn);
	config_expanded = (config && *config) ? prepare_config(config) : NULL;
	argv[argc++] = "-d";
	argv[argc++] = spec;
	if (config_expanded) {
		argv[argc++] = "--config";
		argv[argc++] = config_expanded;
	}
	if (chfields && *chfields) {
		argv[argc++] = "-C";
		argv[argc++] = chfields;
	}
	if (samples > 0) {
		argv[argc++] = "--samples";
		snprintf(amount, sizeof(amount), "%ld", (long)samples);
		argv[argc++] = amount;
	} else {
		argv[argc++] = "--time";
		snprintf(amount, sizeof(amount), "%ld", (long)time_ms);
		argv[argc++] = amount;
	}
	if (triggers && *triggers) {
		argv[argc++] = "--triggers";
		argv[argc++] = triggers;
	}
	if (wait_trigger) {
		argv[argc++] = "--wait-trigger";
		argv[argc++] = "1";
	}
	argv[argc++] = "--output-file";
	argv[argc++] = *output_file;
	argv[argc] = NULL;

	int rc = run_cli(argv, &run, err, errlen);

	free(spec);
	free(config_expanded);
	if (rc != 0)
		return -1;
	if (run.exit_code != 0) {
		cli_failure(&run, err, errlen);
		proc_result_free(&run);
		return -1;
	}
	sb_reset(capture_log);
	sb_put(capture_log, first_of(&run));
	proc_result_free(&run);
	return 0;
}

static int tool_capture_data(const json_value *args, strbuf *result, char *err,
			     size_t errlen)
{
	char *output_file = NULL;
	strbuf log;

	sb_init(&log);
	if (do_capture(args, &output_file, &log, err, errlen) != 0) {
		sb_free(&log);
		free(output_file);
		return -1;
	}

	sb_put(result, "{\"output_file\":");
	sb_json_string(result, output_file);
	sb_put(result, ",\"bytes\":");
	sb_printf(result, "%lld", file_size(output_file));
	sb_put(result, ",\"log\":");
	sb_json_string(result, log.data);
	sb_put(result, ",\"next\":\"decode_protocol with input_file set to this path, "
		       "or capture_and_decode next time\"}");
	sb_free(&log);
	free(output_file);
	return 0;
}

/* ==========================================================================
 * decode_protocol
 * ========================================================================== */

static int do_decode(const json_value *args, const char *input_file, strbuf *output,
		     char *err, size_t errlen)
{
	const char *decoders = json_get_string(args, "protocol_decoders", NULL);
	const char *format = json_get_string(args, "input_format", NULL);
	const char *annotations = json_get_string(args, "annotations", NULL);
	const char *meta = json_get_string(args, "meta_decoders", NULL);
	if (!meta)
		meta = json_get_string(args, "meta_output", NULL);
	int samplenum = json_get_bool(args, "show_sample_numbers", 0);
	int jsontrace = json_get_bool(args, "json_trace", 0);
	const char *argv[16];
	int argc = 0;
	proc_result run;
	int rc;

	if (!decoders || !*decoders) {
		snprintf(err, errlen, "protocol_decoders is required, e.g. "
				      "'uart:baudrate=115200'");
		return -1;
	}
	argv[argc++] = "--input-file";
	argv[argc++] = input_file;
	if (format && *format) {
		argv[argc++] = "--input-format";
		argv[argc++] = format;
	}
	argv[argc++] = "--protocol-decoders";
	argv[argc++] = decoders;
	if (annotations && *annotations) {
		argv[argc++] = "--protocol-decoder-annotations";
		argv[argc++] = annotations;
	}
	if (meta && *meta) {
		argv[argc++] = "--protocol-decoder-meta";
		argv[argc++] = meta;
	}
	if (samplenum)
		argv[argc++] = "--protocol-decoder-samplenum";
	if (jsontrace)
		argv[argc++] = "--protocol-decoder-jsontrace";
	argv[argc] = NULL;

	rc = run_cli(argv, &run, err, errlen);
	if (rc != 0)
		return -1;
	if (run.exit_code != 0) {
		cli_failure(&run, err, errlen);
		proc_result_free(&run);
		return -1;
	}
	sb_reset(output);
	sb_put(output, run.out.data);
	proc_result_free(&run);
	return 0;
}

static int tool_decode_protocol(const json_value *args, strbuf *result, char *err,
				size_t errlen)
{
	const char *input_file = json_get_string(args, "input_file", "");
	strbuf output;
	int truncated;

	if (!*input_file) {
		snprintf(err, errlen, "input_file is required");
		return -1;
	}
	sb_init(&output);
	if (do_decode(args, input_file, &output, err, errlen) != 0) {
		sb_free(&output);
		return -1;
	}

	truncated = output.len > MAX_TEXT_OUTPUT;
	if (truncated)
		output.data[MAX_TEXT_OUTPUT] = '\0';
	sb_put(result, "{\"input_file\":");
	sb_json_string(result, input_file);
	sb_put(result, ",\"output\":");
	sb_json_string(result, output.data);
	sb_printf(result, ",\"truncated\":%s}", truncated ? "true" : "false");
	sb_free(&output);
	return 0;
}

/* ==========================================================================
 * capture_and_decode (extra)
 * ========================================================================== */

static int tool_capture_and_decode(const json_value *args, strbuf *result, char *err,
				   size_t errlen)
{
	char *output_file = NULL;
	strbuf log, output;
	int truncated;

	sb_init(&log);
	sb_init(&output);
	if (do_capture(args, &output_file, &log, err, errlen) != 0) {
		sb_free(&log);
		sb_free(&output);
		free(output_file);
		return -1;
	}
	if (do_decode(args, output_file, &output, err, errlen) != 0) {
		char reason[400];

		/* The capture is on disk, so say so: the agent can still decode it
		 * later instead of repeating the acquisition. */
		snprintf(reason, sizeof(reason), "%s", err);
		snprintf(err, errlen, "captured to %s but decoding failed: %s",
			 output_file, reason);
		sb_free(&log);
		sb_free(&output);
		free(output_file);
		return -1;
	}

	truncated = output.len > MAX_TEXT_OUTPUT;
	if (truncated)
		output.data[MAX_TEXT_OUTPUT] = '\0';
	sb_put(result, "{\"input_file\":");
	sb_json_string(result, output_file);
	sb_printf(result, ",\"bytes\":%lld", file_size(output_file));
	sb_put(result, ",\"output\":");
	sb_json_string(result, output.data);
	sb_printf(result, ",\"truncated\":%s}", truncated ? "true" : "false");
	sb_free(&log);
	sb_free(&output);
	free(output_file);
	return 0;
}

/* ==========================================================================
 * set_device_options (extra)
 * ========================================================================== */

static int tool_set_device_options(const json_value *args, strbuf *result, char *err,
				   size_t errlen)
{
	const char *driver = json_get_string(args, "driver", NULL);
	const char *conn = json_get_string(args, "conn", NULL);
	const char *config = json_get_string(args, "config", NULL);
	const char *argv[8];
	char *spec;
	char *config_expanded;
	proc_result run;

	if (!driver || !*driver) {
		snprintf(err, errlen, "driver is required");
		return -1;
	}
	if (!config || !*config) {
		snprintf(err, errlen, "config is required, e.g. 'logic_channels=8'");
		return -1;
	}

	spec = driver_spec(driver, conn);
	config_expanded = prepare_config(config);
	argv[0] = "-d";
	argv[1] = spec;
	argv[2] = "--config";
	argv[3] = config_expanded;
	argv[4] = "--set";
	argv[5] = NULL;

	int rc = run_cli(argv, &run, err, errlen);

	free(spec);
	if (rc != 0) {
		free(config_expanded);
		return -1;
	}
	if (run.exit_code != 0) {
		cli_failure(&run, err, errlen);
		proc_result_free(&run);
		free(config_expanded);
		return -1;
	}
	sb_put(result, "{\"driver\":");
	sb_json_string(result, driver);
	sb_put(result, ",\"config\":");
	sb_json_string(result, config_expanded);
	sb_put(result, ",\"applied\":true,\"log\":");
	sb_json_string(result, first_of(&run));
	sb_putc(result, '}');
	free(config_expanded);
	proc_result_free(&run);
	return 0;
}

/* ==========================================================================
 * check_firmware_status
 * ========================================================================== */

static long count_firmware_files(const char *dir)
{
	long count = 0;

#ifdef _WIN32
	WIN32_FIND_DATAA entry;
	char *pattern = path_join(dir, "*.fw");
	HANDLE find = FindFirstFileA(pattern, &entry);

	free(pattern);
	if (find == INVALID_HANDLE_VALUE)
		return 0;
	do {
		if (!(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			count++;
	} while (FindNextFileA(find, &entry));
	FindClose(find);
#else
	DIR *d = opendir(dir);
	struct dirent *entry;

	if (!d)
		return 0;
	while ((entry = readdir(d))) {
		if (entry->d_name[0] == '.')
			continue;
		size_t len = strlen(entry->d_name);

		if (len > 3 && !strcasecmp(entry->d_name + len - 3, ".fw"))
			count++;
	}
	closedir(d);
#endif
	return count;
}

static int is_dir(const char *path)
{
#ifdef _WIN32
	DWORD attributes = GetFileAttributesA(path);

	return attributes != INVALID_FILE_ATTRIBUTES &&
	       (attributes & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat st;

	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

static long append_firmware_dir(strbuf *list, size_t *count, const char *path)
{
	long files;

	if (!path || !*path)
		return 0;
	files = is_dir(path) ? count_firmware_files(path) : 0;
	if (*count)
		sb_putc(list, ',');
	sb_put(list, "{\"path\":");
	sb_json_string(list, path);
	sb_put(list, ",\"exists\":");
	sb_put(list, is_dir(path) ? "true" : "false");
	sb_printf(list, ",\"files\":%ld}", files);
	(*count)++;
	return files;
}

static int tool_check_firmware(const json_value *args, strbuf *result, char *err,
			       size_t errlen)
{
	strbuf list;
	size_t count = 0;
	long total = 0;
	const char *env = getenv("SIGROK_FIRMWARE_DIR");
	char *exe_dir;

	(void)args;
	(void)err;
	(void)errlen;
	sb_init(&list);

	/* Where this package keeps them, then the environment, then the
	 * system wide locations libsigrok searches on its own. */
	exe_dir = path_exe_dir();
	if (exe_dir) {
		static const char *const candidates[] = {
			"share/sigrok-firmware",
			"sigrok-firmware",
			"../Resources/sigrok-firmware",
			"../share/sigrok-firmware",
		};

		for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
			char *path = path_join(exe_dir, candidates[i]);

			if (is_dir(path))
				total += append_firmware_dir(&list, &count, path);
			free(path);
		}
		free(exe_dir);
	}
	if (env && *env)
		total += append_firmware_dir(&list, &count, env);
	total += append_firmware_dir(&list, &count, "/usr/local/share/sigrok-firmware");
	total += append_firmware_dir(&list, &count, "/usr/share/sigrok-firmware");

	sb_put(result, "{\"directories\":[");
	sb_put(result, list.data);
	sb_printf(result, "],\"total_files\":%ld", total);
	sb_put(result, ",\"hint\":\"Only FX2 based analyzers need these files; the "
		       "CH32H417 runs its firmware from flash. Empty directories are "
		       "normal for this board.\"");
	sb_putc(result, '}');
	sb_free(&list);
	return 0;
}

/* ==========================================================================
 * The table
 * ========================================================================== */

static const mcp_tool tool_table[] = {
	{
		"list_supported_hardware",
		"List every hardware driver this build of libsigrok knows.",
		"{\"type\":\"object\",\"properties\":{},\"additionalProperties\":false}",
		tool_list_hardware
	},
	{
		"list_supported_decoders",
		"List the protocol decoders. Optional filter matches the id or the "
		"description, case insensitive.",
		"{\"type\":\"object\",\"properties\":{\"filter\":{\"type\":\"string\","
		"\"description\":\"Substring to match against decoder id or description\"}},"
		"\"additionalProperties\":false}",
		tool_list_decoders
	},
	{
		"list_input_formats",
		"List the file formats that can be read.",
		"{\"type\":\"object\",\"properties\":{},\"additionalProperties\":false}",
		tool_list_input_formats
	},
	{
		"list_output_formats",
		"List the file formats that can be written.",
		"{\"type\":\"object\",\"properties\":{},\"additionalProperties\":false}",
		tool_list_output_formats
	},
	{
		"show_version",
		"Versions of u3logicanalyzer-cli, libsigrok and libsigrokdecode.",
		"{\"type\":\"object\",\"properties\":{},\"additionalProperties\":false}",
		tool_show_version
	},
	{
		"scan_devices",
		"Scan for connected devices. Driver and conn restrict the scan to one "
		"serial or network instrument.",
		"{\"type\":\"object\",\"properties\":{\"driver\":{\"type\":\"string\","
		"\"description\":\"Driver id, e.g. wch-ch32h417-libusb or demo\"},"
		"\"conn\":{\"type\":\"string\",\"description\":\"Connection string, e.g. "
		"/dev/ttyUSB0:serialcomm=115200/8n1\"}},\"additionalProperties\":false}",
		tool_scan_devices
	},
	{
		"show_driver_details",
		"Driver capabilities and the options of the device it found: functions, "
		"scan options, channel groups, samplerates, triggers.",
		"{\"type\":\"object\",\"required\":[\"driver\"],\"properties\":{"
		"\"driver\":{\"type\":\"string\",\"description\":\"Driver id, e.g. demo\"},"
		"\"conn\":{\"type\":\"string\",\"description\":\"Connection string when the "
		"driver needs one\"}},\"additionalProperties\":false}",
		tool_show_driver_details
	},
	{
		"show_decoder_details",
		"Documentation, options, channels and annotation classes of one protocol "
		"decoder, read from its sources.",
		"{\"type\":\"object\",\"required\":[\"decoder\"],\"properties\":{"
		"\"decoder\":{\"type\":\"string\",\"description\":\"Decoder id, e.g. uart\"}},"
		"\"additionalProperties\":false}",
		tool_show_decoder_details
	},
	{
		"capture_data",
		"Acquire samples and save them to a file. Either samples or time is "
		"required. Returns the path for decode_protocol.",
		"{\"type\":\"object\",\"required\":[\"driver\"],\"properties\":{"
		"\"driver\":{\"type\":\"string\",\"description\":\"Driver id, e.g. "
		"wch-ch32h417-libusb or demo\"},"
		"\"conn\":{\"type\":\"string\"},"
		"\"config\":{\"type\":\"string\",\"description\":\"Device options joined "
		"with ':', e.g. samplerate=4m:voltage_threshold=1.2\"},"
		"\"channels\":{\"type\":\"string\",\"description\":\"Channels, e.g. D0,D1\"},"
		"\"samples\":{\"type\":\"number\",\"description\":\"Number of samples\"},"
		"\"time\":{\"type\":\"number\",\"description\":\"Duration in milliseconds\"},"
		"\"output_file\":{\"type\":\"string\",\"description\":\"Target file, "
		"auto-generated when omitted\"},"
		"\"triggers\":{\"type\":\"string\",\"description\":\"Trigger, e.g. D0=r\"},"
		"\"wait_trigger\":{\"type\":\"boolean\"}},\"additionalProperties\":false}",
		tool_capture_data
	},
	{
		"decode_protocol",
		"Run protocol decoders over a captured file and return their annotations.",
		"{\"type\":\"object\",\"required\":[\"input_file\",\"protocol_decoders\"],"
		"\"properties\":{\"input_file\":{\"type\":\"string\"},"
		"\"protocol_decoders\":{\"type\":\"string\",\"description\":\"e.g. "
		"uart:rx=D0:baudrate=115200\"},"
		"\"input_format\":{\"type\":\"string\",\"description\":\"e.g. srzip, vcd\"},"
		"\"annotations\":{\"type\":\"string\",\"description\":\"e.g. uart=rx-data\"},"
		"\"meta_decoders\":{\"type\":\"string\",\"description\":\"Decoder ids whose meta output to show, comma separated, e.g. uart\"},"
		"\"meta_output\":{\"type\":\"string\",\"description\":\"Deprecated alias of meta_decoders\"},"
		"\"show_sample_numbers\":{\"type\":\"boolean\"},"
		"\"json_trace\":{\"type\":\"boolean\"}},\"additionalProperties\":false}",
		tool_decode_protocol
	},
	{
		"check_firmware_status",
		"Where firmware files were searched, including the directory shipped with "
		"this package.",
		"{\"type\":\"object\",\"properties\":{},\"additionalProperties\":false}",
		tool_check_firmware
	},
	/* ---- additions of this server ---- */
	{
		"capture_and_decode",
		"Capture and decode in one call: takes every capture_data argument plus "
		"protocol_decoders, returns the file and the decoded annotation.",
		"{\"type\":\"object\",\"required\":[\"driver\",\"protocol_decoders\"],"
		"\"properties\":{\"driver\":{\"type\":\"string\"},"
		"\"conn\":{\"type\":\"string\"},"
		"\"config\":{\"type\":\"string\",\"description\":\"Device options joined "
		"with ':', e.g. samplerate=4m:voltage_threshold=1.2\"},"
		"\"channels\":{\"type\":\"string\"},"
		"\"samples\":{\"type\":\"number\"},"
		"\"time\":{\"type\":\"number\"},"
		"\"output_file\":{\"type\":\"string\"},"
		"\"triggers\":{\"type\":\"string\"},"
		"\"wait_trigger\":{\"type\":\"boolean\"},"
		"\"protocol_decoders\":{\"type\":\"string\",\"description\":\"e.g. "
		"uart:rx=D0:baudrate=115200\"},"
		"\"annotations\":{\"type\":\"string\"},"
		"\"meta_decoders\":{\"type\":\"string\",\"description\":\"Decoder ids whose meta output to show, comma separated, e.g. uart\"},"
		"\"meta_output\":{\"type\":\"string\",\"description\":\"Deprecated alias of meta_decoders\"},"
		"\"show_sample_numbers\":{\"type\":\"boolean\"},"
		"\"json_trace\":{\"type\":\"boolean\"}},\"additionalProperties\":false}",
		tool_capture_and_decode
	},
	{
		"set_device_options",
		"Apply device options without acquiring anything, to verify them or to "
		"leave the board in a known state.",
		"{\"type\":\"object\",\"required\":[\"driver\",\"config\"],\"properties\":{"
		"\"driver\":{\"type\":\"string\"},\"conn\":{\"type\":\"string\"},"
		"\"config\":{\"type\":\"string\",\"description\":\"Device options joined "
		"with ':', e.g. voltage_threshold=1.2\"}},"
		"\"additionalProperties\":false}",
		tool_set_device_options
	},
	{NULL, NULL, NULL, NULL}
};

const mcp_tool *tools_table(void)
{
	return tool_table;
}

const mcp_tool *tool_find(const char *name)
{
	for (const mcp_tool *tool = tool_table; tool->name; tool++)
		if (!strcmp(tool->name, name))
			return tool;
	return NULL;
}
