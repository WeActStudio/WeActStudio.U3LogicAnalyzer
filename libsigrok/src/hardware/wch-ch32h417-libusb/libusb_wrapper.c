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
 * libusb transport wrapper implementation, the counterpart of ch375_wrapper.c.
 *
 * The device needs nothing but bulk transfers, so the whole transport is
 * ordinary libusb work with one exception: emulating the "buffered upload"
 * service the CH375 kernel driver provided. That driver kept transfers in
 * flight behind our back and let the read thread poll how much data had
 * accumulated; here the same effect comes from a ring of asynchronous bulk
 * transfers, modelled on libsigrok's fx2lafw driver:
 *
 *   - enabling the stream allocates a ring of transfers on the IN endpoint,
 *   - each completion stages its payload and immediately resubmits,
 *   - the read thread's existing poll drains one staged buffer per read.
 *
 * libusb delivers completions through callbacks that only run while someone
 * handles events. The read loop polls every millisecond, so query_buf_upload_ex
 * pumps the event loop instead of spawning a thread of our own.
 */

#include <config.h>
#include "libusb_wrapper.h"
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"
#include <glib.h>

#include <libusb.h>
#include <stdlib.h>
#include <string.h>

#define LOG_PREFIX "ch32h417-libusb"

/*
 * The ring geometry is expressed in SuperSpeed bursts: a chunk that is not a
 * whole number of bursts would be cut short by the first partially filled
 * burst, and the ring would never run at the depth it was sized for.
 */
G_STATIC_ASSERT(LIBUSB_STREAM_RING_CHUNK % LIBUSB_STREAM_BURST == 0);

/* ============================================================================
 * Internal state
 * ============================================================================ */

/* One slot of the transfer ring, holding USB target and staged payload. */
struct stream_slot {
	struct libusb_transfer *transfer;
	unsigned char *target;     /* Buffer libusb writes into */
	unsigned char *stage;      /* Payload waiting to be drained */
	size_t capacity;
	size_t stage_len;
	size_t stage_off;
	int inflight;
};

/* A sample stream: one IN endpoint with its transfer ring. */
struct usb_stream {
	struct stream_slot slots[LIBUSB_STREAM_RING_COUNT];
	unsigned long pipe_num;
	unsigned int depth;
	gboolean enabled;
	unsigned long pending_bytes;
	unsigned long pending_buffers;
	unsigned long overflow;
};

struct usb_device {
	libusb_device *device;
	libusb_device_handle *handle;
	uint8_t bus;
	uint8_t address;
	unsigned long index;
	char name[256];
	unsigned long usb_id;
	uint16_t bcd_usb;
	unsigned long timeout_write;
	unsigned long timeout_read;
	struct usb_stream stream[2];    /* index 0: logic (pipe 2), 1: ADC (pipe 3) */
};

/*
 * Reference count on libusb_init(). The wrapper is entered from the scan path,
 * from dev_open()/dev_close() and from the GUI thread, so the library has to
 * stay initialised while any device handle is alive.
 */
G_LOCK_DEFINE_STATIC(libusb_state);
static gint libusb_refcount = 0;

/* Devices found by the last scan_devices(), addressed like the CH375 indices. */
static struct usb_device *device_table[LIBUSB_MAX_NUMBER];
static unsigned int device_count = 0;

/* ============================================================================
 * Helpers
 * ============================================================================ */

/* Map a pipe number onto the ring that serves it. */
static struct usb_stream *stream_for_pipe(struct usb_device *du, unsigned long pipe_num)
{
	if (pipe_num == 2)
		return &du->stream[0];
	if (pipe_num == 3)
		return &du->stream[1];
	return NULL;
}

static uint8_t pipe_to_out_ep(unsigned long pipe_num)
{
	return (uint8_t)(pipe_num & 0x7f);
}

static uint8_t pipe_to_in_ep(unsigned long pipe_num)
{
	return (uint8_t)((pipe_num & 0x7f) | 0x80);
}

static struct usb_device *device_by_index(unsigned long index)
{
	if (index >= (unsigned long)device_count)
		return NULL;
	return device_table[index];
}

/*
 * Build the device identifier the shared scan code parses. It expects a Windows
 * device path whose third component is the serial number, so synthesise one
 * from the descriptor string.
 */
