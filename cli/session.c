/*
 * This file is part of u3logicanalyzer-cli, a fork of the sigrok-cli project.
 *
 * Copyright (C) 2013 Bert Vermeulen <bert@biot.com>
 * Copyright (C) 2026 OpenChipLab
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
#include <stdbool.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdlib.h>
#include "u3logicanalyzer-cli.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_host.h>
#elif !defined(_WIN32)
#include <sys/sysinfo.h>
#endif

/*
 * How much of the memory that is left the buffer is allowed to take before
 * the capture is stopped, in percent. The data of a capture is held in memory
 * in full (see memory_buffer below), so it grows with the capture.
 */
#define MEMORY_BUFFER_SHARE_PERCENT 50

struct memory_buffer {
	uint8_t **chunks;       
	size_t *chunk_sizes;   
	size_t chunk_count;     
	size_t chunk_capacity;  
	size_t unit_size;       
	uint64_t total_samples; 
	uint64_t samplerate;   
	size_t total_bytes;     /* bytes held in the chunks above */
	size_t limit_bytes;     /* stop the capture once this is reached */
};

static uint64_t limit_samples = 0;
static uint64_t limit_frames = 0;

#ifdef HAVE_SRD
extern struct srd_session *srd_sess;
#endif

/*
 * How much physical memory is left, in bytes, or 0 if it cannot be found out.
 */
static uint64_t available_memory_bytes(void)
{
#ifdef _WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (!GlobalMemoryStatusEx(&statex))
		return 0;

	return (uint64_t)statex.ullAvailPhys;
#elif defined(__APPLE__)
	vm_size_t page_size = 0;
	if (host_page_size(mach_host_self(), &page_size) != KERN_SUCCESS)
		return 0;

	vm_statistics64_data_t vmstat;
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
	    (host_info64_t)&vmstat, &count) != KERN_SUCCESS)
		return 0;

	return (uint64_t)(vmstat.free_count + vmstat.inactive_count) *
		(uint64_t)page_size;
#else
	long avail_kb = -1;
	FILE *f = fopen("/proc/meminfo", "r");
	if (f) {
		char line[256];
		while (fgets(line, sizeof(line), f)) {
			if (sscanf(line, "MemAvailable: %ld kB", &avail_kb) == 1)
				break;
		}
		fclose(f);
	}
	if (avail_kb >= 0)
		return (uint64_t)avail_kb * 1024;

	struct sysinfo si;
	if (sysinfo(&si) == 0)
		return (uint64_t)si.freeram * si.mem_unit;

	return 0;
#endif
}

static void memory_buffer_add_chunk(struct memory_buffer *mbuf,
		const void *data, size_t length, size_t unit_size)
{
	if (!mbuf || !data || !length)
		return;

	if (mbuf->chunk_count >= mbuf->chunk_capacity) {
		mbuf->chunk_capacity = mbuf->chunk_capacity ? mbuf->chunk_capacity * 2 : 64;
		mbuf->chunks = g_realloc(mbuf->chunks, sizeof(uint8_t *) * mbuf->chunk_capacity);
		mbuf->chunk_sizes = g_realloc(mbuf->chunk_sizes, sizeof(size_t) * mbuf->chunk_capacity);
	}

	mbuf->chunks[mbuf->chunk_count] = g_memdup(data, length);
	mbuf->chunk_sizes[mbuf->chunk_count] = length;
	mbuf->chunk_count++;
	mbuf->total_bytes += length;
	if (unit_size > 0) {
		mbuf->unit_size = unit_size;
		mbuf->total_samples += length / unit_size;
	}
}

static void memory_buffer_free(struct memory_buffer *mbuf)
{
	size_t i;

	if (!mbuf)
		return;
	for (i = 0; i < mbuf->chunk_count; i++)
		g_free(mbuf->chunks[i]);
	g_free(mbuf->chunks);
	g_free(mbuf->chunk_sizes);
	g_free(mbuf);
}

static void save_from_memory(const struct sr_output *o,
		const struct sr_output *oa, FILE *outfile,
		struct memory_buffer *mbuf)
{
	struct sr_datafeed_packet packet;
	struct sr_datafeed_logic logic;
	struct sr_datafeed_meta meta;
	struct sr_config src;
	GVariant *gvar;
	GString *out = NULL;
	size_t i;

	if (!o || !mbuf || mbuf->chunk_count == 0)
		return;

	g_message("cli: Saving %zu chunks (%" PRIu64 " samples) to file.",
		mbuf->chunk_count, mbuf->total_samples);

	memset(&packet, 0, sizeof(packet));
	memset(&meta, 0, sizeof(meta));
	memset(&src, 0, sizeof(src));
	src.key = SR_CONF_SAMPLERATE;
	gvar = g_variant_new_uint64(mbuf->samplerate);
	src.data = gvar;
	meta.config = g_slist_append(NULL, &src);
	packet.type = SR_DF_META;
	packet.payload = &meta;
	if (sr_output_send(o, &packet, &out) == SR_OK) {
		if (out) { g_string_free(out, TRUE); out = NULL; }
	}
	g_slist_free(meta.config);

	memset(&packet, 0, sizeof(packet));
	packet.type = SR_DF_LOGIC;
	packet.payload = &logic;
	logic.unitsize = mbuf->unit_size;
	for (i = 0; i < mbuf->chunk_count; i++) {
		logic.data = mbuf->chunks[i];
		logic.length = mbuf->chunk_sizes[i];
		if (sr_output_send(o, &packet, &out) == SR_OK) {
			if (oa && !out)
				sr_output_send(oa, &packet, &out);
			if (outfile && out && out->len > 0) {
				fwrite(out->str, 1, out->len, outfile);
				fflush(outfile);
			}
			if (out) { g_string_free(out, TRUE); out = NULL; }
		}
	}

