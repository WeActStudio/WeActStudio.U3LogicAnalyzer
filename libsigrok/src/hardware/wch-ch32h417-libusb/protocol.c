/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on libsigrok.
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

#include <config.h>
#include <inttypes.h>
#include "protocol.h"
#include <math.h>

/*
 * Logic analyzer sample rates - USB 3.0 (up to 200 MHz).
 *
 * The index of an entry is the clock divider index that is sent to the
 * device, so entries may only ever be appended at the end; inserting a
 * rate in the middle would shift the divider of every entry behind it.
 * See samplerate_to_divider().
 */
const uint64_t ch32h417_libusb_logic_samplerates_usb3[] = {
	SR_MHZ(200),	
	SR_MHZ(187.5),	
	SR_MHZ(175),	
	SR_MHZ(162.5),	
	SR_MHZ(156.25), 
	SR_MHZ(150),	
	SR_MHZ(137.5),	
	SR_MHZ(125),	
	SR_MHZ(112.5),	
	SR_MHZ(100),	
	SR_MHZ(93.75),	
	SR_MHZ(87.5),	
	SR_MHZ(81.25),	
	SR_MHZ(78.125), 
	SR_MHZ(75),		
	SR_MHZ(68.75),	
	SR_MHZ(50),
	SR_MHZ(40),
	SR_MHZ(30),
	SR_MHZ(20),
	SR_MHZ(16),
	SR_MHZ(12),
	SR_MHZ(8),
	SR_MHZ(6),
	SR_MHZ(4),
};

/* Logic analyzer USB 2.0, 16 channels (up to 20 MHz) */
const uint64_t ch32h417_libusb_logic_samplerates_usb2_16ch[] = {
	SR_MHZ(20),
	SR_MHZ(16),
	SR_MHZ(12),
	SR_MHZ(8),
	SR_MHZ(6),
	SR_MHZ(4),
};

/* Logic analyzer USB 2.0, 8 channels (up to 40 MHz) */
const uint64_t ch32h417_libusb_logic_samplerates_usb2_8ch[] = {
	SR_MHZ(40),
	SR_MHZ(30),
	SR_MHZ(20),
	SR_MHZ(16),
	SR_MHZ(12),
	SR_MHZ(8),
	SR_MHZ(6),
	SR_MHZ(4),
};

/*
 * ADC sample rates, up to 20 MHz. The table index plus
 * CH32H417_ADC_DIVIDER_OFFSET is the divider index (see
 * ch32h417_libusb_set_adc_params()).
 */
const uint64_t ch32h417_libusb_adc_samplerates[] = {
	SR_MHZ(20),	  
	SR_MHZ(16),	  
	SR_MHZ(13.33),
	SR_MHZ(11.43),
	SR_MHZ(10),
};

/*
 * The divider index that the driver sends in params[1] is the index
 * into these tables (see samplerate_to_divider() below). The firmware
 * adds a fixed offset per USB mode (UHSIF_Clock_Set() in
 * ch32h417_libusb_logic.c):
 *
 *   USB 3.0,              cases 0x00 .. 0x18
 *   USB 2.0,  8 channels, cases 0x11 .. 0x18
 *   USB 2.0, 16 channels, cases 0x13 .. 0x18
 *
 * These assertions keep the tables in sync with the firmware and with
 * the sizes declared in protocol.h; a mismatch would make the device
 * sample at the wrong frequency.
 */
G_STATIC_ASSERT(ARRAY_SIZE(ch32h417_libusb_logic_samplerates_usb3) == 25);
G_STATIC_ASSERT(ARRAY_SIZE(ch32h417_libusb_logic_samplerates_usb2_8ch) == 8);
G_STATIC_ASSERT(ARRAY_SIZE(ch32h417_libusb_logic_samplerates_usb2_16ch) == 6);

/* ADC sample rates start at this divider index */
#define CH32H417_ADC_DIVIDER_OFFSET 3

static uint8_t resp_buffer[PACKET_SIZE_IN];
static int resp_len = 0;

SR_PRIV int ch32h417_libusb_send_command(struct sr_dev_inst *sdi,
							   uint8_t cmd, uint8_t len, const uint8_t *data)
{
	struct dev_context *devc;
	uint8_t out_buffer[PACKET_SIZE_OUT] = {0};
	unsigned long io_length;

	devc = sdi->priv;

	if (len > (PACKET_SIZE_OUT - 2))
	{
		sr_err("Command data too long: %d > %d", len, PACKET_SIZE_OUT - 2);
		return SR_ERR_ARG;
	}

	out_buffer[0] = cmd;
	out_buffer[1] = len;
	if (len > 0 && data)
		memcpy(&out_buffer[2], data, len);

	io_length = PACKET_SIZE_OUT;

	if (!ch32h417_libusb_write_endpoint(devc->device_index, PIPE_CMD, out_buffer, &io_length))
	{
		sr_err("Endpoint write failed for command 0x%02x", cmd);
		return SR_ERR;
	}

	sr_dbg("Sent command 0x%02x, len=%d, written=%lu bytes", cmd, len, io_length);

	return SR_OK;
}

/*
 * Receive a command response
 *
 * Response packet layout:
 *   [0]      Command byte (CMD_* | 0x10)
 *   [1]      Length field = status byte + number of payload bytes
 *   [2]      Status byte (0x00 means success)
 *   [3..]    Payload
 *
 * @param data      Caller-provided buffer, may be NULL
 * @param data_size Caller buffer capacity in bytes; pass 0 when data is NULL
 * @param len       Returns the payload bytes actually copied into data, may be NULL
 */
