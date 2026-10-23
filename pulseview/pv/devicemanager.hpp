/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2013 Joel Holdsworth <joel@airwebreathe.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PULSEVIEW_PV_DEVICEMANAGER_HPP
#define PULSEVIEW_PV_DEVICEMANAGER_HPP

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QString>

using std::list;
using std::map;
using std::set;
using std::shared_ptr;
using std::string;
using std::vector;

namespace Glib {
class VariantBase;
}

namespace sigrok {
class ConfigKey;
class Context;
class Driver;
}

using sigrok::ConfigKey;

namespace pv {

namespace devices {
class Device;
class HardwareDevice;
}

class Session;

class DeviceManager
{
public:
	DeviceManager(shared_ptr<sigrok::Context> context,
		std::string driver, bool do_scan);

	~DeviceManager() = default;

	const shared_ptr<sigrok::Context>& context() const;

	shared_ptr<sigrok::Context> context();

	const list< shared_ptr<devices::HardwareDevice> >& devices() const;
	shared_ptr<devices::HardwareDevice> user_spec_device() const;

	bool driver_supported(shared_ptr<sigrok::Driver> driver) const;

	list< shared_ptr<devices::HardwareDevice> > driver_scan(
		shared_ptr<sigrok::Driver> driver,
		map<const sigrok::ConfigKey *, Glib::VariantBase> drvopts);

	const map<string, string> get_device_info(
		const shared_ptr<devices::Device> device);

	const shared_ptr<devices::HardwareDevice> find_device_from_info(
		const map<string, string> search_info);

	/*
	 * The board a saved page was on, or nothing when it is not here. A
	 * WeAct board is named by its serial number; any other board of that
	 * vendor may report one serial for all of them, so its port is used.
	 */
	const shared_ptr<devices::HardwareDevice> find_saved_device(
		const map<string, string> &info);

	/*
	 * A driver that does not answer a scan in time is left alone from then
	 * on: the call that is stuck still holds it, so asking again would
	 * either queue behind it or hang in the same place, and every later
	 * device pass would stall again. This is process wide for the same
	 * reason the problem is - the driver is.
	 */
	static bool scan_is_stalled(const std::string &driver_name);

	/* The drivers that hit the timeout since the last call, taken off the
	 * list so the caller can tell the user about each one once. */
	static std::vector<std::string> take_stalled_drivers();

	/*
	 * Whether the board a device identity names sits on the bus right now, in
	 * any state: an FX2 that was just plugged is there while it is being
	 * flashed, it just does not answer yet.
	 */
	static bool board_on_bus(const QString &identity);

	/*
	 * Where that board stands: absent, in its boot loader with no firmware to
	 * answer with, or running. A blank board has to be flashed before anything
	 * can be waited for; one that says it runs and stays silent is hung.
	 * A vendor board runs on its own back end and has no FX2 version to ask
	 * for, so it is told apart.
	 */
	enum class UsbBoardState { Absent, Boot, Running, RunningVendor };
	static UsbBoardState board_state(const QString &identity);

	/*
	 * Whether the board at this identity was seen answering after the given
	 * moment. An arrival older than that is one the board already came back
	 * from, which is how a board pulled right after being plugged is told
	 * from one that is on its way back.
	 */
	static bool board_up_since(const QString &identity, qint64 since_ms);

private:
	/* The drivers that ran into the timeout, and the ones not reported */
	static std::set<std::string> stalled_drivers_;
	static std::vector<std::string> stalled_reports_;

	bool compare_devices(shared_ptr<devices::Device> a,
		shared_ptr<devices::Device> b);

	static map<const ConfigKey *, Glib::VariantBase>
	drive_scan_options(vector<string> user_spec,
		set<const ConfigKey *> driver_opts);

protected:
	shared_ptr<sigrok::Context> context_;
	list< shared_ptr<devices::HardwareDevice> > devices_;
	shared_ptr<devices::HardwareDevice> user_spec_device_;
};

} // namespace pv

#endif // PULSEVIEW_PV_DEVICEMANAGER_HPP