	memset(&packet, 0, sizeof(packet));
	packet.type = SR_DF_END;
	packet.payload = NULL;
	if (sr_output_send(o, &packet, &out) == SR_OK) {
		if (oa && !out)
			sr_output_send(oa, &packet, &out);
		if (outfile && out && out->len > 0) {
			fwrite(out->str, 1, out->len, outfile);
			fflush(outfile);
		}
		if (out) { g_string_free(out, TRUE); out = NULL; }
	}

	g_message("cli: Save complete.");
}

static int set_limit_time(const struct sr_dev_inst *sdi)
{
	GVariant *gvar;
	uint64_t time_msec;
	uint64_t samplerate;
	struct sr_dev_driver *driver;

	driver = sr_dev_inst_driver_get(sdi);

	/*
	 * strtoull() turns a negative number into a huge one, and the
	 * conversion below would wrap around a second time. Either way the
	 * limit ends up one no capture can reach, and the session never
	 * stops, so a leading minus is refused before anything else.
	 */
	if (opt_time[0] == '-') {
		g_critical("Invalid time '%s'", opt_time);
		return SR_ERR;
	}

	if (!(time_msec = sr_parse_timestring(opt_time))) {
		g_critical("Invalid time '%s'", opt_time);
		return SR_ERR;
	}

	if (sr_dev_config_capabilities_list(sdi, NULL, SR_CONF_LIMIT_MSEC)
			& SR_CONF_SET) {
		gvar = g_variant_new_uint64(time_msec);
		if (sr_config_set(sdi, NULL, SR_CONF_LIMIT_MSEC, gvar) != SR_OK) {
			g_critical("Failed to configure time limit.");
			return SR_ERR;
		}
	} else if (sr_dev_config_capabilities_list(sdi, NULL, SR_CONF_SAMPLERATE)
			& (SR_CONF_GET | SR_CONF_SET)) {
		/* Convert to samples based on the samplerate. */
		sr_config_get(driver, sdi, NULL, SR_CONF_SAMPLERATE, &gvar);
		samplerate = g_variant_get_uint64(gvar);
		g_variant_unref(gvar);
		if (!samplerate) {
			g_critical("Cannot turn '%s' into samples: "
					"the samplerate is unknown.", opt_time);
			return SR_ERR;
		}
		/* A time this long would wrap the multiplication. */
		if (time_msec > G_MAXUINT64 / samplerate) {
			g_critical("Time '%s' is too long for %" PRIu64 " Hz.",
					opt_time, samplerate);
			return SR_ERR;
		}
		limit_samples = (samplerate) * time_msec / (uint64_t)1000;
		if (limit_samples == 0) {
			g_critical("Not enough time at this samplerate.");
			return SR_ERR;
		}
		gvar = g_variant_new_uint64(limit_samples);
		if (sr_config_set(sdi, NULL, SR_CONF_LIMIT_SAMPLES, gvar) != SR_OK) {
			g_critical("Failed to configure time-based sample limit.");
			return SR_ERR;
		}
	} else {
		g_critical("This device does not support time limits.");
		return SR_ERR;
	}

	return SR_OK;
}

/*
 * The file name says what the user wants when -O is left out: writing a
 * session archive into a file called something.vcd is not what that name
 * asks for. Unknown extensions keep the session archive default.
 */
static const char *output_format_from_extension(const char *filename)
{
	static const struct {
		const char *extension;
		const char *module;
	} by_extension[] = {
		{"sr", "srzip"},
		{"srzip", "srzip"},
		{"vcd", "vcd"},
		{"csv", "csv"},
		{"bin", "binary"},
		{"ascii", "ascii"},
		{"txt", "ascii"},
		{"hex", "hex"},
		{"json", "wavedrom"},
		{"wavedrom", "wavedrom"},
		{"la8", "chronovu_la8"},
		{"ols", "ols"},
		{"wav", "wav"},
	};
	const char *dot;
	size_t i;

	if (!filename)
		return NULL;

	dot = strrchr(filename, '.');
	if (!dot || !dot[1])
		return NULL;

	for (i = 0; i < G_N_ELEMENTS(by_extension); i++) {
		if (!g_ascii_strcasecmp(dot + 1, by_extension[i].extension))
			return by_extension[i].module;
	}

	return NULL;
}

const struct sr_output *setup_output_format(const struct sr_dev_inst *sdi, FILE **outfile)
{
	const struct sr_output_module *omod;
	const struct sr_option **options;
	const struct sr_output *o;
	GHashTable *fmtargs, *fmtopts;
	char *fmtspec;

	if (!opt_output_format) {
		if (opt_output_file) {
			const char *by_name =
				output_format_from_extension(opt_output_file);

			opt_output_format = by_name ?
				(char *)by_name : DEFAULT_OUTPUT_FORMAT_FILE;
		} else {
			opt_output_format = DEFAULT_OUTPUT_FORMAT_NOFILE;
		}
	}

	fmtargs = parse_generic_arg(opt_output_format, TRUE, NULL);
	fmtspec = g_hash_table_lookup(fmtargs, "sigrok_key");
	if (!fmtspec)
		g_critical("Invalid output format.");
	if (!(omod = sr_output_find(fmtspec)))
		g_critical("Unknown output module '%s'.", fmtspec);
	g_hash_table_remove(fmtargs, "sigrok_key");
	if ((options = sr_output_options_get(omod))) {
		fmtopts = generic_arg_to_opt(options, fmtargs);
		(void)warn_unknown_keys(options, fmtargs, NULL);
		sr_output_options_free(options);
	} else {
		fmtopts = NULL;
	}
	o = sr_output_new(omod, fmtopts, sdi, opt_output_file);

	if (opt_output_file) {
		if (!sr_output_test_flag(omod, SR_OUTPUT_INTERNAL_IO_HANDLING)) {
			*outfile = g_fopen(opt_output_file, "wb");
			if (!*outfile) {
				g_critical("Cannot write to output file '%s'.",
					opt_output_file);
			}
		} else {
			*outfile = NULL;
		}
	} else {
		setup_binary_stdout();
		*outfile = stdout;
	}

	if (fmtopts)
		g_hash_table_destroy(fmtopts);
	g_hash_table_destroy(fmtargs);

	return o;
}