SR_PRIV int ch32h417_libusb_receive_response(struct sr_dev_inst *sdi,
								   uint8_t expected_cmd, uint8_t *data,
								   int data_size, int *len)
{
	struct dev_context *devc;
	unsigned long io_length;
	uint8_t resp_data_len;
	int payload_len, avail, copied;

	devc = sdi->priv;

	if (len)
		*len = 0;

	io_length = PACKET_SIZE_IN;

	if (!ch32h417_libusb_read_endpoint(devc->device_index, PIPE_CMD, resp_buffer, &io_length))
	{
		sr_err("Endpoint read failed for response");
		return SR_ERR;
	}

	resp_len = (int)io_length;
	if (resp_len < 0 || resp_len > PACKET_SIZE_IN)
		resp_len = PACKET_SIZE_IN;

	if (resp_len < 2)
	{
		sr_err("Response too short: %d bytes", resp_len);
		return SR_ERR;
	}

	if (resp_buffer[0] != expected_cmd)
	{
		sr_err("Unexpected response: 0x%02x (expected 0x%02x)",
			   resp_buffer[0], expected_cmd);
		return SR_ERR;
	}

	resp_data_len = resp_buffer[1];

	if (resp_data_len > 0)
	{
		if (resp_len < 3)
		{
			sr_err("Response 0x%02x has no status byte (%d bytes)",
				   expected_cmd, resp_len);
			return SR_ERR;
		}

		if (resp_buffer[2] != 0x00)
		{
			sr_err("Command failed with status 0x%02x", resp_buffer[2]);
			return SR_ERR;
		}
	}

	/* Payload length = length field minus the status byte */
	payload_len = (resp_data_len > 0) ? (resp_data_len - 1) : 0;

	/* Never exceed the bytes actually received, to avoid over-reading resp_buffer */
	avail = (resp_len > 3) ? (resp_len - 3) : 0;
	if (payload_len > avail)
	{
		sr_warn("Response 0x%02x declares %d data bytes, only %d received",
				expected_cmd, payload_len, avail);
		payload_len = avail;
	}

	copied = 0;
	if (data)
	{
		/* Never exceed the caller buffer capacity, to avoid an out-of-bounds write */
		copied = payload_len;
		if (copied > data_size)
		{
			sr_warn("Response 0x%02x truncated: %d data bytes > buffer %d",
					expected_cmd, copied, data_size);
			copied = data_size;
		}
		if (copied > 0)
			memcpy(data, &resp_buffer[3], copied);
	}

	if (len)
		*len = copied;

	sr_dbg("Received response 0x%02x, len=%d", resp_buffer[0], copied);

	return SR_OK;
}


/*
 * Map a sample rate to the hardware clock divider index.
 *
 * The divider index is the position of the rate inside the table the
 * driver handed to the device: the firmware takes the index that the host
 * sends in params[1] and looks it up in its own divider table
 * (UHSIF_Clock_Set(), "case 0x00 .. 0x18" in ch32h417_libusb_logic.c), adding a
 * fixed offset for USB 2.0 mode.
 *
 * @param rates      Sample rate table, in divider order.
 * @param num_rates  Number of entries in @p rates.
 * @param samplerate Rate to look up.
 *
 * @return The divider index, or -1 when the rate is not in the table.
 */
static int samplerate_to_divider(const uint64_t *rates,
								 unsigned int num_rates, uint64_t samplerate)
{
	unsigned int i;

	for (i = 0; i < num_rates; i++) {
		if (rates[i] == samplerate)
			return (int)i;
	}

	return -1;
}

SR_PRIV int ch32h417_libusb_set_logic_params(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	uint8_t params[2];
	int ret;

	devc = sdi->priv;

	/* Sample width */
	params[0] = devc->sample_width;

	/* Clock divider index */
	int div = samplerate_to_divider(devc->samplerates,
			(unsigned int)devc->num_samplerates, devc->cur_samplerate);
	if (div < 0)
	{
		sr_err("Invalid samplerate: %" PRIu64, devc->cur_samplerate);
		return SR_ERR_ARG;
	}
	params[1] = (uint8_t)div;

	sr_info("Setting logic params: width=0x%02x, div=%d", params[0], params[1]);

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_SET_LOGIC_PARAMS, 2, params);
	if (ret != SR_OK)
		return ret;

	return ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_SET_LOGIC_PARAMS, NULL, 0, NULL);
}

/*
 * Set the DAC threshold
 * Sends the two-byte DAC value: buf[0] = high byte, buf[1] = low byte
 * Value range 0-1024: 0 maps to about 3.3 V, 1024 to about 1.2 V
 */
SR_PRIV int ch32h417_libusb_set_threshold_value(const struct sr_dev_inst *sdi, uint16_t value)
{
	uint8_t params[2];
	int ret;

	params[0] = (value >> 8) & 0xFF;  /* high byte */
	params[1] = value & 0xFF;          /* low byte */

	sr_info("Setting DAC threshold: 0x%04x (%d)", value, value);

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_SET_LOGIC_LEVEL, 2, params);
	if (ret != SR_OK)
		return ret;

	return ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_SET_LOGIC_LEVEL, NULL, 0, NULL);
}

/*
 * Get the logic analyzer parameters
 */
