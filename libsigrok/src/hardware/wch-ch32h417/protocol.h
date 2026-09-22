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

#ifndef LIBSIGROK_HARDWARE_WCH_CH32H417_PROTOCOL_H
#define LIBSIGROK_HARDWARE_WCH_CH32H417_PROTOCOL_H

#include <glib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"
#include "ch375_wrapper.h"

#define LOG_PREFIX "wch-ch32h417"

#define USB_VENDOR_ID       0x1a86
#define USB_PRODUCT_ID      0x5537

/* USB configuration */
#define USB_INTERFACE       0
#define USB_CONFIGURATION   1
#define USB_TIMEOUT         500


#define PIPE_CMD            1       /* Command */
#define PIPE_LOGIC_DATA     2       /* Logic analyzer data */
#define PIPE_ADC_DATA       3       /* ADC data */

#define PACKET_SIZE_OUT         64      /* Host-to-device command packet, fixed 64 bytes */
#define PACKET_SIZE_IN          32      /* Device-to-host command packet, fixed 32 bytes */

/* Transfer parameters */
#define NUM_SIMUL_TRANSFERS     64
#define MAX_EMPTY_TRANSFERS     (NUM_SIMUL_TRANSFERS * 2)

/* Buffered upload transfer size (1 MB) */
#define BUFFER_UPLOAD_SIZE      (1024 * 1024)

/* Logic analyzer commands */
#define CMD_SET_LOGIC_PARAMS    0xA0
#define CMD_SET_LOGIC_LEVEL     0xA1
#define CMD_START_LOGIC         0xA2
#define CMD_STOP_LOGIC          0xA3
#define CMD_GET_LOGIC_PARAMS    0xA4
#define CMD_GET_LOGIC_LEVEL     0xA5

/* ADC commands */
#define CMD_SET_ADC_PARAMS      0xA6
#define CMD_SET_ADC_CHANNEL     0xA7
#define CMD_START_ADC           0xA8
#define CMD_STOP_ADC            0xA9
#define CMD_GET_ADC_PARAMS      0xAA
#define CMD_GET_ADC_CHANNEL     0xAB

/* Miscellaneous commands */
#define CMD_GET_VERSION         0xAC
#define CMD_ENTER_IAP           0xAE

/* Response commands */
#define RESP_SET_LOGIC_PARAMS   0xB0
#define RESP_SET_LOGIC_LEVEL    0xB1
#define RESP_GET_LOGIC_PARAMS   0xB4
#define RESP_GET_LOGIC_LEVEL    0xB5
#define RESP_SET_ADC_PARAMS     0xB6
#define RESP_SET_ADC_CHANNEL    0xB7
#define RESP_GET_ADC_PARAMS     0xBA
#define RESP_GET_ADC_CHANNEL    0xBB
#define RESP_GET_VERSION        0xBC

/* Sample width */
#define SAMPLE_WIDTH_8BIT       0x08
#define SAMPLE_WIDTH_16BIT      0x10

/* ADC width */
#define ADC_WIDTH_8BIT          0x08
#define ADC_WIDTH_10BIT         0x0A

/* Device capability flags */
#define DEV_CAPS_16BIT_LOGIC    (1 << 0)
#define DEV_CAPS_ADC            (1 << 1)
#define DEV_CAPS_ADC_10BIT      (1 << 2)

/* Operating mode */
enum ch32h417_mode {
	MODE_LOGIC_ANALYZER,
	MODE_ADC,
};

/* ADC channels */
enum adc_channel {
	ADC_CHANNEL_1 = 0,
	ADC_CHANNEL_2 = 1,
};

/* Device profile */
struct ch32h417_profile {
	uint16_t vid;
	uint16_t pid;
	const char *vendor;
	const char *model;
	const char *SN_start;
	uint32_t dev_caps;
};

/* Device context */
struct dev_context {
	const struct ch32h417_profile *profile;

