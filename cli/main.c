/*
 * This file is part of u3logicanalyzer-cli, a fork of the sigrok-cli project.
 *
 * Copyright (C) 2013 Bert Vermeulen <bert@biot.com>
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

#include <config.h>
#include <stdlib.h>
#include <glib.h>
#include "u3logicanalyzer-cli.h"

struct sr_context *sr_ctx = NULL;

/*
 * Bit i set means channel i was asked for with -C. Zero means the option was
 * not given at all, which leaves every channel of the device in play.
 */
uint64_t selected_channel_mask = 0;

/*
 * How many logic channels the device has to be put into before the
 * capture, 8 or 16. Zero means that nothing was asked for and that the
 * device is left on whatever it came up with.
 */
int logic_channel_count_needed = 0;
#ifdef HAVE_SRD
struct srd_session *srd_sess = NULL;
#endif

static void logger(const gchar *log_domain, GLogLevelFlags log_level,
		   const gchar *message, gpointer cb_data)
{
	(void)log_domain;
	(void)cb_data;

	/*
	 * All messages, warnings, errors etc. go to stderr (not stdout) in
	 * order to not mess up the CLI tool data output, e.g. VCD output.
	 */
	if (log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)
			|| opt_loglevel > SR_LOG_WARN) {
		fprintf(stderr, "%s\n", message);
		fflush(stderr);
	}

	if (log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL))
		exit(1);

}

int select_channels(struct sr_dev_inst *sdi)
{
	struct sr_channel *ch;
	gboolean enabled;
	GSList *selected_channels, *l, *channels;

	channels = sr_dev_inst_channels_get(sdi);

	if (opt_channels) {
		if (!(selected_channels = parse_channelstring(sdi, opt_channels)))
			return SR_ERR;

		selected_channel_mask = 0;
		for (l = channels; l; l = l->next) {
			ch = l->data;
			enabled = (g_slist_find(selected_channels, ch) != NULL);
			if (sr_dev_channel_enable(ch, enabled) != SR_OK)
				return SR_ERR;
			if (enabled && ch->index >= 0 && ch->index < 64)
				selected_channel_mask |= (uint64_t)1 << ch->index;
		}
		g_slist_free(selected_channels);

		/*
		 * The device ships either 8 or 16 channels, and the wider
		 * of the two costs two bytes per sample instead of one. Take
		 * the narrow one whenever every channel that was asked for
		 * fits into it: that halves what travels over USB and what
		 * the capture costs in memory, and the channels that were
		 * asked for keep their numbers.
		 */
		if (selected_channel_mask && !(selected_channel_mask & ~0xffULL))
			logic_channel_count_needed = 8;
		else
			logic_channel_count_needed = 16;
	}
#ifdef HAVE_SRD
	map_pd_channels(sdi);
#endif
	return SR_OK;
}

/*
 * Put the sample rate back to what it was before the width was switched.
 *
 * Switching between 8 and 16 channels swaps the table of rates the device
 * offers, and the driver moves to the top of the new one: on a USB 2.0
 * link 16 channels start at 20 MHz and 8 channels at 40 MHz. Left alone
 * that doubles the rate, and --time then asks for twice as many samples
 * in the same number of bytes, which is the opposite of what the narrow
 * width is for. A rate the user asked for is applied afterwards by
 * set_dev_options_array(), so it still wins over this.
 */
static void restore_samplerate(struct sr_dev_inst *sdi, uint64_t samplerate)
{
	struct sr_dev_driver *driver;
	GVariant *gvar;
	uint64_t now;
	int ret;

	driver = sr_dev_inst_driver_get(sdi);

	if (maybe_config_get(driver, sdi, NULL, SR_CONF_SAMPLERATE, &gvar) != SR_OK)
		return;
	now = g_variant_get_uint64(gvar);
	g_variant_unref(gvar);

	if (now == samplerate)
		return;

	ret = maybe_config_set(driver, sdi, NULL, SR_CONF_SAMPLERATE,
			g_variant_new_uint64(samplerate));
	if (ret == SR_OK) {
		g_debug("Kept the sample rate at %" PRIu64 " Hz.", samplerate);
		return;
	}

	/*
	 * The new width does not offer that rate any more, 40 MHz going
	 * back to 16 channels being the case that shows it. The driver has
	 * already moved to the top of the new table, which is the fastest
	 * rate that is left, so there is nothing better to pick here.
	 */
	g_debug("Could not keep the sample rate at %" PRIu64 " Hz: %s.",
		samplerate, sr_strerror(ret));
}