SR_PRIV int ch32h417_libusb_get_logic_params(const struct sr_dev_inst *sdi,
								   uint8_t *width, uint8_t *div)
{
	uint8_t data[2] = {0};
	int len = 0;
	int ret;
	gint64 t1, t2;

	t1 = g_get_monotonic_time();
	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_GET_LOGIC_PARAMS, 0, NULL);
	t2 = g_get_monotonic_time();
	sr_dbg("ch32h417_libusb_get_logic_params: send_command took %" G_GINT64_FORMAT " us", t2 - t1);
	if (ret != SR_OK)
		return ret;

	t1 = g_get_monotonic_time();
	ret = ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_GET_LOGIC_PARAMS, data, (int)sizeof(data), &len);
	t2 = g_get_monotonic_time();
	sr_dbg("ch32h417_libusb_get_logic_params: receive_response took %" G_GINT64_FORMAT " us", t2 - t1);
	if (ret != SR_OK)
		return ret;

	if (width)
		*width = data[0];
	if (div)
		*div = data[1];

	sr_dbg("Got logic params: width=0x%02x, div=%d", data[0], data[1]);

	return SR_OK;
}

/*
 * Get the logic analyzer threshold level
 */
SR_PRIV int ch32h417_libusb_get_logic_level(const struct sr_dev_inst *sdi,
								  uint8_t *level)
{
	uint8_t data = 0;
	int len = 0;
	int ret;

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_GET_LOGIC_LEVEL, 0, NULL);
	if (ret != SR_OK)
		return ret;

	ret = ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_GET_LOGIC_LEVEL, &data, 1, &len);
	if (ret != SR_OK)
		return ret;

	if (level)
		*level = data;

	sr_dbg("Got logic level: %d", data);

	return SR_OK;
}

/*
 * Set the ADC parameters
 * Buf[0]: 0x08 = 8-bit ADC, 0x0a = 10-bit ADC
 * Buf[1]: clock divider index (0 = 20 MHz, 1 = 10 MHz, ..., divider 64 max)
 */
SR_PRIV int ch32h417_libusb_set_adc_params(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	uint8_t params[2];
	int ret;

	devc = sdi->priv;

	params[0] = devc->adc_width;

	/* Determine the clock divider index */
	int div = samplerate_to_divider(ch32h417_libusb_adc_samplerates,
			(unsigned int)ARRAY_SIZE(ch32h417_libusb_adc_samplerates), devc->cur_samplerate);
	if (div < 0)
	{
		sr_err("Invalid ADC samplerate: %" PRIu64, devc->cur_samplerate);
		return SR_ERR_ARG;
	}
	div += CH32H417_ADC_DIVIDER_OFFSET;
	params[1] = (uint8_t)div;

	sr_info("Setting ADC params: width=0x%02x, div=%d", params[0], params[1]);

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_SET_ADC_PARAMS, 2, params);
	if (ret != SR_OK)
		return ret;

	return ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_SET_ADC_PARAMS, NULL, 0, NULL);
}

/*
 * Set the ADC channel
 * Buf[0]: 0 = channel 1, 1 = channel 2
 */
SR_PRIV int ch32h417_libusb_set_adc_channel(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	uint8_t param;
	int ret;

	devc = sdi->priv;

	param = (uint8_t)devc->adc_channel;

	sr_info("Setting ADC channel: %d", param);

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_SET_ADC_CHANNEL, 1, &param);
	if (ret != SR_OK)
		return ret;

	return ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_SET_ADC_CHANNEL, NULL, 0, NULL);
}

/*
 * Get the ADC parameters
 */
SR_PRIV int ch32h417_libusb_get_adc_params(const struct sr_dev_inst *sdi,
								 uint8_t *width, uint8_t *div)
{
	uint8_t data[2] = {0};
	int len = 0;
	int ret;

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_GET_ADC_PARAMS, 0, NULL);
	if (ret != SR_OK)
		return ret;

	ret = ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_GET_ADC_PARAMS, data, (int)sizeof(data), &len);
	if (ret != SR_OK)
		return ret;

	if (width)
		*width = data[0];
	if (div)
		*div = data[1];

	return SR_OK;
}

/*
 * Get the ADC channel
 */
SR_PRIV int ch32h417_libusb_get_adc_channel(const struct sr_dev_inst *sdi,
								  uint8_t *channel)
{
	uint8_t data = 0;
	int len = 0;
	int ret;

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_GET_ADC_CHANNEL, 0, NULL);
	if (ret != SR_OK)
		return ret;

	ret = ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_GET_ADC_CHANNEL, &data, 1, &len);
	if (ret != SR_OK)
		return ret;

	if (channel)
		*channel = data;

	return SR_OK;
}

/*
 * Start logic acquisition
 */
SR_PRIV int ch32h417_libusb_start_logic(const struct sr_dev_inst *sdi)
{
	sr_info("Starting logic acquisition");
	return ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_START_LOGIC, 0, NULL);
}

/*
 * Stop logic acquisition
 */
SR_PRIV int ch32h417_libusb_stop_logic(const struct sr_dev_inst *sdi)
{
	sr_info("Stopping logic acquisition");
	return ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_STOP_LOGIC, 0, NULL);
}

/*
 * Start ADC acquisition
 */
SR_PRIV int ch32h417_libusb_start_adc(const struct sr_dev_inst *sdi)
{
	sr_info("Starting ADC acquisition");
	return ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_START_ADC, 0, NULL);
}

/*
 * Stop ADC acquisition
 */
SR_PRIV int ch32h417_libusb_stop_adc(const struct sr_dev_inst *sdi)
{
	sr_info("Stopping ADC acquisition");
	return ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_STOP_ADC, 0, NULL);
}

