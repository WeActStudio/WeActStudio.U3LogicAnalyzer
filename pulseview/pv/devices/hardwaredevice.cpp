/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2015 Joel Holdsworth <joel@airwebreathe.org.uk>
 * Copyright (C) 2026 Q2H2
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

#include <boost/algorithm/string/join.hpp>

#include <QString>

#include <libsigrokcxx/libsigrokcxx.hpp>

#include <pv/devicemanager.hpp>

#include "hardwaredevice.hpp"

using std::shared_ptr;
using std::static_pointer_cast;
using std::string;
using std::vector;

using boost::algorithm::join;

using sigrok::HardwareDevice;

namespace pv {
namespace devices {

HardwareDevice::HardwareDevice(const shared_ptr<sigrok::Context> &context,
	shared_ptr<sigrok::HardwareDevice> device) :
	context_(context),
	device_open_(false)
{
	device_ = device;
}

HardwareDevice::~HardwareDevice()
{
	close();
}

string HardwareDevice::full_name() const
{
	vector<string> parts = {};
	if (device_->vendor().length() > 0)
		parts.push_back(device_->vendor());
	if (device_->model().length() > 0)
		parts.push_back(device_->model());
	if (device_->version().length() > 0)
		parts.push_back(device_->version());
	if (device_->serial_number().length() > 0)
		parts.push_back("[S/N: " + device_->serial_number() + "]");
	if (device_->connection_id().length() > 0)
		parts.push_back("(" + device_->connection_id() + ")");
	return join(parts, " ");
}

shared_ptr<sigrok::HardwareDevice> HardwareDevice::hardware_device() const
{
	return static_pointer_cast<sigrok::HardwareDevice>(device_);
}

string HardwareDevice::display_name(
	const DeviceManager &device_manager) const
{
	(void)device_manager;

	vector<string> parts = {};
	if (device_->vendor().length() > 0)
		parts.push_back(device_->vendor());
	else
		parts.push_back("Demo device");

	/*
	 * The name is the model plus the port the board sits in. The
	 * connection id is that port, and it is also what tells two boards of
	 * the same model apart - a serial number can be missing, copied
	 * between boards, or repeated by firmware that writes the same one
	 * everywhere. Shown for a single board too, so its port is named.
	 */
	if (device_->connection_id().length() > 0)
		parts.push_back(device_->connection_id());
	else if (device_->serial_number().length() > 0)
		parts.push_back("[" + device_->serial_number() + "]");

	return join(parts, " ");
}

void HardwareDevice::open()
{
	if (device_open_)
		close();

	try {
		device_->open();
	} catch (const sigrok::Error &e) {
		throw QString(e.what());
	}

	device_open_ = true;

	// Set up the session
	session_ = context_->create_session();
	session_->add_device(device_);
}

void HardwareDevice::close()
{
	if (device_open_)
		device_->close();

	if (session_)
		session_->remove_devices();

	device_open_ = false;
}

} // namespace devices
} // namespace pv
