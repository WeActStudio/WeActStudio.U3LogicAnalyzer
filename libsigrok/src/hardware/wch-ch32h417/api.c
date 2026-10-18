/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on libsigrok.
 *
 * Copyright (C) 2026 Q2H2
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
#include "protocol.h"
#include <math.h>
#include <stdio.h>

/* Supported devices */
static const struct ch32h417_profile supported_devices[] = {
	/* CH32H417 logic analyzer */
	{USB_VENDOR_ID, USB_PRODUCT_ID, "CH32H417", "CH32H417 Logic Analyzer", "01", 
	 DEV_CAPS_16BIT_LOGIC | DEV_CAPS_ADC},
	{USB_VENDOR_ID, USB_PRODUCT_ID, "CH32H417", "WeAct Studio Logic Analyzer V2", "b5", 
	 DEV_CAPS_16BIT_LOGIC},
	ALL_ZERO};

/* Scan options */
static const uint32_t scanopts[] = {
	SR_CONF_CONN,
};

/* Driver options */
static const uint32_t drvopts[] = {
	SR_CONF_LOGIC_ANALYZER,
};

/* Device options */
static const uint32_t devopts[] = {
	SR_CONF_CONTINUOUS,
	SR_CONF_LIMIT_FRAMES | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_LIMIT_SAMPLES | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_CONN | SR_CONF_GET,
	SR_CONF_SAMPLERATE | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
	SR_CONF_TRIGGER_MATCH | SR_CONF_LIST,
	SR_CONF_CAPTURE_RATIO | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_NUM_LOGIC_CHANNELS | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_PATTERN_MODE | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
	SR_CONF_VOLTAGE_THRESHOLD | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
	SR_CONF_HARDWARE_VERSION | SR_CONF_GET,
	SR_CONF_USB_VERSION | SR_CONF_GET | SR_CONF_LIST,
	SR_CONF_LINK_STOP_REASON | SR_CONF_GET | SR_CONF_LIST,
};

/* ADC channel group device options */
static const uint32_t devopts_adc[] = {
	SR_CONF_SAMPLERATE | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
	SR_CONF_LIMIT_SAMPLES | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_ADC_PRECISION | SR_CONF_GET | SR_CONF_SET,
	SR_CONF_ADC_CHANNEL | SR_CONF_GET | SR_CONF_SET,
};

/* Trigger types */
static const int32_t trigger_matches[] = {
	SR_TRIGGER_ZERO,
	SR_TRIGGER_ONE,
	SR_TRIGGER_RISING,
	SR_TRIGGER_FALLING,
	SR_TRIGGER_EDGE,
};

/* Operating mode names */
static const char *mode_names[] = {
	[MODE_LOGIC_ANALYZER] = "Logic Analyzer",
	[MODE_ADC] = "ADC",
};

/*
 * Values of SR_CONF_USB_VERSION: 2 is USB 2.0 and 3 is USB 3.0, which is
 * what config_get() answers for the link the device is on.
 */
static const uint64_t usb_versions[] = {
	2, 3,
};

/* Values of SR_CONF_LINK_STOP_REASON, see the LINK_STOP_* in protocol.h. */
static const uint64_t link_stop_reasons[] = {
	LINK_STOP_NONE,
	LINK_STOP_NO_DATA,
};

static const uint16_t voltage_to_raw[][2] = {
	{23, 33},
	{22, 93},
	{21, 123},
	{20, 200},
	{19, 225},
	{18, 280},
	{17, 308},
	{16, 400},
	{15, 428},
	{14, 500},
	{13, 545},
	{12, 574},
	{11, 620},
	{10, 675},
	{9, 725},
	{8, 751},
};

static char serial_number[256] = {0};

