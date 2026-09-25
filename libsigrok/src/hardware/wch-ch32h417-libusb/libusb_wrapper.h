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

/*
 * libusb transport wrapper for the CH32H417 logic analyzer.
 *
 * This is the counterpart of the CH375 driver's ch375_wrapper.h. The device
 * exposes a single interface with three bulk endpoint pairs and no
 * vendor-specific control requests, so plain bulk transfers are enough:
 *
 *   EP 0x01 / 0x81   command channel, 64 bytes out / 32 bytes in per packet
 *   EP 0x02 / 0x82   logic sample stream
 *   EP 0x03 / 0x83   ADC sample stream
 *
 * The pipe numbers used by the protocol layer are exactly the endpoint
 * numbers, so both backends address the same endpoints and the USB protocol
 * itself is unchanged.
 *
 * Functions are prefixed ch32h417_libusb_ rather than a bare libusb_ because
 * libusb 1.0 already exports libusb_init, libusb_open, libusb_bulk_transfer
 * and friends; reusing the prefix would redeclare library symbols.
 */

#ifndef LIBSIGROK_HARDWARE_WCH_CH32H417_LIBUSB_WRAPPER_H
#define LIBSIGROK_HARDWARE_WCH_CH32H417_LIBUSB_WRAPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Command packet lengths, matching the firmware's PACKET_SIZE_OUT / _IN */
#define LIBUSB_PACKET_LENGTH        64

/* Endpoint addresses: the CH375 pipe number is the endpoint number */
#define LIBUSB_ENDP_CMD_DOWN        0x01    /* Command channel (OUT) */
#define LIBUSB_ENDP_CMD_UP          0x81    /* Command response (IN) */
#define LIBUSB_ENDP_DATA_DOWN       0x02    /* Logic sample stream (OUT) */
#define LIBUSB_ENDP_DATA_UP         0x82    /* Logic sample stream (IN) */
#define LIBUSB_ENDP_ADC_DOWN        0x03    /* ADC sample stream (OUT) */
#define LIBUSB_ENDP_ADC_UP          0x83    /* ADC sample stream (IN) */

/* Maximum number of devices the scan loop probes */
#define LIBUSB_MAX_NUMBER           16

/* Sentinel returned when a device cannot be opened */
#define LIBUSB_INVALID_HANDLE       ((void *)-1)

/* Return values */
#define LIBUSB_TRUE                 1
#define LIBUSB_FALSE                0

/* Default buffered upload size (1 MB), matching BUFFER_UPLOAD_SIZE */
#define LIBUSB_DEFAULT_TRANSFER_SIZE    (1024 * 1024)

/*
 * Depth and granularity of the userspace stream ring, sized for the fastest
 * link the device can negotiate (USB 3.0 SuperSpeed). A SuperSpeed bulk
 * endpoint has wMaxPacketSize 1024 and bMaxBurst 0x0f, so one burst is 16 kB;
 * keeping every transfer a whole number of bursts stops a partially filled
 * burst from ending the transfer early. The same geometry works at high
 * speed, where the packet size is only 512 bytes.
 *
 * The CH375 kernel driver could hold an entire transferSize (1 MB) of queued
 * data on our behalf; with libusb only what we explicitly submit can arrive,
 * so the ring keeps roughly 20 ms of a saturated SuperSpeed link in flight,
 * which is what gives the read loop slack between two polls.
 */
#define LIBUSB_STREAM_BURST         (16 * 1024)
#define LIBUSB_STREAM_RING_CHUNK    (16 * LIBUSB_STREAM_BURST)  /* 256 kB */
#define LIBUSB_STREAM_RING_COUNT    32                          /* 8 MB */

/*
 * Watchdog for the asynchronous ring. If no new transfer has been accepted for
 * this long the stream is considered stalled and the ring is restarted, which
 * recovers from a device-side hiccup that would otherwise leave the ring
 * permanently below full depth.
 */
#define LIBUSB_STREAM_WATCHDOG_MS   1000

/* ============================================================================
 * Wrapper API declarations
 * ============================================================================ */

/* Library lifecycle, reference counted so late cleanup cannot strand a handle */
int ch32h417_libusb_init(void);
void ch32h417_libusb_cleanup(void);
int ch32h417_libusb_is_loaded(void);

/* Device enumeration and management */
int ch32h417_libusb_scan_devices(void);
void *ch32h417_libusb_open_device(unsigned long index);
void ch32h417_libusb_close_device(unsigned long index);
unsigned long ch32h417_libusb_get_usb_id(unsigned long index);
const char *ch32h417_libusb_get_device_name(unsigned long index);
int ch32h417_libusb_get_device_descr(unsigned long index, void *buffer, unsigned long *length);

/* Timeouts in milliseconds; a value of 0 is clamped so it cannot block forever */
int ch32h417_libusb_set_timeout_ex(unsigned long index, unsigned long write_timeout,
								  unsigned long read_timeout, unsigned long aux_timeout,
								  unsigned long inter_timeout);

/* Endpoint read/write; pipe_num is the endpoint number (1, 2 or 3) */
int ch32h417_libusb_write_endpoint(unsigned long index, unsigned long pipe_num,
								  void *buffer, unsigned long *length);
int ch32h417_libusb_read_endpoint(unsigned long index, unsigned long pipe_num,
								 void *buffer, unsigned long *length);

/* Endpoint control */
int ch32h417_libusb_abort_endpoint_read(unsigned long index, unsigned long pipe_num);
int ch32h417_libusb_reset_in_endpoint(unsigned long index, unsigned long pipe_num);
int ch32h417_libusb_reset_out_endpoint(unsigned long index, unsigned long pipe_num);

/*
 * Buffered upload emulation. On CH375 these hand the work to the kernel
 * driver; here they manage the userspace ring of asynchronous bulk transfers.
 * Query reports how many completed buffers are pending and their total length,
 * and read drains one buffer at a time.
 */
int ch32h417_libusb_set_buf_upload_ex(unsigned long index, unsigned long enable,
									  unsigned long pipe_num, unsigned long transfer_size);
int ch32h417_libusb_query_buf_upload_ex(unsigned long index, unsigned long pipe_num,
										unsigned long *transfer_count, unsigned long *total_len);
int ch32h417_libusb_clear_buf_upload(unsigned long index, unsigned long pipe_num);

/*
 * There is deliberately no set_buf_download_ex() or set_io_mode() counterpart.
 * Both are CH375DLL-side features: buffered download buffers OUT transfers in
 * the driver, and set_io_mode() is a private CH375WDM.sys IOCTL selecting how
 * the kernel buffered upload behaves. libusb drives every transfer from
 * userspace and has neither knob.
 */

#ifdef __cplusplus
}
#endif

#endif /* LIBSIGROK_HARDWARE_WCH_CH32H417_LIBUSB_WRAPPER_H */