int apply_logic_channel_count(struct sr_dev_inst *sdi)
{
	struct sr_dev_driver *driver;
	GVariant *gvar;
	uint64_t samplerate;
	int ret;

	if (!logic_channel_count_needed)
		return SR_OK;

	driver = sr_dev_inst_driver_get(sdi);
	samplerate = 0;
	if (maybe_config_get(driver, sdi, NULL, SR_CONF_SAMPLERATE, &gvar) == SR_OK) {
		samplerate = g_variant_get_uint64(gvar);
		g_variant_unref(gvar);
	}

	ret = maybe_config_set(driver, sdi, NULL,
			SR_CONF_NUM_LOGIC_CHANNELS,
			g_variant_new_int32(logic_channel_count_needed));

	/* Not every device can be told how wide to go. */
	if (ret == SR_ERR_NA)
		return SR_OK;

	if (ret != SR_OK) {
		/*
		 * The capture is still good at the width the device is on,
		 * only the saving is lost, so carry on and say so.
		 */
		g_warning("Could not set the channel count to %d: %s.",
			logic_channel_count_needed, sr_strerror(ret));
		return SR_OK;
	}

	g_debug("Channel count set to %d.", logic_channel_count_needed);

	if (samplerate)
		restore_samplerate(sdi, samplerate);

	return SR_OK;
}

int maybe_config_get(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant **gvar)
{
	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_GET)
		return sr_config_get(driver, sdi, cg, key, gvar);

	return SR_ERR_NA;
}

int maybe_config_set(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant *gvar)
{
	(void)driver;

	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_SET)
		return sr_config_set(sdi, cg, key, gvar);

	return SR_ERR_NA;
}

int maybe_config_list(struct sr_dev_driver *driver,
		const struct sr_dev_inst *sdi, struct sr_channel_group *cg,
		uint32_t key, GVariant **gvar)
{
	if (sr_dev_config_capabilities_list(sdi, cg, key) & SR_CONF_LIST)
		return sr_config_list(driver, sdi, cg, key, gvar);

	return SR_ERR_NA;
}

static void get_option(struct sr_dev_inst *sdi,
	struct sr_channel_group *cg, const char *opt)
{
	struct sr_dev_driver *driver;
	const struct sr_key_info *ci;
	int ret;
	GVariant *gvar;
	const struct sr_key_info *srci, *srmqi, *srmqfi;
	uint32_t mq;
	uint64_t mask, mqflags;
	unsigned int j;
	char *s;

	driver = sr_dev_inst_driver_get(sdi);

	ci = sr_key_info_name_get(SR_KEY_CONFIG, opt);
	if (!ci)
		g_critical("Unknown option '%s'", opt);

	ret = maybe_config_get(driver, sdi, cg, ci->key, &gvar);
	if (ret != SR_OK)
		g_critical("Failed to get '%s': %s", opt, sr_strerror(ret));

	srci = sr_key_info_get(SR_KEY_CONFIG, ci->key);
	if (srci && srci->datatype == SR_T_MQ) {
		g_variant_get(gvar, "(ut)", &mq, &mqflags);
		if ((srmqi = sr_key_info_get(SR_KEY_MQ, mq)))
			printf("%s", srmqi->id);
		else
			printf("%d", mq);
		for (j = 0, mask = 1; j < 32; j++, mask <<= 1) {
			if (!(mqflags & mask))
				continue;
			if ((srmqfi = sr_key_info_get(SR_KEY_MQFLAGS, mqflags & mask)))
				printf("/%s", srmqfi->id);
			else
				printf("/%" PRIu64, mqflags & mask);
		}
		printf("\n");
	} else {
		s = g_variant_print(gvar, FALSE);
		printf("%s\n", s);
		g_free(s);
	}

	g_variant_unref(gvar);
}

static void get_options(void)
{
	GSList *devices;
	struct sr_dev_inst *sdi;
	size_t get_idx;
	char *get_text, *cg_name;
	GHashTable *args;
	GHashTableIter iter;
	gpointer key, value;
	struct sr_channel_group *cg;

	/* Lookup and open the device. */
	devices = device_scan();
	if (!devices) {
		g_critical("No devices found.");
		return;
	}
	sdi = devices->data;
	g_slist_free(devices);

	if (sr_dev_open(sdi) != SR_OK) {
		g_critical("Failed to open device.");
		return;
	}

	/* Set configuration values when -c was specified. */
	set_dev_options_array(sdi, opt_configs);

	/* Get configuration values which were specified by --get. */
	for (get_idx = 0; (get_text = opt_gets[get_idx]); get_idx++) {
		args = parse_generic_arg(get_text, FALSE, "channel_group");
		if (!args)
			continue;
		cg_name = g_hash_table_lookup(args, "sigrok_key");
		cg = lookup_channel_group(sdi, cg_name);
		g_hash_table_iter_init(&iter, args);
		while (g_hash_table_iter_next(&iter, &key, &value)) {
			if (g_ascii_strcasecmp(key, "sigrok_key") == 0)
				continue;
			get_option(sdi, cg, key);
		}
	}

	/* Close the device. */
	sr_dev_close(sdi);
}

