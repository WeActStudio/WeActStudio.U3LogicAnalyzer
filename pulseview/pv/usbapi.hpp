/*
 * The libusb the front end talks to, and the bus events it reports.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PULSEVIEW_PV_USBAPI_HPP
#define PULSEVIEW_PV_USBAPI_HPP

#include <string>
#include <vector>

#include <libusb.h>

namespace pv {

/*
 * The libusb entry points the front end uses: the bus watcher, and the HID
 * enumeration behind the firmware-update action.
 *
 * On Windows every call goes through libusb-1.0.dll - the copy the drivers are
 * linked against - so the process holds one libusb and a context made here is
 * the one the drivers use. On Linux the linked library has hot-plug built in
 * and the table simply points at it.
 */
struct UsbApi
{
	int (LIBUSB_CALL *init)(libusb_context **);
	void (LIBUSB_CALL *exit)(libusb_context *);
	ssize_t (LIBUSB_CALL *get_device_list)(libusb_context *,
		libusb_device ***);
	void (LIBUSB_CALL *free_device_list)(libusb_device **, int);
	int (LIBUSB_CALL *get_device_descriptor)(libusb_device *,
		struct libusb_device_descriptor *);
	int (LIBUSB_CALL *get_port_numbers)(libusb_device *, uint8_t *, int);
	int (LIBUSB_CALL *has_capability)(enum libusb_capability);
	int (LIBUSB_CALL *hotplug_register)(libusb_context *, int, int, int, int,
		int, libusb_hotplug_callback_fn, void *,
		libusb_hotplug_callback_handle *);
	int (LIBUSB_CALL *hotplug_deregister)(libusb_context *,
		libusb_hotplug_callback_handle);
	int (LIBUSB_CALL *handle_events_timeout)(libusb_context *,
		struct timeval *);

	/* Whether the calls needed to walk the bus are all there. */
	bool ready() const;

	/* Whether hot-plug events can be asked for as well. */
	bool can_watch() const;
};

/*
 * The table to use, or nullptr when no usable libusb could be reached. The
 * answer is worked out once and kept.
 */
const UsbApi *usb_api();

/*
 * One bus change as libusb reports it. The port chain is read from the device
 * the event carries: it is still valid for a board that was removed, and it is
 * how a page is found without looking at the bus again.
 */
struct UsbBusEvent
{
	int event;              /* LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED / _LEFT */
	unsigned short vid;
	std::string chain;      /* "2.2", or empty when the device cannot tell */
};

/* The events libusb reported since the last call. */
std::vector<UsbBusEvent> usb_events_take();

/* The callback registered for every watched vendor/product pair. */
int LIBUSB_CALL on_usb_bus_change(libusb_context *ctx, libusb_device *dev,
	libusb_hotplug_event event, void *user_data);

} // namespace pv

#endif