	/* Operating mode */
	enum ch32h417_mode mode;

	/* Sample rates */
	const uint64_t *samplerates;
	int num_samplerates;
	uint64_t cur_samplerate;

	/* Logic analyzer configuration */
	uint64_t limit_samples;
	uint64_t limit_frames;
	uint64_t capture_ratio;
	uint64_t num_frames;
	uint64_t sent_samples;
	uint8_t sample_width;      /* 8bit or 16bit */
	int32_t num_channels;      /* 8 or 16 channels */
	uint16_t threshold_value_1;  /* Raw DAC threshold value (0-1024) */

	/* ADC configuration */
	uint8_t adc_width;         /* 8bit or 10bit */
	enum adc_channel adc_channel;
	GSList *enabled_analog_channels;

	/* Trigger */
	gboolean trigger_fired;
	gboolean acq_aborted;
	gboolean acq_started;     /* Acquisition fully started: the session source and the DF header exist */
	gboolean stop_sent;       /* Stop command already sent for this acquisition */
	struct soft_trigger_logic *stl;

	/* Firmware version */
	uint16_t fw_version;

	/* USB speed */
	gboolean is_usb2;

	/* CH375 device */
	int device_index;          /* Device index */
	void *device_handle;       /* Device handle */
	
	/* Data read thread */
	GThread *read_thread;      /* Read thread */
	GMutex mutex;              /* Mutex */

	/* Data read buffer */
	size_t buffer_size;

	void (*send_data_proc)(struct sr_dev_inst *sdi,
		uint8_t *data, size_t length, size_t sample_width);
};

/*
 * Sample rate tables, defined in protocol.c. The index of an entry is
 * the clock divider index that is sent to the device, so entries may
 * only ever be appended at the end; inserting a rate in the middle
 * would shift the divider of every entry behind it. The declared sizes
 * are checked against the definitions by compile-time assertions in
 * protocol.c.
 */
SR_PRIV extern const uint64_t logic_samplerates_usb3[25];
SR_PRIV extern const uint64_t logic_samplerates_usb2_16ch[6];
SR_PRIV extern const uint64_t logic_samplerates_usb2_8ch[8];
SR_PRIV extern const uint64_t adc_samplerates[5];

/* Protocol function declarations */
SR_PRIV int ch32h417_send_command(struct sr_dev_inst *sdi,
	uint8_t cmd, uint8_t len, const uint8_t *data);
SR_PRIV int ch32h417_receive_response(struct sr_dev_inst *sdi,
	uint8_t expected_cmd, uint8_t *data, int data_size, int *len);

SR_PRIV int ch32h417_set_logic_params(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_set_threshold_value(const struct sr_dev_inst *sdi, uint16_t value);
SR_PRIV int ch32h417_get_logic_params(const struct sr_dev_inst *sdi,
	uint8_t *width, uint8_t *div);
SR_PRIV int ch32h417_get_logic_level(const struct sr_dev_inst *sdi,
	uint8_t *level);

SR_PRIV int ch32h417_set_adc_params(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_set_adc_channel(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_get_adc_params(const struct sr_dev_inst *sdi,
	uint8_t *width, uint8_t *div);
SR_PRIV int ch32h417_get_adc_channel(const struct sr_dev_inst *sdi,
	uint8_t *channel);

SR_PRIV int ch32h417_start_logic(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_stop_logic(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_start_adc(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_stop_adc(const struct sr_dev_inst *sdi);

SR_PRIV struct dev_context *ch32h417_dev_new(void);
SR_PRIV gboolean ch32h417_detect_usb_speed(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_get_fw_version(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_acquisition_start(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_acquisition_stop(const struct sr_dev_inst *sdi);
SR_PRIV int ch32h417_enter_iap(const struct sr_dev_inst *sdi);

#endif /* LIBSIGROK_HARDWARE_WCH_CH32H417_PROTOCOL_H */