const struct sr_transform *setup_transform_module(const struct sr_dev_inst *sdi)
{
	const struct sr_transform_module *tmod;
	const struct sr_option **options;
	const struct sr_transform *t;
	GHashTable *fmtargs, *fmtopts;
	char *fmtspec;

	fmtargs = parse_generic_arg(opt_transform_module, TRUE, NULL);
	fmtspec = g_hash_table_lookup(fmtargs, "sigrok_key");
	if (!fmtspec)
		g_critical("Invalid transform module.");
	if (!(tmod = sr_transform_find(fmtspec)))
		g_critical("Unknown transform module '%s'.", fmtspec);
	g_hash_table_remove(fmtargs, "sigrok_key");
	if ((options = sr_transform_options_get(tmod))) {
		fmtopts = generic_arg_to_opt(options, fmtargs);
		(void)warn_unknown_keys(options, fmtargs, NULL);
		sr_transform_options_free(options);
	} else {
		fmtopts = NULL;
	}
	t = sr_transform_new(tmod, fmtopts, sdi);
	if (fmtopts)
		g_hash_table_destroy(fmtopts);
	g_hash_table_destroy(fmtargs);

	return t;
}

/* Get the input stream's list of channels and their types, once. */
static void props_get_channels(struct df_arg_desc *args,
	const struct sr_dev_inst *sdi)
{
	struct input_stream_props *props;
	GSList *l;
	const struct sr_channel *ch;

	if (!args)
		return;
	props = &args->props;
	if (props->channels)
		return;

	props->channels = g_slist_copy(sr_dev_inst_channels_get(sdi));
	if (!props->channels)
		return;
	for (l = props->channels; l; l = l->next) {
		ch = l->data;
		if (!ch->enabled)
			continue;
		if (ch->type != SR_CHANNEL_ANALOG)
			continue;
		props->first_analog_channel = ch;
		break;
	}
}

static gboolean props_chk_1st_channel(struct df_arg_desc *args,
	const struct sr_datafeed_analog *analog)
{
	struct sr_channel *ch;

	if (!args || !analog || !analog->meaning)
		return FALSE;
	ch = g_slist_nth_data(analog->meaning->channels, 0);
	if (!ch)
		return FALSE;
	return ch == args->props.first_analog_channel;
}

static void props_dump_details(struct df_arg_desc *args)
{
	struct input_stream_props *props;
	size_t ch_count;
	GSList *l;
	const struct sr_channel *ch;
	const char *type;

	if (!args)
		return;
	props = &args->props;
	if (props->samplerate)
		printf("Samplerate: %" PRIu64 "\n", props->samplerate);
	if (props->channels) {
		ch_count = g_slist_length(props->channels);
		printf("Channels: %zu\n", ch_count);
		for (l = props->channels; l; l = l->next) {
			ch = l->data;
			if (ch->type == SR_CHANNEL_ANALOG)
				type = "analog";
			else
				type = "logic";
			printf("- %s: %s\n", ch->name, type);
		}
	}
	if (props->unitsize)
		printf("Logic unitsize: %zu\n", props->unitsize);
	if (props->sample_count_logic)
		printf("Logic sample count: %" PRIu64 "\n", props->sample_count_logic);
	if (props->sample_count_analog)
		printf("Analog sample count: %" PRIu64 "\n", props->sample_count_analog);
	if (props->frame_count)
		printf("Frame count: %" PRIu64 "\n", props->frame_count);
	if (props->triggered)
		printf("Trigger count: %" PRIu64 "\n", props->triggered);
}

static void props_cleanup(struct df_arg_desc *args)
{
	struct input_stream_props *props;

	if (!args)
		return;
	props = &args->props;
	g_slist_free(props->channels);
	props->channels = NULL;
	props->first_analog_channel = NULL;
}