/*
 * Enter IAP mode (firmware upgrade mode)
 * Command: 0xAE + len(0)
 * The device switches to IAP mode after this command, enabling a firmware upgrade
 */
SR_PRIV int ch32h417_libusb_enter_iap(const struct sr_dev_inst *sdi)
{
	sr_info("Entering IAP mode for firmware upgrade");
	return ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_ENTER_IAP, 0, NULL);
}

/* Send the stop command and emit SR_DF_END */
static void send_stop_command(const struct sr_dev_inst *sdi)
{
	int ret = -1;
	struct dev_context *devc = sdi->priv;

	/* Send the stop command */
	if (devc->mode == MODE_LOGIC_ANALYZER)
	{
		ret = ch32h417_libusb_stop_logic(sdi);
		if (ret != SR_OK)
		{
			sr_warn("Failed to send stop logic command on finish.");
		}
	}
	else
	{
		ret = ch32h417_libusb_stop_adc(sdi);
		if (ret != SR_OK)
		{
			sr_warn("Failed to send stop ADC command on finish.");
		}
	}
	
	/* Flush the buffered upload cache */
	unsigned long pipe_num = (devc->mode == MODE_LOGIC_ANALYZER) ?
									 PIPE_LOGIC_DATA : PIPE_ADC_DATA;
	if (!ch32h417_libusb_clear_buf_upload(devc->device_index, pipe_num)) {
		sr_warn("Failed to clear upload buffer pipe %lu for device at index %d", pipe_num, devc->device_index);
	}
}

/*
 * Create the device context
 */
SR_PRIV struct dev_context *ch32h417_libusb_dev_new(void)
{
	struct dev_context *devc;

	devc = g_malloc0(sizeof(struct dev_context));
	devc->mode = MODE_LOGIC_ANALYZER;
	devc->cur_samplerate = ch32h417_libusb_logic_samplerates_usb3[0];
	devc->limit_samples = 0;
	devc->limit_frames = 1;
	devc->capture_ratio = 0;
	devc->sample_width = SAMPLE_WIDTH_16BIT;
	devc->num_channels = 16; /* 16 channels by default */
	devc->adc_width = ADC_WIDTH_8BIT;
	devc->adc_channel = ADC_CHANNEL_1;
	devc->fw_version = 0;
	devc->is_usb2 = FALSE;
	devc->samplerates = ch32h417_libusb_logic_samplerates_usb3;
	devc->num_samplerates = ARRAY_SIZE(ch32h417_libusb_logic_samplerates_usb3);

	/* USB transport initialization */
	devc->device_index = -1;
	devc->device_handle = NULL;
	devc->buffer_size = BUFFER_UPLOAD_SIZE;

	g_mutex_init(&devc->mutex);

	return devc;
}

/*
 * Detect the USB speed from the bcdUSB field of the device descriptor
 * USB device descriptor layout:
 *   offset 0:   bLength (descriptor length)
 *   offset 1:   bDescriptorType (descriptor type)
 *   offset 2-3: bcdUSB (USB version, BCD encoded)
 *   ...
 * @return TRUE for USB 2.0, FALSE for USB 3.0
 */
SR_PRIV gboolean ch32h417_libusb_detect_usb_speed(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	uint8_t descr[18];  /* A USB device descriptor is always 18 bytes */
	unsigned long length;
	uint16_t bcd_usb;

	devc = sdi->priv;

	g_usleep(50000);

	length = sizeof(descr);
	if (!ch32h417_libusb_get_device_descr(devc->device_index, descr, &length)) {
		sr_warn("Failed to get device descriptor, assuming USB 3.0");
		return FALSE;  /* Default to USB 3.0 */
	}

	if (length < 4) {
		sr_warn("Device descriptor too short (%lu bytes), assuming USB 3.0", length);
		return FALSE;
	}

	/* The bcdUSB field sits at offset 2-3, little endian */
	bcd_usb = descr[2] | (descr[3] << 8);

	sr_info("Device descriptor: bcdUSB=0x%04x", bcd_usb);

	/* USB 2.0: bcdUSB = 0x0200
	 * USB 3.0: bcdUSB = 0x0300
	 */
	if (bcd_usb >= 0x0300) {
		sr_info("USB 3.0 device detected (bcdUSB=0x%04x)", bcd_usb);
		return FALSE;
	} else {
		sr_info("USB 2.0 device detected (bcdUSB=0x%04x)", bcd_usb);
		return TRUE;
	}
}

/*
 * Get the firmware version
 * The CH32H417 has no FPGA, only a firmware version
 * Command: 0xAC + len(0)
 * Response: 0xBC + len(2) + status(0x00) + buf[0] (fw version) + buf[1] (hw version)
 */
SR_PRIV int ch32h417_libusb_get_fw_version(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	uint8_t data[2] = {0};
	int len = 0;
	int ret;
	uint8_t hw_version;

	devc = sdi->priv;

	ret = ch32h417_libusb_send_command((struct sr_dev_inst *)sdi, CMD_GET_VERSION, 0, NULL);
	if (ret != SR_OK) {
		sr_err("Failed to send get version command");
		return ret;
	}

	ret = ch32h417_libusb_receive_response((struct sr_dev_inst *)sdi, RESP_GET_VERSION, data, (int)sizeof(data), &len);
	if (ret != SR_OK) {
		sr_err("Failed to receive version response");
		return ret;
	}

	if (len >= 1) {
		/* Some firmware revisions return only one payload byte (the software
		 * version); a missing hardware version is treated as 0 instead of reading stale data. */
		hw_version = (len >= 2) ? data[1] : 0;
		devc->fw_version = ((uint16_t)data[0] << 8) | hw_version;
		sr_info("Firmware version: %d, Hardware version: %d", data[0], hw_version);
	} else {
		sr_warn("Version response too short: %d", len);
		devc->fw_version = 0;
	}

	sr_dbg("Firmware version: 0x%04x", devc->fw_version);

	return SR_OK;
}