static void build_device_name(struct usb_device *du, const char *serial)
{
	unsigned long vid = du->usb_id & 0xffff;
	unsigned long pid = (du->usb_id >> 16) & 0xffff;

	/*
	 * The trailing '#' is what terminates the instance id. api.c locates the
	 * serial number by scanning for a third '#', so dropping it here would
	 * make every device look like it had no serial number at all.
	 */
	g_snprintf(du->name, sizeof(du->name), "\\\\?\\usb#vid_%04lx&pid_%04lx#%s#",
			   vid, pid, serial && serial[0] ? serial : "000000000000");
}

/* ============================================================================
 * Asynchronous stream ring
 * ============================================================================ */

static void LIBUSB_CALL stream_transfer_cb(struct libusb_transfer *transfer)
{
	struct usb_stream *stream = transfer->user_data;
	struct stream_slot *slot = NULL;
	unsigned int i;

	if (!stream)
		return;

	for (i = 0; i < stream->depth; i++) {
		if (stream->slots[i].transfer == transfer) {
			slot = &stream->slots[i];
			break;
		}
	}

	if (!slot)
		return;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED && transfer->actual_length > 0) {
		if (slot->stage_len == 0) {
			size_t len = (size_t)transfer->actual_length;
			if (len > slot->capacity)
				len = slot->capacity;
			memcpy(slot->stage, slot->target, len);
			slot->stage_len = len;
			slot->stage_off = 0;
			stream->pending_bytes += len;
			stream->pending_buffers++;
		} else {
			/*
			 * The read thread has not drained the previous payload yet. The
			 * ring is deep enough that this should not happen in practice;
			 * count it rather than blocking in the callback.
			 */
			stream->overflow++;
			if (stream->overflow == 1 || stream->overflow % 100 == 0)
				sr_warn("Stream overflow on pipe %lu, %lu payload(s) dropped",
						stream->pipe_num, stream->overflow);
		}
	} else if (transfer->status != LIBUSB_TRANSFER_COMPLETED &&
			   transfer->status != LIBUSB_TRANSFER_CANCELLED &&
			   transfer->status != LIBUSB_TRANSFER_TIMED_OUT) {
		/*
		 * A timeout is not worth reporting: the ring stays submitted between
		 * acquisitions, so an idle device simply times every transfer out and
		 * gets it resubmitted. Logging those would bury everything else.
		 */
		sr_dbg("Stream transfer on pipe %lu ended with status %s",
			   stream->pipe_num, libusb_error_name(transfer->status));
	}

	if (stream->enabled) {
		if (libusb_submit_transfer(transfer) == LIBUSB_SUCCESS)
			return;
		sr_warn("Failed to resubmit stream transfer on pipe %lu", stream->pipe_num);
	}

	slot->inflight = 0;
}

static void pump_events(void);

static int stream_has_inflight(const struct usb_stream *stream)
{
	unsigned int i;

	for (i = 0; i < LIBUSB_STREAM_RING_COUNT; i++)
		if (stream->slots[i].inflight)
			return TRUE;

	return FALSE;
}

/*
 * Run completion callbacks until nothing is in flight any more. Bounded so a
 * device that never answers cannot wedge the caller for good.
 */
static void stream_quiesce(struct usb_stream *stream)
{
	/* Comfortably longer than the per-transfer timeout, so even a stalled
	 * endpoint gets the chance to land before we give up on it. */
	int64_t deadline = g_get_monotonic_time() + 600 * 1000;

	while (stream_has_inflight(stream)) {
		if (g_get_monotonic_time() >= deadline) {
			sr_warn("Stream on pipe %lu is still busy after cancellation", stream->pipe_num);
			return;
		}
		pump_events();
	}
}

