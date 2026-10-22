/*
 * What the front end talks to, and what the bus watcher hands back.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pv/usbapi.hpp>

#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

namespace pv {

namespace {

/* Defined at the end of the file: fills the table once, and says whether it
 * is usable. */
bool fill_api(UsbApi *api);

} // namespace

bool UsbApi::ready() const
{
	return init && exit && get_device_list && free_device_list &&
		get_device_descriptor && get_port_numbers;
}

bool UsbApi::can_watch() const
{
	return ready() && has_capability && hotplug_register &&
		handle_events_timeout;
}

namespace {

std::mutex events_mutex;
std::vector<UsbBusEvent> events;

/* The chain of ports a device sits in, the way a device identity spells it. */
std::string chain_of(libusb_device *dev)
{
	const UsbApi *const api = usb_api();

	if (!dev || !api || !api->get_port_numbers)
		return std::string();

	uint8_t ports[8];
	const int depth = api->get_port_numbers(dev, ports, 8);

	if (depth <= 0)
		return std::string();

	std::string chain;
	for (int i = 0; i < depth; i++)
	{
		if (i)
			chain += '.';
		chain += std::to_string(ports[i]);
	}

	return chain;
}

} // namespace

std::vector<UsbBusEvent> usb_events_take()
{
	std::lock_guard<std::mutex> lock(events_mutex);
	std::vector<UsbBusEvent> taken;
	taken.swap(events);
	return taken;
}

int LIBUSB_CALL on_usb_bus_change(libusb_context *, libusb_device *dev,
	libusb_hotplug_event event, void *)
{
	UsbBusEvent entry;
	entry.event = (int)event;
	entry.vid = 0;
	entry.chain = chain_of(dev);

	/* The vendor is what tells a board of the vendor back end from an FX2. */
	{
		struct libusb_device_descriptor des;

		const UsbApi *const api = usb_api();

		/* A removed device still answers for its address, so this is only
		 * used when it can be read at all. */
		if (api && api->get_device_descriptor &&
			api->get_device_descriptor(dev, &des) == 0)
			entry.vid = des.idVendor;
	}

	{
		std::lock_guard<std::mutex> lock(events_mutex);
		events.push_back(entry);
	}

	return 0;
}

const UsbApi *usb_api()
{
	static UsbApi api;
	static std::once_flag resolved;
	static bool usable = false;

	/*
	 * Resolved once, from whichever thread asks first: the watcher and the
	 * HID enumeration both get here, and a half-filled table must never be
	 * handed out.
	 */
	std::call_once(resolved, []() {
		usable = fill_api(&api);
	});

	return usable ? &api : nullptr;
}

namespace {

/* Fills the table, and says whether it is usable at all. */
bool fill_api(UsbApi *api)
{
#ifdef _WIN32
	/* The dll the drivers load as well: one implementation, one context. */
	HMODULE module = LoadLibraryA("libusb-1.0.dll");

	if (!module)
		return false;

	auto sym = [module](const char *name) {
		return (void *)GetProcAddress(module, name);
	};

	api->init = (decltype(api->init))sym("libusb_init");
	api->exit = (decltype(api->exit))sym("libusb_exit");
	api->get_device_list =
		(decltype(api->get_device_list))sym("libusb_get_device_list");
	api->free_device_list =
		(decltype(api->free_device_list))sym("libusb_free_device_list");
	api->get_device_descriptor = (decltype(api->get_device_descriptor))
		sym("libusb_get_device_descriptor");
	api->get_port_numbers =
		(decltype(api->get_port_numbers))sym("libusb_get_port_numbers");
	api->has_capability =
		(decltype(api->has_capability))sym("libusb_has_capability");
	api->hotplug_register = (decltype(api->hotplug_register))
		sym("libusb_hotplug_register_callback");
	api->hotplug_deregister = (decltype(api->hotplug_deregister))
		sym("libusb_hotplug_deregister_callback");
	api->handle_events_timeout = (decltype(api->handle_events_timeout))
		sym("libusb_handle_events_timeout");
#else
	api->init = libusb_init;
	api->exit = libusb_exit;
	api->get_device_list = libusb_get_device_list;
	api->free_device_list = libusb_free_device_list;
	api->get_device_descriptor = libusb_get_device_descriptor;
	api->get_port_numbers = libusb_get_port_numbers;
	api->has_capability = libusb_has_capability;
	api->hotplug_register = libusb_hotplug_register_callback;
	api->hotplug_deregister = libusb_hotplug_deregister_callback;
	api->handle_events_timeout = libusb_handle_events_timeout;
#endif

	return api->ready();
}

} // namespace

} // namespace pv