void datafeed_in(const struct sr_dev_inst *sdi,
		const struct sr_datafeed_packet *packet, void *cb_data)
{
	static const struct sr_output *o = NULL;
	static const struct sr_output *oa = NULL;
	static uint64_t rcvd_samples_logic = 0;
	static uint64_t rcvd_samples_analog = 0;
	static uint64_t samplerate = 0;
	static int triggered = 0;
	static FILE *outfile = NULL;
	static struct memory_buffer *mbuf = NULL;
	static gboolean stop_requested = FALSE;

	const struct sr_datafeed_meta *meta;
	const struct sr_datafeed_logic *logic;
	const struct sr_datafeed_analog *analog;
	struct df_arg_desc *df_arg;
	int do_props;
	struct input_stream_props *props;
	struct sr_session *session;
	struct sr_config *src;
	GSList *l;
	GVariant *gvar;
	uint64_t end_sample;
	uint64_t input_len;
	struct sr_dev_driver *driver;

	/* Avoid warnings when building without decoder support. */
	(void)input_len;

	driver = sr_dev_inst_driver_get(sdi);

	/* Skip all packets before the first header. */
	if (packet->type != SR_DF_HEADER && !o)
		return;

	/* Prepare to either process data, or "just" gather properties. */
	df_arg = cb_data;
	session = df_arg->session;
	do_props = df_arg->do_props;
	props = &df_arg->props;

	switch (packet->type) {
	case SR_DF_HEADER:
		g_debug("cli: Received SR_DF_HEADER.");
		stop_requested = FALSE;
		if (maybe_config_get(driver, sdi, NULL, SR_CONF_SAMPLERATE,
				&gvar) == SR_OK) {
			samplerate = g_variant_get_uint64(gvar);
			g_variant_unref(gvar);
		}
		if (do_props) {
			/* Setup variables for maximum code path re-use. */
			o = (void *)-1;
			limit_samples = 0;
			/* Start collecting input stream properties. */
			memset(props, 0, sizeof(*props));
			props->samplerate = samplerate;
			props_get_channels(df_arg, sdi);
			break;
		}
		if (!(o = setup_output_format(sdi, &outfile)))
			g_critical("Failed to initialize output module.");

		/* Set up backup analog output module. */
		if (outfile)
			oa = sr_output_new(sr_output_find("analog"), NULL,
					sdi, NULL);

		/*
		 * Collect the capture in memory and write it out in one go at the
		 * end. That is what the output modules need: srzip rewrites the
		 * whole archive on every append, so handing it the data as it
		 * arrives makes writing quadratic in the size of the capture and
		 * the capture falls behind.
		 *
		 * The buffer is capped at a share of the memory that is free (see
		 * the check on it in datafeed_in()); without a cap, a long capture
		 * at a high sample rate takes the machine down with it instead.
		 */
		if (!opt_pds && opt_output_file) {
			if (mbuf) {
				memory_buffer_free(mbuf);
				mbuf = NULL;
			}
			mbuf = g_malloc0(sizeof(struct memory_buffer));
			mbuf->samplerate = samplerate;
			mbuf->limit_bytes = (size_t)(available_memory_bytes() *
				MEMORY_BUFFER_SHARE_PERCENT / 100);
			g_debug("Memory-first capture mode: buffering up to %zu bytes in memory",
				mbuf->limit_bytes);
		}

		rcvd_samples_logic = rcvd_samples_analog = 0;

#ifdef HAVE_SRD
		if (opt_pds) {
			if (samplerate) {
				if (srd_session_metadata_set(srd_sess, SRD_CONF_SAMPLERATE,
						g_variant_new_uint64(samplerate)) != SRD_OK) {
					g_critical("Failed to configure decode session.");
					break;
				}
				pd_samplerate = samplerate;
			}
			if (srd_session_start(srd_sess) != SRD_OK) {
				g_critical("Failed to start decode session.");
				break;
			}
		}
#endif
		break;

	case SR_DF_META:
		g_debug("cli: Received SR_DF_META.");
		meta = packet->payload;
		for (l = meta->config; l; l = l->next) {
			src = l->data;
			switch (src->key) {
			case SR_CONF_SAMPLERATE:
				samplerate = g_variant_get_uint64(src->data);
				g_debug("cli: Got samplerate %"PRIu64" Hz.", samplerate);
				if (mbuf)
					mbuf->samplerate = samplerate;
				if (do_props) {
					props->samplerate = samplerate;
					break;
				}
#ifdef HAVE_SRD
				if (opt_pds) {
					if (srd_session_metadata_set(srd_sess, SRD_CONF_SAMPLERATE,
							g_variant_new_uint64(samplerate)) != SRD_OK) {
						g_critical("Failed to pass samplerate to decoder.");
					}
					pd_samplerate = samplerate;
				}
#endif
				break;
			case SR_CONF_SAMPLE_INTERVAL:
				samplerate = g_variant_get_uint64(src->data);
				g_debug("cli: Got sample interval %"PRIu64" ms.", samplerate);
				if (do_props) {
					props->samplerate = samplerate;
					break;
				}
				break;
			default:
				/* Unknown metadata is not an error. */
				break;
			}
		}
		break;

	case SR_DF_TRIGGER:
		g_debug("cli: Received SR_DF_TRIGGER.");
		if (do_props) {
			props->triggered++;
			break;
		}
		triggered = 1;
		break;

	case SR_DF_LOGIC:
		logic = packet->payload;
		g_message("cli: Received SR_DF_LOGIC (%"PRIu64" bytes, unitsize = %d, first bytes: 0x%02x 0x%02x).",
				logic->length, logic->unitsize,
				(uint8_t)(logic->length > 0 ? ((const uint8_t *)logic->data)[0] : 0),
				(uint8_t)(logic->length > 1 ? ((const uint8_t *)logic->data)[1] : 0));
		if (logic->length == 0)
			break;

		if (do_props) {
			props_get_channels(df_arg, sdi);
			props->unitsize = logic->unitsize;
			props->sample_count_logic += logic->length / logic->unitsize;
			break;
		}

		/* Don't store any samples until triggered. */
		if (opt_wait_trigger && !triggered)
			break;

		/* Check if we already have enough samples. */
		if (limit_samples && rcvd_samples_logic >= limit_samples) {
			g_message("cli: Already have enough samples (%"PRIu64" >= %"PRIu64"), stopping.",
				rcvd_samples_logic, limit_samples);
			stop_requested = TRUE;
			break;
		}

		end_sample = rcvd_samples_logic + logic->length / logic->unitsize;
		/* Cut off last packet according to the sample limit. */
		if (limit_samples && end_sample > limit_samples)
			end_sample = limit_samples;
		input_len = (end_sample - rcvd_samples_logic) * logic->unitsize;

		if (mbuf && input_len > 0) {
			memory_buffer_add_chunk(mbuf, logic->data, input_len,
				logic->unitsize);
			rcvd_samples_logic = end_sample;

			if (limit_samples && rcvd_samples_logic >= limit_samples) {
				g_message("cli: Sample limit reached (%" PRIu64 "), stopping session.",
					rcvd_samples_logic);
				stop_requested = TRUE;
			} else if (mbuf->limit_bytes &&
				   mbuf->total_bytes >= mbuf->limit_bytes) {
				/*
				 * The room set aside for the capture is used up. Stop
				 * here instead of letting the machine run out of
				 * memory: once it starts swapping, the capture is no
				 * longer reliable and the window stops responding.
				 * What was captured so far is still written out at
				 * the end.
				 */
				g_message("cli: Memory buffer full (%" G_GSIZE_FORMAT
					" bytes, limit %" G_GSIZE_FORMAT "), stopping session.",
					mbuf->total_bytes, mbuf->limit_bytes);
				stop_requested = TRUE;
			}
			break;
		}

		if (opt_pds) {
#ifdef HAVE_SRD
			/* Calculate constant channel mask and values for optimization.
			 * IMPORTANT: The mask must use decoder channel indices, not physical indices. */
			uint64_t constant_mask = 0, constant_vals = 0;
			uint64_t num_samples = (end_sample - rcvd_samples_logic);
			int unitsize = logic->unitsize;
			int num_channels = unitsize * 8; /* Each bit is a channel. */
			const uint8_t *data = logic->data;

			/* Get decoder channel mapping: array[dec_ch_idx] = physical_ch_idx */
			int pd_num_channels = 0;
			int *pd_channel_map = get_pd_channel_map(&pd_num_channels);

			if (num_samples > 0 && pd_channel_map && pd_num_channels > 0) {
				/* For each decoder channel, check if its physical channel is constant */
				uint8_t phys_first_bits[64];
				bool phys_is_constant[64];
				memset(phys_is_constant, true, sizeof(phys_is_constant));
				memset(phys_first_bits, 0, sizeof(phys_first_bits));

				/* Check each sample to determine constant channels (using physical indices) */
				for (uint64_t s = 0; s < num_samples; s++) {
					const uint8_t *sample = data + s * unitsize;
					for (int b = 0; b < unitsize; b++) {
						uint8_t byte_val = sample[b];
						for (int bit = 0; bit < 8; bit++) {
							int phys_ch_idx = b * 8 + bit;
							if (phys_ch_idx >= num_channels) break;
							uint8_t bit_val = (byte_val >> bit) & 1;
							if (s == 0) {
								phys_first_bits[phys_ch_idx] = bit_val;
							} else if (phys_is_constant[phys_ch_idx] && bit_val != phys_first_bits[phys_ch_idx]) {
								phys_is_constant[phys_ch_idx] = false;
							}
						}
					}
				}

				/* Build mask using decoder channel indices */
				for (int dec_ch_idx = 0; dec_ch_idx < pd_num_channels; dec_ch_idx++) {
					int phys_ch_idx = pd_channel_map[dec_ch_idx];
					if (phys_ch_idx < 0 || phys_ch_idx >= num_channels)
						continue;  /* Unused optional channel */
					if (phys_is_constant[phys_ch_idx]) {
						constant_mask |= (1ULL << dec_ch_idx);
						if (phys_first_bits[phys_ch_idx])
							constant_vals |= (1ULL << dec_ch_idx);
					}
				}

				g_free(pd_channel_map);
			} else if (num_samples > 0 && num_channels <= 64) {
				/* Fallback: no channel mapping available, use physical indices */
				uint8_t first_bits[64];
				bool is_constant[64];
				memset(is_constant, true, sizeof(is_constant));
				memset(first_bits, 0, sizeof(first_bits));

				for (uint64_t s = 0; s < num_samples; s++) {
					const uint8_t *sample = data + s * unitsize;
					for (int b = 0; b < unitsize; b++) {
						uint8_t byte_val = sample[b];
						for (int bit = 0; bit < 8; bit++) {
							int ch_idx = b * 8 + bit;
							if (ch_idx >= num_channels) break;
							uint8_t bit_val = (byte_val >> bit) & 1;
							if (s == 0) {
								first_bits[ch_idx] = bit_val;
							} else if (is_constant[ch_idx] && bit_val != first_bits[ch_idx]) {
								is_constant[ch_idx] = false;
							}
						}
					}
				}

				for (int ch = 0; ch < num_channels; ch++) {
					if (is_constant[ch]) {
						constant_mask |= (1ULL << ch);
						if (first_bits[ch])
							constant_vals |= (1ULL << ch);
					}
				}
			}

			if (srd_session_send(srd_sess, rcvd_samples_logic, end_sample,
					logic->data, input_len, logic->unitsize,
					constant_mask, constant_vals) != SRD_OK)
				stop_requested = TRUE;
#endif

				/* 检查是否这是最后一块数据，如果是则发送 EOF */
				if (limit_samples && end_sample >= limit_samples) {
					g_message("cli: Last chunk reached, sending EOF.");
					srd_session_send_eof(srd_sess);
				}
		}

		rcvd_samples_logic = end_sample;

		/* Stop acquisition when sample limit is reached. */
		if (limit_samples && rcvd_samples_logic >= limit_samples) {
			g_message("cli: Sample limit reached (%"PRIu64"), stopping session.",
				rcvd_samples_logic);
#ifdef HAVE_SRD
				/* 先发送 EOF 给解码器，让它从 wait() 中返回 */
				if (opt_pds && srd_sess) {
					g_message("cli: Sending EOF to decoder.");
					srd_session_send_eof(srd_sess);
				}
#endif
			stop_requested = TRUE;
		}
		break;

	case SR_DF_ANALOG:
		analog = packet->payload;
		g_message("cli: Received SR_DF_ANALOG (%" G_GUINT32_FORMAT " samples).", analog->num_samples);
		if (analog->num_samples == 0)
			break;

		if (do_props) {
			/* Only count the first analog channel. */
			props_get_channels(df_arg, sdi);
			if (!props_chk_1st_channel(df_arg, analog))
				break;
			props->sample_count_analog += analog->num_samples;
			break;
		}

		if (limit_samples && rcvd_samples_analog >= limit_samples)
			break;

		rcvd_samples_analog += analog->num_samples;
		break;

	case SR_DF_FRAME_BEGIN:
		g_debug("cli: Received SR_DF_FRAME_BEGIN.");
		break;

	case SR_DF_FRAME_END:
		g_debug("cli: Received SR_DF_FRAME_END.");
		if (do_props) {
			props->frame_count++;
			break;
		}
		break;

	default:
		break;
	}

	if (!do_props && o && !opt_pds && !mbuf) {
		GString *out = NULL;
		if (sr_output_send(o, packet, &out) == SR_OK) {
			if (oa && !out) {
				/*
				 * The user didn't specify an output module,
				 * but needs to see this analog data.
				 */
				sr_output_send(oa, packet, &out);
			}
			if (outfile && out && out->len > 0) {
				fwrite(out->str, 1, out->len, outfile);
				fflush(outfile);
			}
			if (out)
				g_string_free(out, TRUE);
		}
	}

	/*
	 * SR_DF_END needs to be handled after the output module's receive()
	 * is called, so it can properly clean up that module.
	 */
	if (packet->type == SR_DF_END) {
		g_message("cli: Received SR_DF_END.");
		if (mbuf && mbuf->chunk_count > 0) {
			g_message("cli: Writing %" PRIu64 " samples to file...",
				mbuf->total_samples);
			save_from_memory(o, oa, outfile, mbuf);
		}
		if (mbuf) {
			memory_buffer_free(mbuf);
			mbuf = NULL;
		}

#if defined HAVE_SRD_SESSION_SEND_EOF && HAVE_SRD_SESSION_SEND_EOF
		(void)srd_session_send_eof(srd_sess);
#endif

		if (do_props) {
			props_dump_details(df_arg);
			props_cleanup(df_arg);
			o = NULL;
		}

		if (o)
			sr_output_free(o);
		o = NULL;

		if (oa)
			sr_output_free(oa);
		oa = NULL;

		if (outfile && outfile != stdout)
			fclose(outfile);

		if (limit_samples) {
			if (rcvd_samples_logic > 0 && rcvd_samples_logic < limit_samples)
				g_warning("Device only sent %" PRIu64 " samples.",
					   rcvd_samples_logic);
			else if (rcvd_samples_analog > 0 && rcvd_samples_analog < limit_samples)
				g_warning("Device only sent %" PRIu64 " samples.",
					   rcvd_samples_analog);
		}
	}

	/*
	 * Stop only after this packet has been handed to the output module.
	 * sr_session_stop() runs the stop synchronously in this thread, and the
	 * drivers answer it with SR_DF_END right away, which frees the output
	 * module. Doing that in the middle of the packet would drop its samples.
	 */
	if (stop_requested) {
		stop_requested = FALSE;
		sr_session_stop(session);
	}

}