static void ch32h417_flush_cmd_endpoint(unsigned long index)
{
	uint8_t buffer[PACKET_SIZE_IN];
	unsigned long io_length;

	/* Set the timeout to 0 ms */
	if (!ch375_set_timeout_ex(index, 1, 1, 1, 1)) {
		sr_warn("Failed to set timeout ex for device at index %d", index);
	}
	g_usleep(1000);

	uint8_t retry_count = 0;
	while(1)
	{
		io_length = PACKET_SIZE_IN;
		if(!ch375_read_endpoint(index, PIPE_CMD, buffer, &io_length))
			break;
		else{
			if(io_length == 0)
				break;
			sr_warn("Device has data not read, ignore it. Data: %02x", buffer[0]);
		}
		retry_count++;
		if(retry_count > 10)
		{
			sr_err("Timeout while flushing command endpoint for device at index %d", index);
			break;
		}
		g_usleep(1000);
	}
	g_usleep(1000);
}

/* Initialize the CH375 device communication parameters */
static void ch375_init_comm_params(struct dev_context *devc)
{
	/* Set the IO mode to synchronous */
	if (!ch375_set_io_mode(devc->device_index, 0)) {
		sr_warn("Failed to set IO mode to sync for device at index %d", devc->device_index);
	}

	/* Flush the PIPE_CMD endpoint */
	ch32h417_flush_cmd_endpoint(devc->device_index);

	/* Set the timeout */
	if (!ch375_set_timeout_ex(devc->device_index, USB_TIMEOUT, USB_TIMEOUT, USB_TIMEOUT, USB_TIMEOUT)) {
		sr_warn("Failed to set timeout ex for device at index %d", devc->device_index);
	}

	/* Configure the buffered upload pipe */
	if (!ch375_set_buf_upload_ex(devc->device_index, 1, PIPE_LOGIC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_LOGIC_DATA, devc->device_index);
	}
	if (!ch375_set_buf_upload_ex(devc->device_index, 1, PIPE_ADC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_ADC_DATA, devc->device_index);
	}
	if (!ch375_set_buf_download_ex(devc->device_index, 0, PIPE_CMD, 0)) {
		sr_warn("Failed to set download buffer pipe %d for device at index %d", PIPE_CMD, devc->device_index);
	}
}

/**
 * @brief Parse a USB device path string and extract the instance serial number in lower case
  * @param dev_path Input string, e.g. "\\\\?\\usb#vid_1a86&pid_5537#b5e339e33906e19c7f6416064c#{5e7f6bdf-1ce5-4d78-bbcf-d20c44329f7d}"
 * @param out_sn Output buffer that receives the lower-case serial number
 * @param buf_sz Output buffer size
 * @return 0 on success, -1 on failure
 */
static int extract_usb_serial(const char *dev_path, char *out_sn, size_t buf_sz)
{
    const char *p1, *p2, *p3;
    size_t len;

    if (!dev_path || !out_sn || buf_sz == 0)
        return -1;

    // Find the first '#'
    p1 = strchr(dev_path, '#');
    if (p1 == NULL)
        return -1;
    p1++;

    // Find the second '#'
    p2 = strchr(p1, '#');
    if (p2 == NULL)
        return -1;
	p2++;

	// Find the third '#'
    p3 = strchr(p2, '#');
    if (p3 == NULL)
        return -1;

    len = (size_t)(p3 - p2);
    if (len >= buf_sz)
        return -1;

    memcpy(out_sn, p2, len);
    out_sn[len] = '\0';

    // Convert everything to lower case
    for (size_t i = 0; i < len; i++)
    {
        out_sn[i] = (char)tolower((unsigned char)out_sn[i]);
    }
    return 0;
}

/**
 * @brief Check whether str starts with the given prefix
 * @return 1 if it is a prefix, 0 otherwise
 */
static int str_startswith(const char *str, const char *prefix)
{
    if (!str || !prefix)
        return 0;
    size_t pre_len = strlen(prefix);
    size_t str_len = strlen(str);
    if (str_len < pre_len)
        return 0;
    return (memcmp(str, prefix, pre_len) == 0);
}