static void set_options(void)
{
	struct sr_dev_inst *sdi;
	GSList *devices;

	if (!opt_configs) {
		g_critical("No setting specified.");
		return;
	}

	if (!(devices = device_scan())) {
		g_critical("No devices found.");
		return;
	}
	sdi = devices->data;
	g_slist_free(devices);

	if (sr_dev_open(sdi) != SR_OK) {
		g_critical("Failed to open device.");
		return;
	}

	set_dev_options_array(sdi, opt_configs);

	sr_dev_close(sdi);
}

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>

/* Own directory, the equivalent of %~dp0 in a batch file. */
static int get_exe_dir(wchar_t *out, int bufsize)
{
    DWORD ret = GetModuleFileNameW(NULL, out, bufsize);
    if (ret == 0 || ret == (DWORD)bufsize)
        return -1;

    wchar_t *p = wcsrchr(out, L'\\');
    if (p)
        *(p+1) = L'\0';
    return 0;
}
#else
#include <limits.h>
#include <string.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

/*
 * Everything this tool needs (decoders, firmware) sits next to the binary,
 * and an MCP client starts it without a launcher: there is no AppRun or
 * shell wrapper in the path, so the variables have to be set here.
 */
static void set_env_if_missing(const char *name, const char *value)
{
	if (value && *value && !g_getenv(name))
		g_setenv(name, value, TRUE);
}

static int get_exe_dir(char *out, size_t bufsize)
{
#ifdef __APPLE__
	uint32_t size = (uint32_t)bufsize;
	char resolved[PATH_MAX];

	if (_NSGetExecutablePath(out, &size) != 0)
		return -1;
	if (!realpath(out, resolved))
		return -1;
	snprintf(out, bufsize, "%s", resolved);
#else
	ssize_t len = readlink("/proc/self/exe", out, bufsize - 1);

	if (len <= 0)
		return -1;
	out[len] = '\0';
#endif

	char *slash = strrchr(out, '/');
	if (!slash)
		return -1;
	*slash = '\0';
	return 0;
}

/*
 * First of the candidates that exists below exe_dir, NULL when there is none.
 */
static char *find_dir(const char *exe_dir, const char *const *candidates, size_t n)
{
	char *path;
	size_t i;

	for (i = 0; i < n; i++) {
		path = g_build_filename(exe_dir, candidates[i], NULL);
		if (g_file_test(path, G_FILE_TEST_IS_DIR))
			return path;
		g_free(path);
	}
	return NULL;
}

/*
 * Search order covers the three layouts the package ships in: next to the
 * binary (tar.gz, deb: /opt/U3LogicAnalyzer), one level up (AppImage:
 * usr/bin against usr/share) and the app bundle (Contents/MacOS against
 * Contents/Resources). Windows has its own layout and list below.
 */
static void setup_posix_paths(const char *exe_dir)
{
	static const char *const decoder_dirs[] = {
		"share/libsigrokdecode/decoders",
		"decoders",
		"../share/libsigrokdecode/decoders",
		"../Resources/decoders",
	};
	static const char *const firmware_dirs[] = {
		"share/sigrok-firmware",
		"sigrok-firmware",
		"../share/sigrok-firmware",
		"../Resources/sigrok-firmware",
	};
	char *dir;

	if (!g_getenv("SIGROKDECODE_DIR")) {
		dir = find_dir(exe_dir, decoder_dirs, G_N_ELEMENTS(decoder_dirs));
		if (dir) {
			g_setenv("SIGROKDECODE_DIR", dir, TRUE);
			g_free(dir);
		}
	}

	if (!g_getenv("SIGROK_FIRMWARE_DIR")) {
		dir = find_dir(exe_dir, firmware_dirs, G_N_ELEMENTS(firmware_dirs));
		if (dir) {
			g_setenv("SIGROK_FIRMWARE_DIR", dir, TRUE);
			g_free(dir);
		}
	}

	/*
	 * Decoders live inside the bundle on macOS, where a __pycache__ written
	 * at runtime breaks the code signature of the whole app.
	 */
	set_env_if_missing("PYTHONDONTWRITEBYTECODE", "1");
}
#endif