int opt_to_gvar(char *key, char *value, struct sr_config *src)
{
	const struct sr_key_info *srci, *srmqi;
	double tmp_double, dlow, dhigh;
	uint64_t tmp_u64, p, q, low, high, mqflags;
	uint32_t mq;
	GVariant *rational[2], *range[2], *gtup[2];
	GVariantBuilder *vbl;
	gboolean tmp_bool;
	gchar **keyval;
	int ret, i;

	if (!(srci = sr_key_info_name_get(SR_KEY_CONFIG, key))) {
		g_critical("Unknown device option '%s'.", (char *) key);
		return -1;
	}
	src->key = srci->key;

	if ((!value || strlen(value) == 0) &&
		(srci->datatype != SR_T_BOOL)) {
		g_critical("Option '%s' needs a value.", (char *)key);
		return -1;
	}

	ret = 0;
	switch (srci->datatype) {
	case SR_T_UINT64:
		ret = sr_parse_sizestring(value, &tmp_u64);
		if (ret != 0)
			break;
		src->data = g_variant_new_uint64(tmp_u64);
		break;
	case SR_T_INT32:
		ret = sr_parse_sizestring(value, &tmp_u64);
		if (ret != 0)
			break;
		src->data = g_variant_new_int32(tmp_u64);
		break;
	case SR_T_STRING:
		src->data = g_variant_new_string(value);
		break;
	case SR_T_BOOL:
		if (!value)
			tmp_bool = TRUE;
		else
			tmp_bool = sr_parse_boolstring(value);
		src->data = g_variant_new_boolean(tmp_bool);
		break;
	case SR_T_FLOAT:
		tmp_double = strtof(value, NULL);
		src->data = g_variant_new_double(tmp_double);
		break;
	case SR_T_RATIONAL_PERIOD:
		if ((ret = sr_parse_period(value, &p, &q)) != SR_OK)
			break;
		rational[0] = g_variant_new_uint64(p);
		rational[1] = g_variant_new_uint64(q);
		src->data = g_variant_new_tuple(rational, 2);
		break;
	case SR_T_RATIONAL_VOLT:
		if ((ret = sr_parse_voltage(value, &p, &q)) != SR_OK)
			break;
		rational[0] = g_variant_new_uint64(p);
		rational[1] = g_variant_new_uint64(q);
		src->data = g_variant_new_tuple(rational, 2);
		break;
	case SR_T_UINT64_RANGE:
		if (sscanf(value, "%"PRIu64"-%"PRIu64, &low, &high) != 2) {
			ret = -1;
			break;
		} else {
			range[0] = g_variant_new_uint64(low);
			range[1] = g_variant_new_uint64(high);
			src->data = g_variant_new_tuple(range, 2);
		}
		break;
	case SR_T_DOUBLE_RANGE:
		if (sscanf(value, "%lf-%lf", &dlow, &dhigh) != 2) {
			ret = -1;
			break;
		} else {
			range[0] = g_variant_new_double(dlow);
			range[1] = g_variant_new_double(dhigh);
			src->data = g_variant_new_tuple(range, 2);
		}
		break;
	case SR_T_KEYVALUE:
		/* Expects the argument to be in the form of key=value. */
		keyval = g_strsplit(value, "=", 2);
		if (!keyval[0] || !keyval[1]) {
			g_strfreev(keyval);
			ret = -1;
			break;
		} else {
			vbl = g_variant_builder_new(G_VARIANT_TYPE_DICTIONARY);
			g_variant_builder_add(vbl, "{ss}",
					      keyval[0], keyval[1]);
			src->data = g_variant_builder_end(vbl);
			g_strfreev(keyval);
		}
		break;
	case SR_T_MQ:
		/*
		  Argument is MQ id e.g. ("voltage") optionally followed by one
		  or more /<mqflag> e.g. "/ac".
		 */
		keyval = g_strsplit(value, "/", 0);
		if (!keyval[0] || !(srmqi = sr_key_info_name_get(SR_KEY_MQ, keyval[0]))) {
			g_strfreev(keyval);
			ret = -1;
			break;
		}
		mq = srmqi->key;
		mqflags = 0;
		for (i = 1; keyval[i]; i++) {
			if (!(srmqi = sr_key_info_name_get(SR_KEY_MQFLAGS, keyval[i]))) {
				ret = -1;
				break;
			}
			mqflags |= srmqi->key;
		}
		g_strfreev(keyval);
		if (ret != -1) {
			gtup[0] = g_variant_new_uint32(mq);
			gtup[1] = g_variant_new_uint64(mqflags);
			src->data = g_variant_new_tuple(gtup, 2);
		}
		break;
	default:
		g_critical("Unknown data type specified for option '%s' "
			   "(driver implementation bug?).", key);
		ret = -1;
	}

	if (ret < 0)
		g_critical("Invalid value: '%s' for option '%s'", value, key);

	return ret;
}