/*
 * Read the abort flag. Several threads write it, so it has to be read
 * while holding devc->mutex.
 */
static gboolean acquisition_aborted(struct dev_context *devc)
{
	gboolean aborted;

	g_mutex_lock(&devc->mutex);
	aborted = devc->acq_aborted;
	g_mutex_unlock(&devc->mutex);

	return aborted;
}

/* Set the abort flag while holding devc->mutex. */
static void acquisition_set_aborted(struct dev_context *devc,
									gboolean aborted)
{
	g_mutex_lock(&devc->mutex);
	devc->acq_aborted = aborted;
	g_mutex_unlock(&devc->mutex);
}

/*
 * Send the stop command at most once per acquisition. Both the data read
 * thread and finish_acquisition() want to stop the device, and re-issuing
 * the command while it is already idle confuses it. The flag is reset by
 * ch32h417_libusb_acquisition_start().
 */
static void send_stop_command_once(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc = sdi->priv;
	gboolean need_send;

	g_mutex_lock(&devc->mutex);
	need_send = !devc->stop_sent;
	devc->stop_sent = TRUE;
	g_mutex_unlock(&devc->mutex);

	if (need_send)
		send_stop_command(sdi);
}

static int acquisition_check_cb(int fd, int revents, void *cb_data)
{
	struct sr_dev_inst *sdi = cb_data;
	struct dev_context *devc = sdi->priv;

	(void)fd;
	(void)revents;

	if (acquisition_aborted(devc)) {
		sr_dbg("acquisition_check_cb: thread requested stop");
		sr_dev_acquisition_stop(sdi);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

/* Send logic analyzer data */
static void la_send_data_proc(struct sr_dev_inst *sdi,
			  uint8_t *data, size_t length, size_t unitsize)
{
	const struct sr_datafeed_logic logic = {
		.length = length,
		.unitsize = unitsize,
		.data = data};

	const struct sr_datafeed_packet packet = {
		.type = SR_DF_LOGIC,
		.payload = &logic};

	sr_session_send(sdi, &packet);
}

/* Send ADC data */
static void adc_send_data_proc(struct sr_dev_inst *sdi,
							   uint8_t *data, size_t length, size_t sample_width)
{
	struct dev_context *devc;
	struct sr_datafeed_analog analog;
	struct sr_analog_encoding encoding;
	struct sr_analog_meaning meaning;
	struct sr_analog_spec spec;
	float *float_data;
	size_t num_samples;
	size_t i;
	uint16_t raw_sample;
	float voltage;

	devc = sdi->priv;

	/* Sample count: 1 byte per sample for 8-bit ADC, 2 bytes for 10-bit ADC */
	size_t sample_bytes = (devc->adc_width == ADC_WIDTH_10BIT) ? 2 : 1;
	num_samples = length / sample_bytes;

	if (num_samples == 0)
		return;

	/* Convert the raw ADC data into float voltages */
	float_data = g_try_malloc(sizeof(float) * num_samples);
	if (!float_data) {
		sr_err("Failed to allocate float buffer for ADC data");
		return;
	}

	if (devc->adc_width == ADC_WIDTH_10BIT) {
		/* 10-bit ADC: assemble the value byte by byte, little endian, to avoid unaligned access and host endianness differences */
		const float scale = 320.0f / 220.0f / 1024.0f * 3.3f;

		for (i = 0; i < num_samples; i++) {
			raw_sample = (uint16_t)data[i * 2] |
						 ((uint16_t)data[i * 2 + 1] << 8);
			voltage = (float)(raw_sample & 0x3FF) * scale;
			float_data[i] = voltage;
		}
	} else {
		/* 8-bit ADC: read a single byte */
		const float scale = 320.0f / 220.0f / 256.0f * 3.3f;

		for (i = 0; i < num_samples; i++)
			float_data[i] = (float)data[i] * scale;
	}

	sr_analog_init(&analog, &encoding, &meaning, &spec, 2);
	analog.meaning->channels = devc->enabled_analog_channels;
	analog.meaning->mq = SR_MQ_VOLTAGE;
	analog.meaning->unit = SR_UNIT_VOLT;
	analog.meaning->mqflags = 0;
	analog.num_samples = num_samples;
	analog.data = float_data;

	const struct sr_datafeed_packet packet = {
		.type = SR_DF_ANALOG,
		.payload = &analog};

	sr_session_send(sdi, &packet);

	g_free(float_data);
}

/* Data read thread */
static gpointer data_read_thread(gpointer arg)
{
	struct sr_dev_inst *sdi = arg;
	struct dev_context *devc = sdi->priv;
	unsigned long pipe_num;
	unsigned long transfer_count, total_data_len;
	unsigned long length;
	uint8_t *buffer;
	size_t unitsize;
	unsigned int num_samples;
	int trigger_offset, pre_trigger_samples, cur_sample_count, processed_samples;
	int64_t no_data_timeout_ms = 500; /* 500 ms without data counts as a timeout */

	pipe_num = (devc->mode == MODE_LOGIC_ANALYZER) ? PIPE_LOGIC_DATA : PIPE_ADC_DATA;

	if (devc->mode == MODE_LOGIC_ANALYZER)
		unitsize = (devc->num_channels == 16) ? 2 : 1;
	else
		unitsize = (devc->adc_width == ADC_WIDTH_10BIT) ? 2 : 1;

	buffer = g_try_malloc(devc->buffer_size);
	if (!buffer) {
		/*
		 * Stop the acquisition the same way a link problem does instead of
		 * just returning: acq_aborted is still FALSE at this point, so the
		 * timer callback would keep the capture running and the GUI would
		 * wait forever for data that is never going to come.
		 */
		sr_err("Failed to allocate read buffer");
		send_stop_command_once(sdi);
		acquisition_set_aborted(devc, TRUE);
		sr_dbg("data_read_thread EXIT (no buffer)");
		return NULL;
	}

	sr_dbg("Data read thread started, pipe=%lu, buffer_size=%zu",
		   pipe_num, devc->buffer_size);
	
	int64_t last_query_time = g_get_monotonic_time();
	int64_t current_time = last_query_time;

	unsigned int dbg_packets = 0;
	while (!acquisition_aborted(devc)) {
		processed_samples = 0;
		current_time = g_get_monotonic_time();

		/* Poll for buffered data */
		if (!ch32h417_libusb_query_buf_upload_ex(devc->device_index, pipe_num,
									   &transfer_count, &total_data_len)) {
			/* Query failed, wait briefly and retry */
			sr_warn("Failed to query buffer upload, retrying in 1ms");
			g_usleep(1000);
			continue;
		}

		if (transfer_count == 0) {
			/* No data: check whether the device has stopped */
			/* Track how long there has been no data */
			const int64_t no_data_time_ms =
				(current_time - last_query_time) / 1000;
			if (no_data_time_ms >= no_data_timeout_ms) {
				/*
				 * Nothing arrived although the capture was not due to end.
				 * A link that cannot sustain the configured rate is one
				 * way this shows up, so report the stop instead of letting
				 * the GUI present the truncated capture as complete.
				 */
				devc->link_stop_reason = LINK_STOP_NO_DATA;
				send_stop_command_once(sdi);
				/* Timed out with no data, the device may have stopped acquiring */
				sr_info("No data for %" PRId64 " ms, device may have stopped", no_data_timeout_ms);
				g_free(buffer);
				/* Flag the thread as exiting; the timer callback handles the stop */
				acquisition_set_aborted(devc, TRUE);
				sr_dbg("data_read_thread EXIT (timeout)");
				return NULL;
			}
			g_usleep(1000);
			continue;
		}

		/* Data received, reset the idle timer */
		last_query_time = current_time;

		/* Read the data */
		length = devc->buffer_size;
		if (!ch32h417_libusb_read_endpoint(devc->device_index, pipe_num, buffer, &length)) {
			sr_warn("Failed to read data from pipe %lu", pipe_num);
			g_usleep(1000);
			continue;
		}

		if (length == 0) {
			g_usleep(1000);
			continue;
		}

		/* A line per read is far too expensive at SuperSpeed rates. */
		if (++dbg_packets >= 256) {
			dbg_packets = 0;
			sr_dbg("Read %lu bytes from pipe %lu", length, pipe_num);
		}

		cur_sample_count = length / unitsize;

		/* Trigger handling */
	check_trigger:
		g_mutex_lock(&devc->mutex);

		if (devc->trigger_fired) {
			if (!devc->limit_samples || devc->sent_samples < devc->limit_samples) {
				/* Send the incoming transfer to the session bus. */
				num_samples = cur_sample_count - processed_samples;
				if (devc->limit_samples && devc->sent_samples + num_samples > devc->limit_samples)
					num_samples = devc->limit_samples - devc->sent_samples;

				devc->send_data_proc(sdi, (uint8_t *)buffer + processed_samples * unitsize,
					num_samples * unitsize, unitsize);
				devc->sent_samples += num_samples;
				processed_samples += num_samples;
			}
		} else {
			trigger_offset = soft_trigger_logic_check(devc->stl,
				buffer + processed_samples * unitsize,
				length - processed_samples * unitsize,
				&pre_trigger_samples);
			if (trigger_offset > -1) {
				std_session_send_df_frame_begin(sdi);
				devc->sent_samples += pre_trigger_samples;
				num_samples = cur_sample_count - processed_samples - trigger_offset;
				if (devc->limit_samples &&
						devc->sent_samples + num_samples > devc->limit_samples)
					num_samples = devc->limit_samples - devc->sent_samples;

				devc->send_data_proc(sdi, (uint8_t *)buffer
						+ processed_samples * unitsize
						+ trigger_offset * unitsize,
						num_samples * unitsize, unitsize);
				devc->sent_samples += num_samples;
				processed_samples += trigger_offset + num_samples;

				devc->trigger_fired = TRUE;
			}
		}
		g_mutex_unlock(&devc->mutex);

		/* Enforce the sample limit */
		const int frame_ended = devc->limit_samples && (devc->sent_samples >= devc->limit_samples);
		const int final_frame = devc->limit_frames && (devc->num_frames >= (devc->limit_frames - 1));

		if (frame_ended) {
			devc->num_frames++;
			devc->sent_samples = 0;
			devc->trigger_fired = FALSE;
			std_session_send_df_frame_end(sdi);

			/* There may be another trigger in the remaining data, go back and check for it */
			if (processed_samples < cur_sample_count) {
				/* Reset the trigger stage */
				if (devc->stl)
					devc->stl->cur_stage = 0;
				else {
					std_session_send_df_frame_begin(sdi);
					devc->trigger_fired = TRUE;
				}
				if (!final_frame)
					goto check_trigger;
			}
		}
		if (frame_ended && final_frame) {
			send_stop_command_once(sdi);

			acquisition_set_aborted(devc, TRUE);
			g_free(buffer);
			sr_dbg("data_read_thread EXIT (frame limit)");
			return NULL;
		}

	}

	g_free(buffer);
	sr_dbg("Data read thread exiting");

	return NULL;
}

/* Finish acquisition */
static void finish_acquisition(struct sr_dev_inst *sdi)
{
	struct dev_context *devc;

	devc = sdi->priv;

	/* Send the stop command and the end packet */
	send_stop_command_once(sdi);

	/* Remove the timer callback. It only exists when the acquisition was
	 * started successfully, and so does the DF header sent below. */
	if (devc->acq_started)
		sr_session_source_remove(sdi->session, -1);

	/* Stop the read thread */
	acquisition_set_aborted(devc, TRUE);

	/* Wait for the thread to finish */
	if (devc->read_thread) {
		g_thread_join(devc->read_thread);
		devc->read_thread = NULL;
	}
	
	if (devc->acq_started) {
		std_session_send_df_end(sdi);
		devc->acq_started = FALSE;
	}

	sr_dbg("devc->read_thread EXIT");

	if (devc->stl)
	{
		soft_trigger_logic_free(devc->stl);
		devc->stl = NULL;
	}

	sr_dbg("finish_acquisition function done");
}

/*
 * Stop the data read thread and wait for it to be gone. Used on teardown:
 * the thread uses both the device context and the device instance, so
 * neither may be freed while it is still running. Safe when there is no
 * thread, and safe after finish_acquisition(), which clears the pointer.
 */
SR_PRIV void ch32h417_libusb_join_read_thread(struct sr_dev_inst *sdi)
{
	struct dev_context *devc;

	if (!sdi)
		return;

	devc = sdi->priv;
	if (!devc || !devc->read_thread)
		return;

	acquisition_set_aborted(devc, TRUE);
	g_thread_join(devc->read_thread);
	devc->read_thread = NULL;
}

static void ch32h417_libusb_flush_cmd_endpoint(unsigned long index)
{
	uint8_t buffer[PACKET_SIZE_IN];
	unsigned long io_length;

	/* Set the timeout to 0 ms */
	if (!ch32h417_libusb_set_timeout_ex(index, 1, 1, 1, 1)) {
		sr_warn("Failed to set timeout ex for device at index %lu", index);
	}
	g_usleep(1000);

	uint8_t retry_count = 0;
	while(1)
	{
		io_length = PACKET_SIZE_IN;
		if(!ch32h417_libusb_read_endpoint(index, PIPE_CMD, buffer, &io_length))
			break;
		else{
			if(io_length == 0)
				break;
			sr_warn("Device has data not read, ignore it. Data: %02x", buffer[0]);
		}
		retry_count++;
		if(retry_count > 10)
		{
			sr_err("Timeout while flushing command endpoint for device at index %lu", index);
			break;
		}
		g_usleep(1000);
	}

	/* Restore the timeout */
	if (!ch32h417_libusb_set_timeout_ex(index, USB_TIMEOUT, USB_TIMEOUT, USB_TIMEOUT, USB_TIMEOUT)) {
		sr_warn("Failed to set timeout ex for device at index %lu", index);
	}

	g_usleep(1000);
}

/*
 * Start acquisition
 */
SR_PRIV int ch32h417_libusb_acquisition_start(const struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	struct sr_trigger *trigger;
	unsigned long pipe_num;
	int ret;

	devc = sdi->priv;
	devc->sent_samples = 0;
	acquisition_set_aborted(devc, FALSE);
	devc->link_stop_reason = LINK_STOP_NONE;
	devc->num_frames = 0;
	devc->acq_started = FALSE;

	/* Flush endpoint 0 */
	sr_info("Flushing cmd endpoint...");
	ch32h417_libusb_flush_cmd_endpoint(devc->device_index);

	/* Enable buffered upload */
	if (!ch32h417_libusb_set_buf_upload_ex(devc->device_index, 0, PIPE_LOGIC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_LOGIC_DATA, devc->device_index);
	}
	if (!ch32h417_libusb_set_buf_upload_ex(devc->device_index, 0, PIPE_ADC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_ADC_DATA, devc->device_index);
	}

	if (!ch32h417_libusb_set_buf_upload_ex(devc->device_index, 1, PIPE_LOGIC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_LOGIC_DATA, devc->device_index);
	}
	if (!ch32h417_libusb_set_buf_upload_ex(devc->device_index, 1, PIPE_ADC_DATA, devc->buffer_size)) {
		sr_warn("Failed to set upload buffer pipe %d for device at index %d", PIPE_ADC_DATA, devc->device_index);
	}

	/* Configure the channels */
	if (devc->mode == MODE_LOGIC_ANALYZER)
	{
		pipe_num = PIPE_LOGIC_DATA;
		/* Send a stop command first to make sure the device is idle */
		ret = ch32h417_libusb_stop_logic(sdi);
		if (ret != SR_OK)
		{
			sr_warn("Pre-stop logic command failed, continuing anyway...");
		}

		/* Set the logic analyzer parameters */
		ret = ch32h417_libusb_set_logic_params(sdi);
		if (ret != SR_OK)
		{
			sr_err("Failed to set logic params.");
			return ret;
		}

		/* Verification test command */
		// {
		// 	uint8_t width, div, level;
		// 	int test_failed = 0;

		// 	/* Test A4: get the logic analyzer parameters */
		// 	ret = ch32h417_libusb_get_logic_params(sdi, &width, &div);
		// 	if (ret == SR_OK) {
		// 		sr_info("Verify A4: width=0x%02x, div=%d (expect width=0x%02x)",
		// 			width, div, devc->sample_width);
		// 		if (width != devc->sample_width) {
		// 			sr_warn("A4 verification: width mismatch!");
		// 			test_failed = 1;
		// 		}
		// 	} else {
		// 		sr_err("A4 command failed!");
		// 		test_failed = 1;
		// 	}
		// }

		/* Start logic acquisition */
		ret = ch32h417_libusb_start_logic(sdi);
		if (ret != SR_OK)
		{
			sr_err("Failed to start logic acquisition.");
			return ret;
		}
	}
	else
	{
		pipe_num = PIPE_ADC_DATA;

		sr_dbg("ADC mode: initializing enabled_analog_channels");
		sr_dbg("  sdi->channels = %p", sdi->channels);

		if (devc->enabled_analog_channels)
			g_slist_free(devc->enabled_analog_channels);
		devc->enabled_analog_channels = NULL;

		int analog_count = 0;
		for (GSList *l = sdi->channels; l; l = l->next) {
			struct sr_channel *ch = l->data;
			sr_dbg("  channel %p: type=%d, enabled=%d", ch, ch->type, ch->enabled);
			if (ch->type == SR_CHANNEL_ANALOG && ch->enabled) {
				devc->enabled_analog_channels = g_slist_append(
					devc->enabled_analog_channels, ch);
				analog_count++;
			}
		}
		sr_dbg("  enabled_analog_channels count = %d", analog_count);

		ret = ch32h417_libusb_stop_adc(sdi);
		if (ret != SR_OK)
		{
			sr_warn("Pre-stop ADC command failed, continuing anyway...");
		}

		ret = ch32h417_libusb_set_adc_params(sdi);
		if (ret != SR_OK)
		{
			sr_err("Failed to set ADC params.");
			return ret;
		}

		ret = ch32h417_libusb_set_adc_channel(sdi);
		if (ret != SR_OK)
		{
			sr_err("Failed to set ADC channel.");
			return ret;
		}

		/* Start ADC acquisition */
		ret = ch32h417_libusb_start_adc(sdi);
		if (ret != SR_OK)
		{
			sr_err("Failed to start ADC acquisition.");
			return ret;
		}
	}

	/* Configure the trigger */
	if ((trigger = sr_session_trigger_get(sdi->session)))
	{
		int pre_trigger_samples = 0;
		if (devc->limit_samples > 0)
			pre_trigger_samples = (devc->capture_ratio * devc->limit_samples) / 100;
		devc->stl = soft_trigger_logic_new(sdi, trigger, pre_trigger_samples);
		if (!devc->stl)
			return SR_ERR_MALLOC;
		devc->trigger_fired = FALSE;
	}
	else
	{
		devc->trigger_fired = TRUE;
	}

	if (devc->mode == MODE_LOGIC_ANALYZER)
	{
		devc->send_data_proc = la_send_data_proc;
	}
	else
	{
		devc->send_data_proc = adc_send_data_proc;
	}

	acquisition_set_aborted(devc, TRUE);

	if (devc->read_thread) {
		g_thread_join(devc->read_thread);
		devc->read_thread = NULL;
	}

	acquisition_set_aborted(devc, FALSE);

	/* A new acquisition may send the stop command again */
	g_mutex_lock(&devc->mutex);
	devc->stop_sent = FALSE;
	g_mutex_unlock(&devc->mutex);

	if (!ch32h417_libusb_clear_buf_upload(devc->device_index, pipe_num)) {
		sr_warn("Failed to clear upload buffer pipe %lu for device at index %d", pipe_num, devc->device_index);
	}

	devc->read_thread = g_thread_new("ch32h417-read", data_read_thread, (void *)sdi);
	if (!devc->read_thread) {
		sr_err("Failed to create read thread");
		if (!ch32h417_libusb_clear_buf_upload(devc->device_index, pipe_num)) {
			sr_warn("Failed to clear upload buffer pipe %lu for device at index %d", pipe_num, devc->device_index);
		}
		/*
		 * Give back what was set up for the acquisition and mark it as
		 * over: there is no thread to read anything, and leaving the flag
		 * alone would keep the timer callback waiting for one.
		 */
		acquisition_set_aborted(devc, TRUE);
		if (devc->stl) {
			soft_trigger_logic_free(devc->stl);
			devc->stl = NULL;
		}
		send_stop_command_once(sdi);
		return SR_ERR;
	}

	std_session_send_df_header(sdi);
	devc->acq_started = TRUE;

	/* Add a timer callback that watches the acquisition state and stops it on the main thread */
	sr_session_source_add(sdi->session, -1, 0, 5,
		acquisition_check_cb, (void *)sdi);

	return SR_OK;
}

/*
 * Stop acquisition
 */
SR_PRIV int ch32h417_libusb_acquisition_stop(const struct sr_dev_inst *sdi)
{
	finish_acquisition((struct sr_dev_inst *)sdi);
	return SR_OK;
}