static int stream_start(struct usb_device *du, struct usb_stream *stream, unsigned long pipe_num)
{
	unsigned int i;
	uint8_t ep = pipe_to_in_ep(pipe_num);
	unsigned int timeout = du->timeout_read > 0 ? (unsigned int)du->timeout_read : 500;

	if (stream->enabled)
		return LIBUSB_TRUE;

	/* Never reuse a transfer that is still in flight. */
	stream_quiesce(stream);

	for (i = 0; i < LIBUSB_STREAM_RING_COUNT; i++) {
		struct stream_slot *slot = &stream->slots[i];

		if (!slot->transfer) {
			slot->target = g_try_malloc(LIBUSB_STREAM_RING_CHUNK);
			slot->stage = g_try_malloc(LIBUSB_STREAM_RING_CHUNK);
			if (!slot->target || !slot->stage) {
				sr_err("Failed to allocate stream buffers");
				g_free(slot->target);
				g_free(slot->stage);
				slot->target = slot->stage = NULL;
				return LIBUSB_FALSE;
			}

			slot->transfer = libusb_alloc_transfer(0);
			if (!slot->transfer) {
				sr_err("Failed to allocate stream transfer");
				g_free(slot->target);
				g_free(slot->stage);
				slot->target = slot->stage = NULL;
				return LIBUSB_FALSE;
			}
		}

		/*
		 * Re-fill and reset every slot, not just the freshly allocated ones:
		 * a stop/start cycle leaves the staging area holding the previous
		 * payload, and the pipe number may have changed since the ring was
		 * built.
		 */
		slot->capacity = LIBUSB_STREAM_RING_CHUNK;
		slot->stage_len = 0;
		slot->stage_off = 0;

		/* The ring owner is what the callback needs, not the device. */
		libusb_fill_bulk_transfer(slot->transfer, du->handle, ep,
								  slot->target, (int)slot->capacity,
								  stream_transfer_cb, stream, timeout);
	}

	stream->pipe_num = pipe_num;
	stream->depth = LIBUSB_STREAM_RING_COUNT;
	stream->enabled = TRUE;
	stream->pending_bytes = 0;
	stream->pending_buffers = 0;
	stream->overflow = 0;

	for (i = 0; i < LIBUSB_STREAM_RING_COUNT; i++) {
		int ret = libusb_submit_transfer(stream->slots[i].transfer);
		if (ret != LIBUSB_SUCCESS) {
			sr_warn("Failed to submit stream transfer %u: %s", i, libusb_error_name(ret));
			continue;
		}
		stream->slots[i].inflight = 1;
	}

	sr_dbg("Stream ring started on pipe %lu: %u x %d bytes",
		   pipe_num, LIBUSB_STREAM_RING_COUNT, LIBUSB_STREAM_RING_CHUNK);

	return LIBUSB_TRUE;
}

static void stream_stop(struct usb_device *du, struct usb_stream *stream)
{
	unsigned int i;

	(void)du;

	if (!stream->enabled)
		return;

	stream->enabled = FALSE;

	/*
	 * Only ask libusb to complete these early; the callback must not resubmit
	 * afterwards, and tearing the ring down here would let it touch freed
	 * memory. stream_free() does that once everything has landed.
	 */
	for (i = 0; i < stream->depth; i++)
		if (stream->slots[i].transfer && stream->slots[i].inflight)
			libusb_cancel_transfer(stream->slots[i].transfer);

	/*
	 * Cancellation is a request, not an action: the transfers stay in flight
	 * until their callbacks run. The driver disables and re-enables buffered
	 * upload back to back at the start of every acquisition, so waiting here
	 * is what keeps the next start from hitting LIBUSB_ERROR_BUSY.
	 */
	stream_quiesce(stream);

	sr_dbg("Stream ring stopped on pipe %lu", stream->pipe_num);
}

static void stream_free(struct usb_stream *stream)
{
	unsigned int i;

	for (i = 0; i < LIBUSB_STREAM_RING_COUNT; i++) {
		struct stream_slot *slot = &stream->slots[i];

		if (slot->inflight) {
			/*
			 * Still outstanding after stream_stop() gave up on it. Freeing
			 * would hand libusb a dangling transfer, so drop the references
			 * and leak instead: a wedge on one endpoint is survivable, a
			 * callback into freed memory is not. stream_start() allocates
			 * fresh for a slot whose transfer is NULL.
			 */
			sr_warn("Stream transfer on pipe %lu never completed, leaking it",
					stream->pipe_num);
			slot->transfer = NULL;
			slot->target = slot->stage = NULL;
			slot->capacity = 0;
			slot->stage_len = 0;
			slot->stage_off = 0;
			slot->inflight = 0;
			continue;
		}

		if (slot->transfer) {
			libusb_free_transfer(slot->transfer);
			slot->transfer = NULL;
		}
		g_free(slot->target);
		g_free(slot->stage);
		slot->target = slot->stage = NULL;
		slot->capacity = 0;
		slot->stage_len = 0;
		slot->stage_off = 0;
		slot->inflight = 0;
	}

	stream->depth = 0;
	stream->enabled = FALSE;
	stream->pending_bytes = 0;
	stream->pending_buffers = 0;
}