int set_dev_options_array(struct sr_dev_inst *sdi, char **opts)
{
	size_t opt_idx;
	const char *opt_text;
	GHashTable *args;
	int ret;

	for (opt_idx = 0; opts && opts[opt_idx]; opt_idx++) {
		opt_text = opts[opt_idx];
		args = parse_generic_arg(opt_text, FALSE, "channel_group");
		if (!args)
			continue;
		ret = set_dev_options(sdi, args);
		g_hash_table_destroy(args);
		if (ret != SR_OK)
			return ret;
	}

	return SR_OK;
}

int set_dev_options(struct sr_dev_inst *sdi, GHashTable *args)
{
	struct sr_config src;
	const char *cg_name;
	struct sr_channel_group *cg;
	GHashTableIter iter;
	gpointer key, value;
	int ret;

	/*
	 * Not finding the 'sigrok_key' key (optional user specified
	 * channel group name) in the current options group's hash table
	 * is perfectly fine. In that case the -g selection is used,
	 * which defaults to "the device" (global parameters).
	 */
	cg_name = g_hash_table_lookup(args, "sigrok_key");
	cg = lookup_channel_group(sdi, cg_name);

	g_hash_table_iter_init(&iter, args);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		if (g_ascii_strcasecmp(key, "sigrok_key") == 0)
			continue;
		if ((ret = opt_to_gvar(key, value, &src)) != 0)
			return ret;
		if ((ret = maybe_config_set(sr_dev_inst_driver_get(sdi), sdi, cg,
				src.key, src.data)) != SR_OK) {
			g_critical("Failed to set device option '%s': %s.",
				   (char *)key, sr_strerror(ret));
			return ret;
		}
	}

	return SR_OK;
}