int main(int argc, char **argv)
{
#ifdef _WIN32
	wchar_t exe_dir[1024];
    if(get_exe_dir(exe_dir, _countof(exe_dir)) != 0)
    {
        fprintf(stderr, "Get exe path failed\n");
        return 1;
    }

    wchar_t buf[1024];

    // 1. PATH = %~dp0;%PATH%
    swprintf(buf, _countof(buf), L"%ls", exe_dir);
    SetEnvironmentVariableW(L"PATH", buf);

    // 2. PYTHONHOME=%~dp0python-portable
    swprintf(buf, _countof(buf), L"%lspython-portable", exe_dir);
    SetEnvironmentVariableW(L"PYTHONHOME", buf);

    // 3. PYTHONPATH=%~dp0python-portable\lib\python3.14
    swprintf(buf, _countof(buf), L"%lspython-portable\\lib\\python3.14", exe_dir);
    SetEnvironmentVariableW(L"PYTHONPATH", buf);

    // 4. SIGROKDECODE_DIR=%~dp0decoders
    swprintf(buf, _countof(buf), L"%lsdecoders", exe_dir);
    SetEnvironmentVariableW(L"SIGROKDECODE_DIR", buf);

    // 5. PYTHONDONTWRITEBYTECODE=1
    SetEnvironmentVariableW(L"PYTHONDONTWRITEBYTECODE", L"1");
#else
	char exe_dir[PATH_MAX];

	if (get_exe_dir(exe_dir, sizeof(exe_dir)) != 0) {
		fprintf(stderr, "Get exe path failed\n");
		return 1;
	}
	setup_posix_paths(exe_dir);
#endif

	g_log_set_default_handler(logger, NULL);

	if (parse_options(argc, argv)) {
		return 1;
	}

	/*
	 * libsigrok only warns about a level out of range and carries on, so
	 * the value is checked here instead: the option takes 0 to 5.
	 */
	if (opt_loglevel < SR_LOG_NONE || opt_loglevel > SR_LOG_SPEW) {
		g_critical("Invalid loglevel %d, expected 0 to 5.", opt_loglevel);
		return 1;
	}

	/*
	 * A frame has no length of its own; it ends when the sample or the
	 * time limit is reached. Asking for frames without either of those
	 * leaves the first frame open forever, which from the command line
	 * looks like a capture that never finishes.
	 */
	if (opt_frames && !opt_samples && !opt_time && !opt_continuous) {
		g_critical("Option --frames needs --samples or --time "
				"to say how long a frame is.");
		return 1;
	}

	/* Set the loglevel (amount of messages to output) for libsigrok. */
	if (sr_log_loglevel_set(opt_loglevel) != SR_OK)
		goto done;

	if (sr_init(&sr_ctx) != SR_OK)
		goto done;

#ifdef HAVE_SRD
	if (opt_pd_binary && !opt_pds) {
		g_critical("Option -B will not take effect in the absence of -P.");
		goto done;
	}

	/* Set the loglevel (amount of messages to output) for libsigrokdecode. */
	if (srd_log_loglevel_set(opt_loglevel) != SRD_OK)
		goto done;

	if (opt_pds) {
		if (srd_init(NULL) != SRD_OK)
			goto done;
		if (srd_session_new(&srd_sess) != SRD_OK) {
			g_critical("Failed to create new decode session.");
			goto done;
		}
		if (register_pds(opt_pds, opt_pd_annotations) != 0)
			goto done;

		/* Only one output type is ever shown. */
		if (opt_pd_binary) {
			if (setup_pd_binary(opt_pd_binary) != 0)
				goto done;
			if (setup_binary_stdout() != 0)
				goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_BINARY,
					show_pd_binary, NULL) != SRD_OK)
				goto done;
		} else if (opt_pd_meta) {
			if (setup_pd_meta(opt_pd_meta) != 0)
				goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_META,
					show_pd_meta, NULL) != SRD_OK)
				goto done;
		} else {
			if (opt_pd_annotations)
				if (setup_pd_annotations(opt_pd_annotations) != 0)
					goto done;
			if (srd_pd_output_callback_add(srd_sess, SRD_OUTPUT_ANN,
					show_pd_annotations, NULL) != SRD_OK)
				goto done;
		}
		show_pd_prepare();
	}
#endif

	if (opt_version)
		show_version();
	else if (opt_list_supported)
		show_supported();
	else if (opt_list_supported_wiki)
		show_supported_wiki();
	else if (opt_input_file && opt_show)
		load_input_file(TRUE);
	else if (opt_input_format && opt_show)
		show_input();
	else if (opt_output_format && opt_show)
		show_output();
	else if (opt_transform_module && opt_show)
		show_transform();
	else if (opt_scan_devs)
		show_dev_list();
#ifdef HAVE_SRD
	else if (opt_pds && opt_show)
		show_pd_detail();
#endif
	else if (opt_show)
		show_dev_detail();
	else if (opt_input_file)
		load_input_file(FALSE);
	else if (opt_gets)
		get_options();
	else if (opt_set)
		set_options();
	else if (opt_samples || opt_time || opt_frames || opt_continuous)
		run_session();
	else if (opt_list_serial)
		show_serial_ports();
	else
		show_help();

#ifdef HAVE_SRD
	if (opt_pds)
		show_pd_close();
	if (opt_pds)
		srd_exit();
#endif

done:
	if (sr_ctx)
		sr_exit(sr_ctx);

	return 0;
}