/*
 * Move staged payloads into the caller's buffer. Everything that has already
 * landed is copied in one pass: the read thread asks for BUFFER_UPLOAD_SIZE
 * at a time, so handing back a single ring chunk per call would cap it at
 * one chunk per iteration - fine at high speed, far too slow for SuperSpeed.
 */
static int stream_drain(struct usb_stream *stream, void *buffer, unsigned long *length)
{
	unsigned int i;
	unsigned long want = *length;
	unsigned long done = 0;
	uint8_t *out = buffer;

	for (i = 0; i < LIBUSB_STREAM_RING_COUNT && done < want; i++) {
		struct stream_slot *slot = &stream->slots[i];
		size_t space, copied, remain;

		if (slot->stage_len == 0)
			continue;

		space = (size_t)(want - done);
		remain = slot->stage_len - slot->stage_off;
		copied = remain < space ? remain : space;

		memcpy(out + done, slot->stage + slot->stage_off, copied);
		slot->stage_off += copied;
		done += (unsigned long)copied;

		if (slot->stage_off >= slot->stage_len) {
			stream->pending_buffers--;
			stream->pending_bytes -= slot->stage_len;
			slot->stage_len = 0;
			slot->stage_off = 0;
		}
	}

	*length = done;
	return done > 0 ? LIBUSB_TRUE : LIBUSB_FALSE;
}

/*
 * Give libusb a chance to run completion callbacks. The read loop polls about
 * once per millisecond, which is far more often than needed; the small timeout
 * keeps this cheap when nothing has arrived.
 */
static void pump_events(void)
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	libusb_handle_events_timeout(NULL, &tv);
}

/* ============================================================================
 * Library lifecycle
 * ============================================================================ */

int ch32h417_libusb_init(void)
{
	G_LOCK(libusb_state);

	if (libusb_refcount == 0) {
		int ret = libusb_init(NULL);
		if (ret != LIBUSB_SUCCESS) {
			sr_err("Failed to initialise libusb: %s", libusb_error_name(ret));
			G_UNLOCK(libusb_state);
			return SR_ERR;
		}
		sr_info("libusb initialised");
	}

	libusb_refcount++;
	G_UNLOCK(libusb_state);

	return SR_OK;
}

void ch32h417_libusb_cleanup(void)
{
	G_LOCK(libusb_state);

	if (libusb_refcount == 0) {
		G_UNLOCK(libusb_state);
		return;
	}

	libusb_refcount--;

	if (libusb_refcount == 0) {
		libusb_exit(NULL);
		sr_info("libusb released");
	}

	G_UNLOCK(libusb_state);
}

int ch32h417_libusb_is_loaded(void)
{
	return (libusb_refcount > 0);
}

/* ============================================================================
 * Enumeration
 * ============================================================================ */