void run_session(void)
{
	struct df_arg_desc df_arg;
	GSList *devices, *real_devices, *sd;
	GVariant *gvar;
	struct sr_session *session;
	struct sr_trigger *trigger;
	struct sr_dev_inst *sdi;
	uint64_t min_samples, max_samples;
	GArray *drv_opts;
	guint i;
	int is_demo_dev;
	struct sr_dev_driver *driver;
	const struct sr_transform *t;
	GMainLoop *main_loop;

	memset(&df_arg, 0, sizeof(df_arg));
	df_arg.do_props = FALSE;

	devices = device_scan();
	if (!devices) {
		g_critical("No devices found.");
		return;
	}

	real_devices = NULL;
	for (sd = devices; sd; sd = sd->next) {
		sdi = sd->data;

		driver = sr_dev_inst_driver_get(sdi);

		if (!(drv_opts = sr_dev_options(driver, NULL, NULL))) {
			g_critical("Failed to query list of driver options.");
			return;
		}

		is_demo_dev = 0;
		for (i = 0; i < drv_opts->len; i++) {
			if (g_array_index(drv_opts, uint32_t, i) == SR_CONF_DEMO_DEV)
				is_demo_dev = 1;
		}

		g_array_free(drv_opts, TRUE);

		if (!is_demo_dev)
			real_devices = g_slist_append(real_devices, sdi);
	}

	if (g_slist_length(devices) > 1) {
		if (g_slist_length(real_devices) != 1) {
			g_critical("u3logicanalyzer-cli only supports one device for capturing.");
			return;
		} else {
			/* We only have one non-demo device. */
			g_slist_free(devices);
			devices = real_devices;
			real_devices = NULL;
		}
	}

	/* This is unlikely to happen but it makes static analyzers stop complaining. */
	if (!devices) {
		g_critical("No real devices found.");
		return;
	}

	sdi = devices->data;
	g_slist_free(devices);
	g_slist_free(real_devices);

	sr_session_new(sr_ctx, &session);
	/*
	 * The datafeed callback stops the session once a limit is reached, so
	 * it needs the pointer. Clearing it before registration would turn
	 * every sr_session_stop() call in there into a no-op.
	 */
	df_arg.session = session;
	sr_session_datafeed_callback_add(session, datafeed_in, &df_arg);

	if (sr_dev_open(sdi) != SR_OK) {
		g_critical("Failed to open device.");
		return;
	}

	if (sr_session_dev_add(session, sdi) != SR_OK) {
		g_critical("Failed to add device to session.");
		sr_session_destroy(session);
		return;
	}

	/*
	 * The channels have to be in place before the other options are
	 * set. Which of the two widths the device is put into decides what
	 * sample rates it offers, so a rate set beforehand would be thrown
	 * away when the width is switched.
	 */
	if (select_channels(sdi) != SR_OK) {
		g_critical("Failed to set channels.");
		sr_session_destroy(session);
		return;
	}

	if (apply_logic_channel_count(sdi) != SR_OK) {
		g_critical("Failed to set the channel count.");
		sr_session_destroy(session);
		return;
	}

	if (opt_configs) {
		if (set_dev_options_array(sdi, opt_configs) != SR_OK)
			return;
	}

	trigger = NULL;
	if (opt_triggers) {
		if (!parse_triggerstring(sdi, opt_triggers, &trigger)) {
			sr_session_destroy(session);
			return;
		}
		if (sr_session_trigger_set(session, trigger) != SR_OK) {
			sr_session_destroy(session);
			return;
		}
	}

	if (opt_continuous) {
		if (!sr_dev_has_option(sdi, SR_CONF_CONTINUOUS)) {
			g_critical("This device does not support continuous sampling.");
			sr_session_destroy(session);
			return;
		}
	}

	if (opt_time) {
		if (set_limit_time(sdi) != SR_OK) {
			sr_session_destroy(session);
			return;
		}
	}

	if (opt_samples) {
		if ((sr_parse_sizestring(opt_samples, &limit_samples) != SR_OK)) {
			g_critical("Invalid sample limit '%s'.", opt_samples);
			sr_session_destroy(session);
			return;
		}
		if (maybe_config_list(driver, sdi, NULL, SR_CONF_LIMIT_SAMPLES,
				&gvar) == SR_OK) {
			/*
			 * The device has no compression, or compression is turned
			 * off, and publishes its sample memory size.
			 */
			g_variant_get(gvar, "(tt)", &min_samples, &max_samples);
			g_variant_unref(gvar);
			if (limit_samples < min_samples) {
				g_critical("The device stores at least %"PRIu64
						" samples with the current settings.", min_samples);
			}
			if (limit_samples > max_samples) {
				g_critical("The device can store only %"PRIu64
						" samples with the current settings.", max_samples);
			}
		}
		gvar = g_variant_new_uint64(limit_samples);
		if (maybe_config_set(sr_dev_inst_driver_get(sdi), sdi, NULL, SR_CONF_LIMIT_SAMPLES, gvar) != SR_OK) {
			g_critical("Failed to configure sample limit.");
			sr_session_destroy(session);
			return;
		}
	}

	if (opt_frames) {
		if ((sr_parse_sizestring(opt_frames, &limit_frames) != SR_OK)) {
			g_critical("Invalid frame limit '%s'.", opt_frames);
			sr_session_destroy(session);
			return;
		}
		gvar = g_variant_new_uint64(limit_frames);
		if (maybe_config_set(sr_dev_inst_driver_get(sdi), sdi, NULL, SR_CONF_LIMIT_FRAMES, gvar) != SR_OK) {
			g_critical("Failed to configure frame limit.");
			sr_session_destroy(session);
			return;
		}
	}

	if (opt_transform_module) {
		if (!(t = setup_transform_module(sdi)))
			g_critical("Failed to initialize transform module.");
	}

	main_loop = g_main_loop_new(NULL, FALSE);

	sr_session_stopped_callback_set(session,
		(sr_session_stopped_callback)g_main_loop_quit, main_loop);

	if (sr_session_start(session) != SR_OK) {
		g_critical("Failed to start session.");
		g_main_loop_unref(main_loop);
		sr_session_destroy(session);
		return;
	}

	if (opt_continuous)
		add_anykey(session);

	g_main_loop_run(main_loop);

	if (opt_continuous)
		clear_anykey();

	if (trigger)
		sr_trigger_free(trigger);

	sr_session_datafeed_callback_remove_all(session);
	g_main_loop_unref(main_loop);
	df_arg.session = NULL;
	sr_session_destroy(session);
}