/* Scan for devices */
static GSList *scan(struct sr_dev_driver *di, GSList *options)
{
	struct drv_context *drvc;
	struct dev_context *devc;
	struct sr_dev_inst *sdi;
	struct sr_channel *ch;
	struct sr_channel_group *cg;
	struct sr_config *src;
	const struct ch32h417_profile *prof;
	GSList *l, *devices;
	int ret, i, j;
	int num_logic_channels, num_analog_channels;
	const char *conn;
	char connection_id[64];
	char channel_name[16];
	void *handle;
	unsigned long usb_id;
	uint16_t vid, pid;

	drvc = di->context;

	conn = NULL;
	for (l = options; l; l = l->next)
	{
		src = l->data;
		switch (src->key)
		{
		case SR_CONF_CONN:
			conn = g_variant_get_string(src->data, NULL);
			break;
		}
	}

	devices = NULL;

	/* Initialize CH375DLL */
	ret = ch375_init();
	if (ret != SR_OK)
	{
		sr_err("Failed to initialize CH375DLL");
		return NULL;
	}

	for (i = 0; i < CH375_MAX_NUMBER; i++)
	{
		handle = ch375_open_device(i);
		if (handle == CH375_INVALID_HANDLE)
			continue;

		usb_id = ch375_get_usb_id(i);
		vid = (uint16_t)(usb_id & 0xFFFF);
		pid = (uint16_t)(usb_id >> 16);

		sr_dbg("Found device at index %d: VID=0x%04x, PID=0x%04x", i, vid, pid);

		const char *device_name = ch375_get_device_name(i);
		
		/* scan() may inspect more than one device, so the buffer has to be
		 * cleared before every attempt: a device whose name cannot be parsed
		 * would otherwise inherit the serial number of the previous one. */
		memset(serial_number, 0, sizeof(serial_number));
		if (extract_usb_serial(device_name, serial_number,
							 sizeof(serial_number)) != 0)
			sr_warn("Could not parse a serial number from the device name");

		sr_dbg("Device name: %s", device_name ? device_name : "(unknown)");
		sr_dbg("USB serial: %s", serial_number);

		prof = NULL;
		for (j = 0; supported_devices[j].vid; j++)
		{
			if (vid == supported_devices[j].vid &&
				pid == supported_devices[j].pid &&
				str_startswith(serial_number, supported_devices[j].SN_start))
			{
				prof = &supported_devices[j];
				break;
			}
		}

		if (!prof)
		{
			ch375_close_device(i);
			continue;
		}

		/*
		 * The connection id is the place on the USB bus the board sits in
		 * - "ch375/<hub>-<port>" - so it survives a replug, unlike the
		 * index, which moves as soon as another board is unplugged. The
		 * index is only the fallback, for a board whose place cannot be
		 * read.
		 */
		char usb_path[64];
		if (ch375_get_usb_path(i, usb_path, sizeof(usb_path)) == 0)
			snprintf(connection_id, sizeof(connection_id), "%s", usb_path);
		else
			snprintf(connection_id, sizeof(connection_id), "ch375/%d", i);

		/*
		 * One entry per board. The same board answers on more than one
		 * index: a composite device once per interface, and an index a
		 * board held before it was pulled keeps answering for a moment
		 * after it comes back. The connection id is the port it sits in,
		 * so a second one with the same id is the same board - listing it
		 * twice would only offer the user a duplicate.
		 */
		{
			GSList *l;
			for (l = devices; l; l = l->next) {
				const struct sr_dev_inst *listed = l->data;
				if (listed->connection_id &&
					!strcmp(listed->connection_id, connection_id))
					break;
			}

			if (l) {
				sr_info("Device at index %d: already listed as '%s'",
					i, connection_id);
				ch375_close_device(i);
				continue;
			}
		}

		sr_info("Device at index %d: connection id '%s'", i, connection_id);

		sdi = g_malloc0(sizeof(struct sr_dev_inst));
		sdi->status = SR_ST_INITIALIZING;
		sdi->vendor = g_strdup(prof->vendor);
		sdi->model = g_strdup(prof->model);
		sdi->connection_id = g_strdup(connection_id);
		sdi->serial_num = g_strdup(serial_number);

		/* The CH32H417 supports 8- and 16-channel modes */
		num_logic_channels = 16; /* 16 channels by default, switchable to 8 channels through configuration */
		num_analog_channels = (prof->dev_caps & DEV_CAPS_ADC) ? 2 : 0;

		/* Create the logic channel group */
		cg = g_malloc0(sizeof(struct sr_channel_group));
		cg->name = g_strdup("Logic");
		for (j = 0; j < num_logic_channels; j++)
		{
			sprintf(channel_name, "D%d", j);
			ch = sr_channel_new(sdi, j, SR_CHANNEL_LOGIC,
								TRUE, channel_name);
			cg->channels = g_slist_append(cg->channels, ch);
		}
		sdi->channel_groups = g_slist_append(NULL, cg);

		/* Create the analog channel group */
		for (j = 0; j < num_analog_channels; j++)
		{
			snprintf(channel_name, sizeof(channel_name), "A%d", j);
			ch = sr_channel_new(sdi, j + num_logic_channels,
								SR_CHANNEL_ANALOG, TRUE, channel_name);

			cg = g_malloc0(sizeof(struct sr_channel_group));
			cg->name = g_strdup(channel_name);
			cg->channels = g_slist_append(NULL, ch);
			sdi->channel_groups = g_slist_append(sdi->channel_groups, cg);
		}

		devc = ch32h417_dev_new();
		devc->profile = prof;
		devc->device_index = i;

		/*
		 * The DLL keeps one handle per index, so an instance that has only
		 * listed a board must not keep it: the instance that is opened on
		 * that board takes the handle for itself, and a rescan hands out a
		 * new instance while the old one still holds the old handle. The
		 * scan still opens the board - reading its identity needs that -
		 * but lets go of it once the identity is read (see the end of this
		 * loop). dev_open() takes the handle when the board is opened.
		 */
		devc->device_handle = NULL;

		sdi->priv = devc;

		sdi->inst_type = SR_INST_USB;
		sdi->conn = NULL;

		sdi->status = SR_ST_INACTIVE;
		devices = g_slist_append(devices, sdi);

		sr_info("Found CH32H417 device at index %d", i);

		/* The identity is read; the handle is not needed any more. */
		ch375_close_device(i);

		/*
		 * Carry on: two boards of the same model differ only in their
		 * index, and CH375OpenDevice answers each of them separately.
		 * Stopping at the first one hides the others.
		 */
	}

	return std_scan_complete(di, devices);
}