int ch32h417_libusb_scan_devices(void)
{
	libusb_device **list = NULL;
	ssize_t count, i;
	unsigned int found = 0;

	G_LOCK(libusb_state);

	/*
	 * Drop the previous table. Each struct owns its transfer rings, so the
	 * rings have to go quiet before the struct does: an in-flight transfer
	 * carries a pointer to the ring as user_data and would otherwise complete
	 * into a callback that dereferences freed memory. Re-enumerating while a
	 * ring is still running (unplug, or a rescan after the firmware upgrade
	 * dialog) is exactly how that used to happen.
	 */
	for (i = 0; i < LIBUSB_MAX_NUMBER; i++) {
		struct usb_device *du = device_table[i];
		unsigned int k;

		if (!du)
			continue;

		for (k = 0; k < 2; k++) {
			stream_stop(du, &du->stream[k]);
			stream_free(&du->stream[k]);
		}

		if (du->handle)
			libusb_close(du->handle);
		if (du->device)
			libusb_unref_device(du->device);
		g_free(du);
		device_table[i] = NULL;
	}
	device_count = 0;

	if (libusb_refcount == 0) {
		G_UNLOCK(libusb_state);
		return 0;
	}

	count = libusb_get_device_list(NULL, &list);
	if (count < 0) {
		sr_err("Failed to enumerate USB devices: %s", libusb_error_name((int)count));
		G_UNLOCK(libusb_state);
		return 0;
	}

	for (i = 0; i < count && found < LIBUSB_MAX_NUMBER; i++) {
		struct libusb_device_descriptor des;
		struct usb_device *du;
		char serial[64] = {0};
		libusb_device_handle *tmp = NULL;
		int k;

		if (libusb_get_device_descriptor(list[i], &des) != LIBUSB_SUCCESS)
			continue;
		if (des.idVendor != 0x1a86 || des.idProduct != 0x5537)
			continue;

		/* The serial number needs an open handle, so open it briefly here. */
		if (libusb_open(list[i], &tmp) == LIBUSB_SUCCESS && tmp) {
			libusb_get_string_descriptor_ascii(tmp, des.iSerialNumber,
											   (unsigned char *)serial, sizeof(serial) - 1);
			libusb_close(tmp);
		}
		for (k = 0; serial[k]; k++)
			serial[k] = (char)tolower((unsigned char)serial[k]);

		du = g_malloc0(sizeof(struct usb_device));
		du->device = list[i];
		libusb_ref_device(list[i]);
		du->bus = libusb_get_bus_number(list[i]);
		du->address = libusb_get_device_address(list[i]);
		du->index = found;
		du->usb_id = ((unsigned long)des.idProduct << 16) | des.idVendor;
		du->bcd_usb = des.bcdUSB;
		du->timeout_write = 500;
		du->timeout_read = 500;
		build_device_name(du, serial);

		sr_dbg("Found CH32H417 at bus %u address %u, serial %s",
			   du->bus, du->address, serial[0] ? serial : "(none)");

		device_table[found] = du;
		found++;
	}

	device_count = found;
	libusb_free_device_list(list, 1);

	G_UNLOCK(libusb_state);

	sr_info("Found %u CH32H417 device(s)", found);

	return (int)found;
}

/* ============================================================================
 * Device management
 * ============================================================================ */

void *ch32h417_libusb_open_device(unsigned long index)
{
	struct usb_device *du;
	int ret;

	du = device_by_index(index);
	if (!du)
		return LIBUSB_INVALID_HANDLE;

	if (du->handle) {
		sr_dbg("Device at index %lu is already open", index);
		return du->handle;
	}

	ret = libusb_open(du->device, &du->handle);
	if (ret != LIBUSB_SUCCESS) {
		sr_dbg("Failed to open device at index %lu: %s", index, libusb_error_name(ret));
		du->handle = NULL;
		return LIBUSB_INVALID_HANDLE;
	}

	/*
	 * Detach a kernel driver if one claimed the interface. On Windows WinUSB
	 * is already userspace so this is a no-op; on Linux it is what lets us
	 * talk to the device without unbinding by hand.
	 */
	if (libusb_has_capability(LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER)) {
		if (libusb_kernel_driver_active(du->handle, 0) == 1) {
			ret = libusb_detach_kernel_driver(du->handle, 0);
			if (ret < 0)
				sr_warn("Failed to detach kernel driver at index %lu: %s",
						index, libusb_error_name(ret));
		}
	}

	ret = libusb_claim_interface(du->handle, 0);
	if (ret != LIBUSB_SUCCESS) {
		sr_err("Failed to claim interface 0 of device at index %lu: %s",
			   index, libusb_error_name(ret));
		libusb_close(du->handle);
		du->handle = NULL;
		return LIBUSB_INVALID_HANDLE;
	}

	sr_dbg("Opened device at index %lu", index);
	return du->handle;
}

void ch32h417_libusb_close_device(unsigned long index)
{
	struct usb_device *du = device_by_index(index);
	unsigned int i;

	if (!du || !du->handle)
		return;

	for (i = 0; i < 2; i++) {
		stream_stop(du, &du->stream[i]);
		stream_free(&du->stream[i]);
	}

	libusb_release_interface(du->handle, 0);
	libusb_close(du->handle);
	du->handle = NULL;

	sr_dbg("Closed device at index %lu", index);
}

unsigned long ch32h417_libusb_get_usb_id(unsigned long index)
{
	struct usb_device *du = device_by_index(index);

	return du ? du->usb_id : 0;
}

const char *ch32h417_libusb_get_device_name(unsigned long index)
{
	struct usb_device *du = device_by_index(index);

	return du ? du->name : NULL;
}

