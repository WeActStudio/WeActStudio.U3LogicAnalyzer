/*
 * What the front end talks to, and what the bus watcher hands back.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pv/usbapi.hpp>

#include <mutex>
#include <map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace pv {

namespace {

/* Defined at the end of the file: fills the table once, and says whether it
 * is usable. */
bool fill_api(UsbApi *api);

/* What each board answered the last time it was asked, by port chain: an
 * open board cannot be opened a second time, so the answer is kept. */
std::mutex versions_mutex;
std::map<std::string, std::string> known_versions;
} // namespace

/*
 * The bus number is not required: it only tells two boards on the same port of
 * different buses apart, and a libusb without it still names them by chain.
 * Making it a must would have turned a missing optional call into no bus
 * watching at all.
 */
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

bool UsbApi::can_probe() const
{
	return ready() && open && close && control_transfer;
}

namespace {

std::mutex events_mutex;
std::vector<UsbBusEvent> events;

/* The chain of ports a device sits in, without the bus: the port numbers
 * joined by '.', empty when the device cannot tell. A page identity spells
 * the same place as "<bus>-<chain>". */
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

/* The chain itself: "usb/4-2.2" and "4-2.2" both name "2.2". The bus number
 * is left out on purpose - two contexts in one process do not always number
 * buses alike, and the chain is what tells one board from another. */
std::string chain_only(const std::string &path)
{
	std::string port = path;

	if (port.compare(0, 4, "usb/") == 0)
		port = port.substr(4);

	const size_t dash = port.find('-');
	return dash == std::string::npos ? port : port.substr(dash + 1);
}

/* Every fx2 board on the bus that answers, by port chain. A board the
 * drivers have open refuses to be opened, and is simply not in the map. */
std::map<std::string, std::string> fx2_answers()
{
	std::map<std::string, std::string> answers;

	const UsbApi *const api = usb_api();
	if (!api || !api->can_probe())
		return answers;

	libusb_context *ctx = nullptr;
	if (api->init(&ctx) != 0)
		return answers;

	libusb_device **list = nullptr;
	const ssize_t count = api->get_device_list(ctx, &list);

	for (ssize_t i = 0; i < count; i++)
	{
		struct libusb_device_descriptor des;

		if (api->get_device_descriptor(list[i], &des) != 0)
			continue;

		/* The sigrok firmware ids: a blank board that kept them in its
		 * EEPROM is on the bus under these too, and answers nothing. */
		if (des.idVendor != 0x1d50 ||
			(des.idProduct != 0x608c && des.idProduct != 0x608d))
			continue;

		libusb_device_handle *handle = nullptr;

		if (api->open(list[i], &handle) != 0)
			continue;

		/* The request the driver makes when it opens the board; a board
		 * that has no firmware does not answer it. */
		unsigned char reply[2] = { 0, 0 };
		const int taken = api->control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xb0,
			0x0000, 0x0000, reply, sizeof(reply), 500);

		if (taken == (int)sizeof(reply))
			answers[chain_only(chain_of(list[i]))] =
				std::to_string(reply[0]) + "." + std::to_string(reply[1]);

		api->close(handle);
	}

	if (list)
		api->free_device_list(list, 1);

	api->exit(ctx);
	return answers;
}

void remember_answers(const std::map<std::string, std::string> &answers)
{
	if (answers.empty())
		return;

	std::lock_guard<std::mutex> lock(versions_mutex);
	for (const auto &entry : answers)
		known_versions[entry.first] = entry.second;
}

std::string remembered_answer(const std::string &port)
{
	std::lock_guard<std::mutex> lock(versions_mutex);
	const auto hit = known_versions.find(port);
	return hit == known_versions.end() ? std::string() : hit->second;
}

} // namespace

std::vector<UsbBusEvent> usb_events_take()
{
	std::lock_guard<std::mutex> lock(events_mutex);
	std::vector<UsbBusEvent> taken;
	taken.swap(events);
	return taken;
}

std::string fx2_cached_version(const std::string &port_path)
{
	return remembered_answer(chain_only(port_path));
}

void fx2_prefetch_versions()
{
	remember_answers(fx2_answers());
}

int LIBUSB_CALL on_usb_bus_change(libusb_context *, libusb_device *dev,
	libusb_hotplug_event event, void *)
{
	UsbBusEvent entry;
	entry.event = (int)event;
	entry.vid = 0;
	entry.bus = 0;
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

		/* Same window, same reason: a board that is already gone may not
		 * say which bus it was on. */
		if (api && api->get_bus_number)
			entry.bus = api->get_bus_number(dev);
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
	api->get_bus_number =
		(decltype(api->get_bus_number))sym("libusb_get_bus_number");
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
	api->open = (decltype(api->open))sym("libusb_open");
	api->close = (decltype(api->close))sym("libusb_close");
	api->control_transfer = (decltype(api->control_transfer))
		sym("libusb_control_transfer");
#else
	api->init = libusb_init;
	api->exit = libusb_exit;
	api->get_device_list = libusb_get_device_list;
	api->free_device_list = libusb_free_device_list;
	api->get_device_descriptor = libusb_get_device_descriptor;
	api->get_bus_number = libusb_get_bus_number;
	api->get_port_numbers = libusb_get_port_numbers;
	api->has_capability = libusb_has_capability;
	api->hotplug_register = libusb_hotplug_register_callback;
	api->hotplug_deregister = libusb_hotplug_deregister_callback;
	api->handle_events_timeout = libusb_handle_events_timeout;
	api->open = libusb_open;
	api->close = libusb_close;
	api->control_transfer = libusb_control_transfer;
#endif

	return api->ready();
}

} // namespace

} // namespace pv