/*
 * Whether the index an instance holds still stands for the board it was
 * made for. The DLL knows a board by the order it enumerates them, so
 * pulling or adding one moves every index after it: closing by an index
 * that now belongs to another board would shut that board's handle, and
 * the board would fail on every call it makes from then on.
 */
static gboolean index_is_ours(const struct sr_dev_inst *sdi, unsigned long index)
{
	char path[80] = "";

	/*
	 * Only a bus path can be compared: the fallback scan writes when the
	 * path cannot be read is "ch375/<index>", which says nothing about the
	 * board beyond the index the DLL handed out.
	 */
	if (!sdi->connection_id ||
		!g_str_has_prefix(sdi->connection_id, "ch375/") ||
		!strchr(sdi->connection_id + 6, '-'))
		return TRUE;

	/*
	 * A board that no longer answers has given its index away; closing by
	 * it would reach whichever board holds it now.
	 */
	if (ch375_get_usb_path(index, path, sizeof(path)) != 0)
		return FALSE;

	return g_strcmp0(path, sdi->connection_id) == 0;
}

/*
 * Release the resources owned by a device instance before the standard
 * helper frees it. scan() opens every device it inspects, so an instance
 * that was never opened through dev_open() still holds a handle when it is
 * cleared -- the standard helper only closes instances that reached
 * SR_ST_ACTIVE. The mutex created in ch32h417_dev_new() is cleared here as
 * well: on Windows GMutex wraps a CRITICAL_SECTION, which is not released
 * by simply freeing the structure that contains it.
 */
static void dev_clear_private(void *priv)
{
	struct dev_context *devc = priv;

	if (!devc)
		return;

	/*
	 * The handle is let go of by dev_close(), which knows which board the
	 * instance stands for. Closing here would go by the index alone, and
	 * an index outlives the board it once named: the board that has it now
	 * would be shut instead. An instance that never reached dev_close()
	 * leaves its handle to the DLL, which drops it when the process ends.
	 */
	devc->device_handle = NULL;

	g_mutex_clear(&devc->mutex);
}