/*
 * Return the standard device descriptor in its raw USB layout, because the
 * caller reads bcdUSB from offset 2-3 to tell USB 2.0 from USB 3.0.
 */
int ch32h417_libusb_get_device_descr(unsigned long index, void *buffer, unsigned long *length)
{
	struct usb_device *du = device_by_index(index);
	struct libusb_device_descriptor des;
	uint8_t *out = buffer;

	if (!du || !du->device || !buffer || !length)
		return LIBUSB_FALSE;
	if (libusb_get_device_descriptor(du->device, &des) != LIBUSB_SUCCESS) {
		sr_warn("Failed to get device descriptor at index %lu", index);
		return LIBUSB_FALSE;
	}
	if (*length < 18) {
		*length = 0;
		return LIBUSB_FALSE;
	}

	out[0] = des.bLength;
	out[1] = des.bDescriptorType;
	out[2] = (uint8_t)(des.bcdUSB & 0xff);
	out[3] = (uint8_t)(des.bcdUSB >> 8);
	out[4] = des.bDeviceClass;
	out[5] = des.bDeviceSubClass;
	out[6] = des.bDeviceProtocol;
	out[7] = des.bMaxPacketSize0;
	out[8] = (uint8_t)(des.idVendor & 0xff);
	out[9] = (uint8_t)(des.idVendor >> 8);
	out[10] = (uint8_t)(des.idProduct & 0xff);
	out[11] = (uint8_t)(des.idProduct >> 8);
	out[12] = (uint8_t)(des.bcdDevice & 0xff);
	out[13] = (uint8_t)(des.bcdDevice >> 8);
	out[14] = des.iManufacturer;
	out[15] = des.iProduct;
	out[16] = des.iSerialNumber;
	out[17] = des.bNumConfigurations;

	*length = 18;
	du->bcd_usb = des.bcdUSB;

	return LIBUSB_TRUE;
}

/* ============================================================================
 * Timeouts
 * ============================================================================ */

int ch32h417_libusb_set_timeout_ex(unsigned long index, unsigned long write_timeout,
								  unsigned long read_timeout, unsigned long aux_timeout,
								  unsigned long inter_timeout)
{
	struct usb_device *du = device_by_index(index);

	(void)aux_timeout;
	(void)inter_timeout;

	if (!du)
		return LIBUSB_FALSE;

	/* Guard against a zero timeout, which libusb reads as "wait forever". */
	du->timeout_write = write_timeout ? write_timeout : 1;
	du->timeout_read = read_timeout ? read_timeout : 1;

	return LIBUSB_TRUE;
}

/* ============================================================================
 * Endpoint transfers
 * ============================================================================ */

int ch32h417_libusb_write_endpoint(unsigned long index, unsigned long pipe_num,
								  void *buffer, unsigned long *length)
{
	struct usb_device *du = device_by_index(index);
	int transferred = 0;
	int ret;

	if (!du || !du->handle || !buffer || !length)
		return LIBUSB_FALSE;

	ret = libusb_bulk_transfer(du->handle, pipe_to_out_ep(pipe_num),
							   buffer, (int)*length, &transferred,
							   (unsigned int)du->timeout_write);
	if (ret != LIBUSB_SUCCESS) {
		sr_dbg("Bulk write to pipe %lu failed: %s", pipe_num, libusb_error_name(ret));
		*length = 0;
		return LIBUSB_FALSE;
	}

	*length = (unsigned long)transferred;
	return LIBUSB_TRUE;
}

int ch32h417_libusb_read_endpoint(unsigned long index, unsigned long pipe_num,
								 void *buffer, unsigned long *length)
{
	struct usb_device *du = device_by_index(index);
	struct usb_stream *stream;
	int transferred = 0;
	int ret;

	if (!du || !du->handle || !buffer || !length)
		return LIBUSB_FALSE;

	stream = stream_for_pipe(du, pipe_num);

	/* Sample streams run through the ring once the ring is enabled. */
	if (stream && stream->enabled) {
		pump_events();
		return stream_drain(stream, buffer, length);
	}

	ret = libusb_bulk_transfer(du->handle, pipe_to_in_ep(pipe_num),
							   buffer, (int)*length, &transferred,
							   (unsigned int)du->timeout_read);
	if (ret == LIBUSB_ERROR_TIMEOUT) {
		/* No data is not an error: the command channel polls this way. */
		*length = 0;
		return LIBUSB_TRUE;
	}
	if (ret != LIBUSB_SUCCESS) {
		sr_dbg("Bulk read from pipe %lu failed: %s", pipe_num, libusb_error_name(ret));
		*length = 0;
		return LIBUSB_FALSE;
	}

	*length = (unsigned long)transferred;
	return LIBUSB_TRUE;
}