/* Clear devices */
static int dev_clear(const struct sr_dev_driver *di)
{
	/*
	 * Wait for the read threads before the instances are freed: they use
	 * both the device context and the device instance, neither of which
	 * may be released while a thread is still running.
	 */
	struct drv_context *drvc = (struct drv_context *)di->context;
	if (drvc) {
		GSList *l;
		for (l = drvc->instances; l; l = l->next)
			ch32h417_join_read_thread(l->data);
	}

	return std_dev_clear_with_callback(di, dev_clear_private);
}

/*
 * Select the sample rate table to expose for the current device state.
 *
 * A serial number starting with "b5" identifies a device that supports
 * the full table; older ones only expose a subset (a single rate on
 * USB 2.0, the non-legacy rates on USB 3.0, see the "- 11" below).
 *
 * Call this whenever the channel count, the USB speed or the operating
 * mode changes; it leaves devc->cur_samplerate alone.
 */
static void ch32h417_update_samplerate_list(struct dev_context *devc)
{
	const gboolean full_table = str_startswith(devc->profile->SN_start, "b5");

	if (devc->mode != MODE_LOGIC_ANALYZER)
	{
		devc->samplerates = adc_samplerates;
		devc->num_samplerates = ARRAY_SIZE(adc_samplerates);
		return;
	}

	if (devc->is_usb2)
	{
		/* The width is what a sample costs on the wire, and any count
		 * above eight takes two bytes - not just sixteen. */
		if (devc->sample_width == SAMPLE_WIDTH_16BIT)
		{
			devc->samplerates = logic_samplerates_usb2_16ch;
			devc->num_samplerates = full_table ?
				ARRAY_SIZE(logic_samplerates_usb2_16ch) : 1;
		}
		else
		{
			devc->samplerates = logic_samplerates_usb2_8ch;
			devc->num_samplerates = full_table ?
				ARRAY_SIZE(logic_samplerates_usb2_8ch) : 1;
		}
	}
	else
	{
		devc->samplerates = logic_samplerates_usb3;
		devc->num_samplerates = full_table ?
			ARRAY_SIZE(logic_samplerates_usb3) :
			ARRAY_SIZE(logic_samplerates_usb3) - 11;
	}
}

/* Open device */
static int dev_open(struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	void *handle;
	gboolean already_open;

	devc = sdi->priv;
	already_open = (devc->device_handle != NULL && devc->device_handle != CH375_INVALID_HANDLE);


	if (already_open)
	{
		sr_info("Device already open at index %d", devc->device_index);
	}
	else
	{
		handle = ch375_open_device(devc->device_index);
		if (handle == CH375_INVALID_HANDLE)
		{
			sr_err("Failed to open device at index %d", devc->device_index);
			return SR_ERR;
		}

		devc->device_handle = handle;
		ch375_init_comm_params(devc);
	}

	if (devc->mode == MODE_LOGIC_ANALYZER)
	{
		devc->is_usb2 = ch32h417_detect_usb_speed(sdi);
		ch32h417_update_samplerate_list(devc);
		devc->cur_samplerate = devc->samplerates[0];
	}

	/* Set the default sample rate */
	if (devc->cur_samplerate == 0)
	{
		devc->cur_samplerate = devc->samplerates[0];
	}

	sr_info("Opened device at index %d", devc->device_index);

	return SR_OK;
}

/* Close device */
static int dev_close(struct sr_dev_inst *sdi)
{
	struct dev_context *devc;

	devc = sdi->priv;

	if (!devc->device_handle || devc->device_handle == CH375_INVALID_HANDLE)
		return SR_ERR_BUG;

	sr_info("Closing device at index %d", devc->device_index);

	/* The index moved on to another board; that one is not ours to close. */
	if (index_is_ours(sdi, devc->device_index))
		ch375_close_device(devc->device_index);

	devc->device_handle = NULL;

	return SR_OK;
}

/* Get configuration */
static int config_get(uint32_t key, GVariant **data,
					  const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	struct dev_context *devc;

	(void)cg;

	if (!sdi)
		return SR_ERR_ARG;

	devc = sdi->priv;

	switch (key)
	{
	case SR_CONF_CONN:
		if (!sdi->connection_id)
			return SR_ERR_ARG;
		*data = g_variant_new_string(sdi->connection_id);
		break;
	case SR_CONF_LIMIT_FRAMES:
		*data = g_variant_new_uint64(devc->limit_frames);
		break;
	case SR_CONF_LIMIT_SAMPLES:
		*data = g_variant_new_uint64(devc->limit_samples);
		break;
	case SR_CONF_SAMPLERATE:
		*data = g_variant_new_uint64(devc->cur_samplerate);
		break;
	case SR_CONF_CAPTURE_RATIO:
		*data = g_variant_new_uint64(devc->capture_ratio);
		break;
	case SR_CONF_NUM_LOGIC_CHANNELS:
		*data = g_variant_new_int32(devc->num_channels);
		break;
	case SR_CONF_PATTERN_MODE:
		*data = g_variant_new_string(mode_names[devc->mode]);
		break;
	case SR_CONF_HARDWARE_VERSION:
		ch32h417_get_fw_version(sdi);
		*data = g_variant_new_uint64(devc->fw_version);
		break;
	case SR_CONF_USB_VERSION:
		/* 2 means USB 2.0, 3 means USB 3.0 */
		*data = g_variant_new_uint64(devc->is_usb2 ? 2 : 3);
		break;
	case SR_CONF_LINK_STOP_REASON:
		/* See the LINK_STOP_* values in protocol.h. */
		*data = g_variant_new_uint64((uint64_t)devc->link_stop_reason);
		break;
	case SR_CONF_ADC_PRECISION:
		/* Return the ADC resolution: 8 or 10 */
		*data = g_variant_new_uint64((devc->adc_width == ADC_WIDTH_8BIT) ? 8 : 10);
		break;
	case SR_CONF_ADC_CHANNEL:
		/* Return the ADC channel: 0 or 1 */
		*data = g_variant_new_uint64((devc->adc_channel == ADC_CHANNEL_1) ? 0 : 1);
		break;
	case SR_CONF_VOLTAGE_THRESHOLD:
		*data = std_gvar_tuple_double(devc->cur_threshold, devc->cur_threshold);
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

/* Set configuration */
static int config_set(uint32_t key, GVariant *data,
					  const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	struct dev_context *devc;
	int idx;

	(void)cg;

	if (!sdi)
		return SR_ERR_ARG;

	devc = sdi->priv;

	switch (key)
	{
	case SR_CONF_SAMPLERATE:
		if ((idx = std_u64_idx(data, devc->samplerates,
							   devc->num_samplerates)) < 0)
			return SR_ERR_ARG;
		devc->cur_samplerate = devc->samplerates[idx];
		sr_info("Samplerate set to %" PRIu64 " Hz", devc->cur_samplerate);
		break;
	case SR_CONF_LIMIT_FRAMES:
		devc->limit_frames = g_variant_get_uint64(data);
		break;
	case SR_CONF_LIMIT_SAMPLES:
		devc->limit_samples = g_variant_get_uint64(data);
		break;
	case SR_CONF_CAPTURE_RATIO:
	{
		uint64_t ratio = g_variant_get_uint64(data);
		if (ratio > 100)
		{
			sr_err("Invalid capture ratio: %" PRIu64 " (must be 0-100)", ratio);
			return SR_ERR_ARG;
		}
		devc->capture_ratio = ratio;
		break;
	}
	case SR_CONF_NUM_LOGIC_CHANNELS:
	{
		int32_t num = g_variant_get_int32(data);
		if (num < 1 || num > 16)
		{
			sr_err("Invalid channel count: %" PRId32 " (must be 1 to 16)", num);
			return SR_ERR_ARG;
		}
		/*
		 * Eight channels fit in one byte per sample and sixteen take two,
		 * so the width is what the count decides - and the width is what
		 * picks the sample rate table. A capture of two or four channels
		 * is a capture of eight as far as the wire is concerned, and runs
		 * at the higher rates that go with the narrower samples.
		 */
		sr_info("Channel count set to %d", num);

		/* A different channel count selects a different sample rate table */
		if (devc->num_channels != num)
		{
			devc->num_channels = num;
			devc->sample_width = (num > 8) ?
				SAMPLE_WIDTH_16BIT : SAMPLE_WIDTH_8BIT;
			if (devc->is_usb2 && devc->mode == MODE_LOGIC_ANALYZER)
			{
				ch32h417_update_samplerate_list(devc);
				devc->cur_samplerate = devc->samplerates[0];
			}
		}
	}
	break;
	case SR_CONF_PATTERN_MODE:
	{
		int mode_idx = std_str_idx(data, ARRAY_AND_SIZE(mode_names));
		if (mode_idx < 0)
			return SR_ERR_ARG;

		/* Refuse the ADC mode on a device that has no ADC, which is
		 * what config_list() leaves out for it as well. */
		if (mode_idx == MODE_ADC &&
			!(devc->profile->dev_caps & DEV_CAPS_ADC)) {
			sr_err("This device has no ADC, so it cannot be set to the ADC mode");
			return SR_ERR_ARG;
		}

		if (mode_idx == devc->mode)
			break;

		devc->mode = (enum ch32h417_mode)mode_idx;

		/* Update the sample rate list when switching mode */
		ch32h417_update_samplerate_list(devc);

		devc->cur_samplerate = devc->samplerates[0];

		sr_info("Device mode set to %s", mode_names[devc->mode]);
	}
	break;
	case SR_CONF_ADC_PRECISION:
	{
		uint64_t precision = g_variant_get_uint64(data);
		if (precision == 8) {
			devc->adc_width = ADC_WIDTH_8BIT;
			sr_info("ADC precision set to 8-bit");
		} else if (precision == 10) {
			devc->adc_width = ADC_WIDTH_10BIT;
			sr_info("ADC precision set to 10-bit");
		} else {
			sr_err("Invalid ADC precision: %" PRIu64 " (must be 8 or 10)", precision);
			return SR_ERR_ARG;
		}
	}
	break;
	case SR_CONF_ADC_CHANNEL:
	{
		uint64_t channel = g_variant_get_uint64(data);
		if (channel == 0) {
			devc->adc_channel = ADC_CHANNEL_1;
			sr_info("ADC channel set to Channel 0");
		} else if (channel == 1) {
			devc->adc_channel = ADC_CHANNEL_2;
			sr_info("ADC channel set to Channel 1");
		} else {
			sr_err("Invalid ADC channel: %" PRIu64 " (must be 0 or 1)", channel);
			return SR_ERR_ARG;
		}
	}
	break;
	case SR_CONF_VOLTAGE_THRESHOLD:
	{
		double low, high;
		double threshold;
		g_variant_get(data, "(dd)", &low, &high);
		threshold = (low + high) / 2.0;
		uint16_t set = 0;
		if (str_startswith(devc->profile->SN_start, "b5"))
		{
			/*
			 * What is handed over is the level on the input pins, and
			 * the DAC is given the level at the input in 0.1 V. The two
			 * are not a fixed ratio of one another: measured, 0.6 V on
			 * the pins is 1.0 V at the input and 1.5 V on the pins is
			 * 3.3 V at it. The line between those two is what the level
			 * that was handed over is read back along.
			 */
			const double pin_lo = 0.6, pin_hi = 1.5;
			const double input_lo = 1.0, input_hi = 3.3;
			const double input = input_lo + (threshold - pin_lo) *
				(input_hi - input_lo) / (pin_hi - pin_lo);

			const double code = round(input * 10.0);

			if(code < 10.0 || code > 33.0)
			{
				sr_err("voltage_threshold Input Level value out of range: %.0f (must be 10-33)", code);
				return SR_ERR_ARG;
			}
			set = (uint16_t)code;
            set |= 0x8000;
		}
		else
		{
			set = (uint16_t)round(threshold * 10.0);
			for(int i = 0; i < sizeof(voltage_to_raw)/sizeof(voltage_to_raw[0]); i++)
			{
				if(set == voltage_to_raw[i][0])
				{	
					set = voltage_to_raw[i][1];
					break;
				}
			}
			if (set > 1024) {
				sr_err("voltage_threshold DAC value out of range: %" PRIu16 " (must be 0-1024)", set);
				return SR_ERR_ARG;
			}
		}
		devc->cur_threshold = threshold;
		sr_info("Setting cur_threshold to %f, %" PRIu16 "", devc->cur_threshold, set);
		return ch32h417_set_threshold_value(sdi, set);
	}
	break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

/* Configuration list */
static int config_list(uint32_t key, GVariant **data,
					   const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	struct dev_context *devc;

	devc = (sdi) ? sdi->priv : NULL;

	switch (key)
	{
	case SR_CONF_SCAN_OPTIONS:
	case SR_CONF_DEVICE_OPTIONS:
		if (cg)
		{
			/* ADC channel group */
			return STD_CONFIG_LIST(key, data, sdi, cg,
								   scanopts, drvopts, devopts_adc);
		}
		return STD_CONFIG_LIST(key, data, sdi, cg, scanopts, drvopts, devopts);
	case SR_CONF_SAMPLERATE:
		if (!devc)
			return SR_ERR_NA;
		*data = std_gvar_samplerates(devc->samplerates, devc->num_samplerates);
		break;
	case SR_CONF_TRIGGER_MATCH:
		*data = std_gvar_array_i32(ARRAY_AND_SIZE(trigger_matches));
		break;
	case SR_CONF_PATTERN_MODE:
	{
		/*
		 * Only a device that carries an ADC can be switched into the ADC
		 * mode, so the list is built for the device that is asked about
		 * instead of being the same one for every device.
		 */
		const char *modes[ARRAY_SIZE(mode_names)];
		size_t i, n;

		if (!devc || !devc->profile)
			return SR_ERR_NA;

		n = 0;
		for (i = 0; i < ARRAY_SIZE(mode_names); i++) {
			if (i == MODE_ADC && !(devc->profile->dev_caps & DEV_CAPS_ADC))
				continue;
			modes[n++] = mode_names[i];
		}

		*data = g_variant_new_strv(modes, n);
	}
	break;
	case SR_CONF_VOLTAGE_THRESHOLD:
	{
		/*
		 * The value is the level on the input pins, in volts, which is
		 * what the GUI shows and hands back, so nothing is converted
		 * between the two. Measured on the b5 variant: the pins take
		 * 0.6 V to 1.5 V, in 0.05 V steps, and that is 1.0 V to 3.3 V at
		 * the input. config_set() walks that line to reach the input
		 * level, which is what the DAC is given.
		 */
		double lo, hi, step_size;

		if (!devc || !devc->profile)
			return SR_ERR_NA;

		if (str_startswith(devc->profile->SN_start, "b5")) {
			lo = 0.6;  /* 1.0 V at the input */
			hi = 1.5;  /* 3.3 V at the input */
			step_size = 0.05;
		} else {
			lo = 0.8;  /* 0.8 V */
			hi = 2.3;  /* 2.3 V */
			step_size = 0.1;
		}

		*data = std_gvar_min_max_step_thresholds(lo, hi, step_size);
		break;
	}
	case SR_CONF_USB_VERSION:
		*data = std_gvar_array_u64(ARRAY_AND_SIZE(usb_versions));
		break;
	case SR_CONF_LINK_STOP_REASON:
		*data = std_gvar_array_u64(ARRAY_AND_SIZE(link_stop_reasons));
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

/* Stop acquisition */
static int dev_acquisition_stop(struct sr_dev_inst *sdi)
{
	return ch32h417_acquisition_stop(sdi);
}

/* Driver registration info */
static struct sr_dev_driver ch32h417_driver_info = {
	.name = "wch-ch32h417",
	.longname = "WCH CH32H417 Logic Analyzer",
	.api_version = 1,
	.init = std_init,
	.cleanup = std_cleanup,
	.scan = scan,
	.dev_list = std_dev_list,
	.dev_clear = dev_clear,
	.config_get = config_get,
	.config_set = config_set,
	.config_list = config_list,
	.dev_open = dev_open,
	.dev_close = dev_close,
	.dev_acquisition_start = ch32h417_acquisition_start,
	.dev_acquisition_stop = dev_acquisition_stop,
	.context = NULL,
};
SR_REGISTER_DEV_DRIVER(ch32h417_driver_info);