/* ============================================================================
 * Endpoint control
 * ============================================================================ */

int ch32h417_libusb_abort_endpoint_read(unsigned long index, unsigned long pipe_num)
{
	struct usb_device *du = device_by_index(index);
	struct usb_stream *stream;

	if (!du || !du->handle)
		return LIBUSB_FALSE;

	stream = stream_for_pipe(du, pipe_num);
	if (stream)
		stream_stop(du, stream);

	return LIBUSB_TRUE;
}

int ch32h417_libusb_reset_in_endpoint(unsigned long index, unsigned long pipe_num)
{
	struct usb_device *du = device_by_index(index);

	if (!du || !du->handle)
		return LIBUSB_FALSE;
	if (libusb_clear_halt(du->handle, pipe_to_in_ep(pipe_num)) != LIBUSB_SUCCESS) {
		sr_dbg("Failed to clear halt on pipe %lu", pipe_num);
		return LIBUSB_FALSE;
	}

	return LIBUSB_TRUE;
}

int ch32h417_libusb_reset_out_endpoint(unsigned long index, unsigned long pipe_num)
{
	struct usb_device *du = device_by_index(index);

	if (!du || !du->handle)
		return LIBUSB_FALSE;
	if (libusb_clear_halt(du->handle, pipe_to_out_ep(pipe_num)) != LIBUSB_SUCCESS) {
		sr_dbg("Failed to clear halt on pipe %lu", pipe_num);
		return LIBUSB_FALSE;
	}

	return LIBUSB_TRUE;
}

/* ============================================================================
 * Buffered upload emulation
 * ============================================================================ */

int ch32h417_libusb_set_buf_upload_ex(unsigned long index, unsigned long enable,
									  unsigned long pipe_num, unsigned long transfer_size)
{
	struct usb_device *du = device_by_index(index);
	struct usb_stream *stream;

	(void)transfer_size;

	if (!du || !du->handle)
		return LIBUSB_FALSE;

	stream = stream_for_pipe(du, pipe_num);
	if (!stream)
		/* The command channel has no stream; report success as CH375 does. */
		return LIBUSB_TRUE;

	return enable ? stream_start(du, stream, pipe_num)
				  : (stream_stop(du, stream), LIBUSB_TRUE);
}

int ch32h417_libusb_query_buf_upload_ex(unsigned long index, unsigned long pipe_num,
										unsigned long *transfer_count, unsigned long *total_len)
{
	struct usb_device *du = device_by_index(index);
	struct usb_stream *stream;

	if (!du || !du->handle || !transfer_count || !total_len)
		return LIBUSB_FALSE;

	stream = stream_for_pipe(du, pipe_num);
	if (!stream) {
		*transfer_count = 0;
		*total_len = 0;
		return LIBUSB_TRUE;
	}

	/* Let the callbacks run so freshly arrived payloads are visible. */
	pump_events();

	*transfer_count = stream->pending_buffers;
	*total_len = stream->pending_bytes;

	return LIBUSB_TRUE;
}

int ch32h417_libusb_clear_buf_upload(unsigned long index, unsigned long pipe_num)
{
	struct usb_device *du = device_by_index(index);
	struct usb_stream *stream;
	unsigned int i;

	if (!du || !du->handle)
		return LIBUSB_FALSE;

	stream = stream_for_pipe(du, pipe_num);
	if (!stream)
		return LIBUSB_TRUE;

	/* Drop whatever completed; transfers already in flight keep running. */
	for (i = 0; i < LIBUSB_STREAM_RING_COUNT; i++) {
		stream->slots[i].stage_len = 0;
		stream->slots[i].stage_off = 0;
	}
	stream->pending_bytes = 0;
	stream->pending_buffers = 0;

	sr_dbg("Cleared upload buffer for pipe %lu", pipe_num);

	return LIBUSB_TRUE;
}
