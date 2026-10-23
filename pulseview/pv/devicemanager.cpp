/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2013 Joel Holdsworth <joel@airwebreathe.org.uk>
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

#include "devicemanager.hpp"
#include "session.hpp"

#include <cassert>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <libsigrokcxx/libsigrokcxx.hpp>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QProgressDialog>
#include <QThread>

#include <boost/filesystem.hpp>

#include <pv/devices/hardwaredevice.hpp>
#include <pv/usbapi.hpp>
#include <pv/util.hpp>

using std::bind;
using std::list;
using std::map;
using std::placeholders::_1;
using std::placeholders::_2;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

using Glib::VariantBase;

using sigrok::ConfigKey;

/*
 * How long a driver is given to answer a scan. A scan of a healthy link
 * takes a fraction of a second, so this is far beyond it and only a link
 * that has stopped answering reaches it.
 */
static const int ScanTimeoutMs = 5000;

using sigrok::Context;
using sigrok::Driver;

namespace pv {

std::set<std::string> DeviceManager::stalled_drivers_;
std::vector<std::string> DeviceManager::stalled_reports_;

DeviceManager::DeviceManager(shared_ptr<Context> context,
	std::string driver, bool do_scan) :
	context_(context)
{
	/* The scan runs on a worker thread while a board is re-flashed, and a
	 * widget cannot be built there: the caller shows its own progress then. */
	QCoreApplication *const app = QCoreApplication::instance();
	const bool gui_thread = app && QThread::currentThread() == app->thread();
	unique_ptr<QProgressDialog> progress;
	if (gui_thread)
		progress.reset(new QProgressDialog("",
			QObject::tr("Cancel"), 0, context->drivers().size() + 1));
	if (progress)
		progress->setWindowModality(Qt::WindowModal);
	/*
	 * A scan of a healthy link is over in a fraction of a second, and a
	 * dialog that comes and goes in that time is only a flicker - several
	 * of them, when one board being plugged starts more than one scan.
	 * Only a link that has stopped answering takes long enough to be
	 * worth showing, and that is the case the dialog is there for.
	 */
	if (progress)
		progress->setMinimumDuration(800);

	int entry_num = 1;

	/*
	 * Check the presence of an optional user spec for device scans.
	 * Determine the driver name and options (in generic format) when
	 * applicable.
	 */
	std::string user_name;
	vector<std::string> user_opts;
	if (!driver.empty()) {
		user_opts = pv::util::split_string(driver, ":");
		user_name = user_opts.front();
		user_opts.erase(user_opts.begin());
	}

	/*
	 * Scan for devices. No specific options apply here, this is
	 * best effort auto detection.
	 */
	for (auto& entry : context->drivers()) {
		if (!do_scan)
			break;

		// Skip drivers we won't scan anyway
		if (!driver_supported(entry.second))
			continue;

		if (progress)
			progress->setLabelText(QObject::tr("Scanning for devices that driver %1 can access...")
				.arg(QString::fromStdString(entry.first)));

		if (entry.first == user_name)
			continue;
		driver_scan(entry.second, map<const ConfigKey *, VariantBase>());

		if (!progress)
			continue;
		progress->setValue(entry_num++);
		QApplication::processEvents();
		if (progress->wasCanceled())
			break;
	}

	/*
	 * Optionally run another scan with potentially more specific
	 * options when requested by the user. This is motivated by
	 * several different uses: It can find devices that are not
	 * covered by the above auto detection (UART, TCP). It can
	 * prefer one out of multiple found devices, and have this
	 * device pre-selected for new sessions upon user's request.
	 */
	user_spec_device_.reset();
	if (!driver.empty()) {
		shared_ptr<sigrok::Driver> scan_drv;
		map<const ConfigKey *, VariantBase> scan_opts;

		/*
		 * Lookup the device driver name.
		 */
		map<string, shared_ptr<Driver>> drivers = context->drivers();
		auto entry = drivers.find(user_name);
		scan_drv = (entry != drivers.end()) ? entry->second : nullptr;

		/*
		 * Convert generic string representation of options
		 * to the driver specific data types.
		 */
		if (scan_drv && !user_opts.empty()) {
			auto drv_opts = scan_drv->scan_options();
			scan_opts = drive_scan_options(user_opts, drv_opts);
		}

		/*
		 * Run another scan for the specified driver, passing
		 * user provided scan options this time.
		 */
		list< shared_ptr<devices::HardwareDevice> > found;
		if (scan_drv) {
			found = driver_scan(scan_drv, scan_opts);
			if (!found.empty())
				user_spec_device_ = found.front();
		}
	}
	if (progress)
		progress->setValue(entry_num++);
}

const shared_ptr<sigrok::Context>& DeviceManager::context() const
{
	return context_;
}

shared_ptr<Context> DeviceManager::context()
{
	return context_;
}

const list< shared_ptr<devices::HardwareDevice> >&
DeviceManager::devices() const
{
	return devices_;
}

/**
 * Get the device that was detected with user provided scan options.
 */
shared_ptr<devices::HardwareDevice>
DeviceManager::user_spec_device() const
{
	return user_spec_device_;
}

/**
 * Convert generic options to data types that are specific to Driver::scan().
 *
 * @param[in] user_spec Vector of tokenized words, string format.
 * @param[in] driver_opts Driver's scan options, result of Driver::scan_options().
 *
 * @return Map of options suitable for Driver::scan().
 */
map<const ConfigKey *, Glib::VariantBase>
DeviceManager::drive_scan_options(vector<string> user_spec,
	set<const ConfigKey *> driver_opts)
{
	map<const ConfigKey *, Glib::VariantBase> result;

	for (auto& entry : user_spec) {
		/*
		 * Split key=value specs. Accept entries without separator
		 * (for simplified boolean specifications).
		 */
		string key, val;
		size_t pos = entry.find("=");
		if (pos == std::string::npos) {
			key = entry;
			val = "";
		} else {
			key = entry.substr(0, pos);
			val = entry.substr(pos + 1);
		}

		/*
		 * Skip user specifications that are not a member of the
		 * driver's set of supported options. Have the text format
		 * input spec converted to the required driver specific type.
		 */
		const ConfigKey *cfg;
		try {
			cfg = ConfigKey::get_by_identifier(key);
			if (!cfg)
				continue;
			if (driver_opts.find(cfg) == driver_opts.end())
				continue;
		} catch (...) {
			continue;
		}
		result[cfg] = cfg->parse_string(val);
	}

	return result;
}

bool DeviceManager::driver_supported(shared_ptr<Driver> driver) const
{
	/*
	 * We currently only support devices that can deliver samples at
	 * a fixed samplerate (i.e. oscilloscopes and logic analysers).
	 *
	 * @todo Add support for non-monotonic devices (DMMs, sensors, etc).
	 */
	const auto keys = driver->config_keys();

	return keys.count(ConfigKey::LOGIC_ANALYZER) | keys.count(ConfigKey::OSCILLOSCOPE);
}

namespace {

/* What the port a device sits in carries on the bus right now. */
struct UsbNode
{
	uint16_t vid;
	uint16_t pid;
	uint16_t bcd;
	bool named; /* a running board names itself, a boot loader does not */
};

/* Every node on the bus, keyed the way a device identity spells the port
 * ("4-2.2", or "2.2" when the bus number cannot be read). Empty when libusb
 * cannot be reached. */
map<string, UsbNode> scan_usb_nodes()
{
	map<string, UsbNode> nodes;

	const pv::UsbApi *usb = pv::usb_api();
	if (!usb || !usb->ready())
		return nodes;

	libusb_context *ctx = nullptr;
	if (usb->init(&ctx) != 0)
		return nodes;

	libusb_device **list = nullptr;
	const ssize_t count = usb->get_device_list(ctx, &list);
	for (ssize_t i = 0; i < count; i++) {
		struct libusb_device_descriptor desc;
		if (usb->get_device_descriptor(list[i], &desc) != 0)
			continue;

		uint8_t ports[8] = {0};
		const int port_count = usb->get_port_numbers(list[i], ports, 8);
		if (port_count <= 0)
			continue;

		/* The bus number only separates two boards on the same port of
		 * different buses; without it the chain still names them, the way
		 * a bus event does. */
		string key;
		if (usb->get_bus_number)
			key = std::to_string(usb->get_bus_number(list[i])) + "-";
		for (int p = 0; p < port_count; p++) {
			if (p)
				key += '.';
			key += std::to_string(ports[p]);
		}

		UsbNode node;
		node.vid = desc.idVendor;
		node.pid = desc.idProduct;
		node.bcd = desc.bcdDevice;
		node.named = desc.iManufacturer || desc.iProduct ||
			desc.iSerialNumber;
		nodes[key] = node;
	}

	if (list)
		usb->free_device_list(list, 1);
	usb->exit(ctx);

	return nodes;
}

/*
 * One walk serves everything asking within the poll window: a page waiting for
 * its board asks every 250 ms, and each walk is an init/exit pair plus every
 * node on every bus. The window is shorter than that poll, so nothing is seen
 * later than it would have been.
 */
const qint64 NodesCacheMs = 200;
std::mutex s_nodes_mutex;
map<string, UsbNode> s_nodes_cache;
qint64 s_nodes_at = 0;

map<string, UsbNode> usb_nodes_by_port()
{
	const qint64 now = QDateTime::currentMSecsSinceEpoch();

	{
		std::lock_guard<std::mutex> lock(s_nodes_mutex);
		if (s_nodes_at && now - s_nodes_at < NodesCacheMs)
			return s_nodes_cache;
	}

	const map<string, UsbNode> fresh = scan_usb_nodes();

	std::lock_guard<std::mutex> lock(s_nodes_mutex);
	s_nodes_cache = fresh;
	s_nodes_at = now;

	return fresh;
}

/* A scan has just changed what a board answers: the next question goes to the
 * bus again instead of to the window before it. */
void forget_usb_nodes()
{
	std::lock_guard<std::mutex> lock(s_nodes_mutex);
	s_nodes_at = 0;
}

/*
 * The chain of ports a place spells, without the bus number in front: two
 * contexts in one process do not always number buses alike, and the chain is
 * what places a board.
 */
string chain_of_place(const string &place)
{
	const size_t dash = place.find('-');
	return dash == string::npos ? place : place.substr(dash + 1);
}

/*
 * When each port last had a board that answers on it. An arrival older than
 * that is one the board has already come back from: the board is not on its
 * way back, it is where the page left it before it was pulled again.
 */
std::mutex s_up_mutex;
map<string, qint64> s_last_up;

void note_board_up(const string &place)
{
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	const string chain = chain_of_place(place);

	std::lock_guard<std::mutex> lock(s_up_mutex);

	for (auto it = s_last_up.begin(); it != s_last_up.end(); )
		if (now - it->second > 60000)
			it = s_last_up.erase(it);
		else
			++it;

	s_last_up[chain] = now;
}

bool was_up_since(const string &place, qint64 since_ms)
{
	const string chain = chain_of_place(place);

	std::lock_guard<std::mutex> lock(s_up_mutex);

	const auto it = s_last_up.find(chain);
	return it != s_last_up.end() && it->second > since_ms;
}

/* The node the bus lists at this place, if any: the exact key first, then a
 * key with the same chain under any bus number. */
const UsbNode *node_at(const map<string, UsbNode> &nodes, const string &place)
{
	const auto hit = nodes.find(place);
	if (hit != nodes.end())
		return &hit->second;

	const string chain = chain_of_place(place);
	for (const auto &entry : nodes)
		if (chain_of_place(entry.first) == chain)
			return &entry.second;

	return nullptr;
}

/*
 * Whether the board at this port answers as a running fx2lafw one. The
 * driver also lists a board it has just uploaded to, but that one is off
 * the bus renumbering and cannot be opened - offering it turns a click
 * into an error box, so it waits for the scan that follows its return.
 */
bool fx2_is_running(const map<string, UsbNode> &nodes,
	const string &connection_id)
{
	const string prefix = "usb/";
	if (connection_id.compare(0, prefix.size(), prefix) != 0)
		return true;

	const UsbNode *const found =
		node_at(nodes, connection_id.substr(prefix.size()));
	if (!found)
		return false;

	const UsbNode &node = *found;
	if (node.vid != 0x1d50 || (node.pid != 0x608c && node.pid != 0x608d))
		return false;

	/* The firmware puts a version and its strings in the descriptor; a
	 * board whose EEPROM carries the sigrok ids but no code has neither. */
	return node.bcd != 0 || node.named;
}

/* A vendor board on the bus: the application firmware, or its IAP loader. */
bool is_vendor_board(const UsbNode &node)
{
	return node.vid == 0x1a86 &&
		(node.pid == 0x5537 || node.pid == 0xfe17);
}

bool vendor_is_running(const UsbNode &node)
{
	return node.vid == 0x1a86 && node.pid == 0x5537;
}

} // namespace

bool DeviceManager::board_on_bus(const QString &identity)
{
	/*
	 * Only a board the bus names by port is looked for. The vendor back end
	 * numbers the ports its own way, and such a board is back in
	 * milliseconds anyway - there is nothing to wait for.
	 */
	const QString prefix = QStringLiteral("usb/");
	if (!identity.startsWith(prefix))
		return false;

	const string port = identity.mid(prefix.size()).toStdString();
	if (port.empty())
		return false;

	const map<string, UsbNode> nodes = usb_nodes_by_port();

	return node_at(nodes, port) != nullptr;
}

DeviceManager::UsbBoardState
DeviceManager::board_state(const QString &identity)
{
	const QString prefix = QStringLiteral("usb/");
	if (!identity.startsWith(prefix))
		return UsbBoardState::Absent;

	const string port = identity.mid(prefix.size()).toStdString();
	if (port.empty())
		return UsbBoardState::Absent;

	const map<string, UsbNode> nodes = usb_nodes_by_port();
	const UsbNode *const node = node_at(nodes, port);
	if (!node)
		return UsbBoardState::Absent;

	/*
	 * The same test the device list uses, plus the vendor back end: its
	 * boards answer by their USB id alone, and the loader id means they are
	 * not answering yet. A board read as absent here is one the wait gives
	 * up on, so the vendor ids have to count as present.
	 */
	const bool vendor = is_vendor_board(*node);
	const bool running = vendor ? vendor_is_running(*node) :
		fx2_is_running(nodes, string("usb/") + port);

	if (!running)
		return UsbBoardState::Boot;

	note_board_up(port);

	return vendor ? UsbBoardState::RunningVendor : UsbBoardState::Running;
}

bool DeviceManager::board_up_since(const QString &identity, qint64 since_ms)
{
	const QString prefix = QStringLiteral("usb/");
	if (!identity.startsWith(prefix))
		return false;

	const string port = identity.mid(prefix.size()).toStdString();
	if (port.empty())
		return false;

	return was_up_since(port, since_ms);
}

list< shared_ptr<devices::HardwareDevice> >
DeviceManager::driver_scan(
	shared_ptr<Driver> driver, map<const ConfigKey *, VariantBase> drvopts)
{
	list< shared_ptr<devices::HardwareDevice> > driver_devices;

	assert(driver);

	if (!driver_supported(driver)){
		qDebug() << "!driver_supported(driver)";
		return driver_devices;
	}

	const string name = driver->name();

	/*
	 * A driver that ran into the timeout before is not asked again. Its
	 * scan is still waiting somewhere inside the driver, and a second
	 * request would either queue behind that one or hang in the same place
	 * - either way the device list would stall on every pass from now on.
	 */
	if (DeviceManager::scan_is_stalled(name)) {
		qWarning() << "Skipping a driver whose scan is stuck:"
			<< QString::fromStdString(name);
		return driver_devices;
	}

	// Remove any device instances from this driver from the device
	// list. They will not be valid after the scan.
	devices_.remove_if([&](shared_ptr<devices::HardwareDevice> device) {
		return device->hardware_device()->driver() == driver; });
	
	/*
	 * The scan runs on a thread of its own so that a driver that never
	 * answers - a board wedged in its vendor DLL, a port that went away
	 * without saying so - cannot hold up the program. The driver is held by
	 * the thread itself, so it stays alive even when this returns first.
	 */
	/* Driver::scan() hands back a vector, not a list. */
	typedef std::vector< std::shared_ptr<sigrok::HardwareDevice> > ScannedDevices;
	auto promise = std::make_shared< std::promise<ScannedDevices> >();
	std::future<ScannedDevices> answer = promise->get_future();

	std::thread([driver, drvopts, promise]() {
		try {
			promise->set_value(driver->scan(drvopts));
		} catch (...) {
			/*
			 * Also where set_value() lands when this side already gave up
			 * and its future is gone - there is nobody left to tell.
			 */
			try {
				promise->set_exception(std::current_exception());
			} catch (...) {
			}
		}
	}).detach();

	if (answer.wait_for(std::chrono::milliseconds(ScanTimeoutMs)) !=
		std::future_status::ready) {
		qWarning() << "Driver" << QString::fromStdString(name)
			<< "did not answer its scan within" << ScanTimeoutMs
			<< "ms; carrying on without it";
		stalled_drivers_.insert(name);
		stalled_reports_.push_back(name);
		return driver_devices;
	}

	try {
		// Take the devices the scan found.
		ScannedDevices devices = answer.get();

		/* Nothing is open while a scan runs, so this is when an FX2 can be
		 * asked which firmware it runs. The answer is kept, and the version
		 * dialog shows it later - the page has the board open by then. */
		if (name == "fx2lafw" && !devices.empty())
			pv::fx2_prefetch_versions();

		/*
		 * The fx2lafw driver lists the boards it has just uploaded the
		 * firmware to as well. They are renumbering and cannot be opened,
		 * so only the boards the bus shows as running are taken; the scan
		 * that follows their return brings the rest.
		 */
		const bool fx2_driver = name == "fx2lafw";
		map<string, UsbNode> nodes;
		if (fx2_driver) {
			/* A scan is the point where a board may have just been
			 * flashed: read the bus rather than the window before. */
			forget_usb_nodes();
			nodes = usb_nodes_by_port();
		}

		// Add the scanned devices to the main list, set display names and sort.
		for (shared_ptr<sigrok::HardwareDevice>& device : devices) {
			if (fx2_driver &&
				!fx2_is_running(nodes, device->connection_id())) {
				qInfo() << "fx2lafw:" << QString::fromStdString(
					device->connection_id())
					<< "has no firmware yet, not listed";
				continue;
			}

			/* Either back end that names a board by its port has just seen
			 * it answer; the vendor back end numbers boards its own way. */
			if (device->connection_id().compare(0, 4, "usb/") == 0)
				note_board_up(device->connection_id());

			const shared_ptr<devices::HardwareDevice> d(
				new devices::HardwareDevice(context_, device));
			driver_devices.push_front(d);
		}

		devices_.insert(devices_.end(), driver_devices.begin(),
			driver_devices.end());
		// devices_.sort(bind(&DeviceManager::compare_devices, this, _1, _2));
		// driver_devices.sort(bind(
		// 	&DeviceManager::compare_devices, this, _1, _2));

	} catch (const sigrok::Error &e) {
		qWarning() << QApplication::tr("Error when scanning device driver '%1': %2").
			arg(QString::fromStdString(name), e.what());
	}

	return driver_devices;
}

bool DeviceManager::scan_is_stalled(const std::string &driver_name)
{
	return stalled_drivers_.count(driver_name) > 0;
}

std::vector<std::string> DeviceManager::take_stalled_drivers()
{
	std::vector<std::string> names;
	names.swap(stalled_reports_);
	return names;
}

const map<string, string> DeviceManager::get_device_info(
	shared_ptr<devices::Device> device)
{
	map<string, string> result;

	assert(device);

	const shared_ptr<sigrok::Device> sr_dev = device->device();
	if (sr_dev->vendor().length() > 0)
		result["vendor"] = sr_dev->vendor();
	if (sr_dev->model().length() > 0)
		result["model"] = sr_dev->model();
	if (sr_dev->version().length() > 0)
		result["version"] = sr_dev->version();
	if (sr_dev->serial_number().length() > 0)
		result["serial_num"] = sr_dev->serial_number();
	if (sr_dev->connection_id().length() > 0)
		result["connection_id"] = sr_dev->connection_id();

	return result;
}

const shared_ptr<devices::HardwareDevice> DeviceManager::find_device_from_info(
	const map<string, string> search_info)
{
	shared_ptr<devices::HardwareDevice> last_resort_dev;
	map<string, string> dev_info;

	for (shared_ptr<devices::HardwareDevice> dev : devices_) {
		assert(dev);
		dev_info = get_device_info(dev);

		// If present, vendor and model always have to match.
		if (dev_info.count("vendor") > 0 && search_info.count("vendor") > 0)
			if (dev_info.at("vendor") != search_info.at("vendor"))
				continue;

		if (dev_info.count("model") > 0 && search_info.count("model") > 0)
			if (dev_info.at("model") != search_info.at("model"))
				continue;

		// Most unique match: vendor/model/serial_num (but don't match a S/N of 0)
		if ((dev_info.count("serial_num") > 0) && (dev_info.at("serial_num") != "0")
				&& search_info.count("serial_num") > 0)
			if (dev_info.at("serial_num") == search_info.at("serial_num") &&
					dev_info.at("serial_num") != "0")
				return dev;

		// Second best match: vendor/model/connection_id
		if (dev_info.count("connection_id") > 0 &&
			search_info.count("connection_id") > 0)
			if (dev_info.at("connection_id") == search_info.at("connection_id"))
				return dev;

		// Last resort: vendor/model/version
		if (dev_info.count("version") > 0 &&
			search_info.count("version") > 0)
			if (dev_info.at("version") == search_info.at("version") &&
					dev_info.at("version") != "0")
				return dev;

		// For this device, we merely have a vendor/model match.
		last_resort_dev = dev;
	}

	// If there wasn't even a vendor/model/version match, we end up here.
	// This is usually the case for devices with only vendor/model data.
	// The selected device may be wrong with multiple such devices attached
	// but it is the best we can do at this point. After all, there may be
	// only one such device and we do want to select it in this case.
	return last_resort_dev;
}

const shared_ptr<devices::HardwareDevice> DeviceManager::find_saved_device(
	const map<string, string> &info)
{
	const auto model = info.find("model");
	const auto sn = info.find("serial_num");
	const auto conn = info.find("connection_id");

	/*
	 * Only the WeAct Studio Logic Analyzer V2 gives every board a serial
	 * number of its own. The other boards of this vendor may report one and
	 * the same, and a serial they all carry names none of them.
	 */
	const bool serial_is_an_id = model != info.end() &&
		model->second.find(util::WeActV2ModelName) != string::npos &&
		sn != info.end() && !sn->second.empty() && sn->second != "0";

	if (serial_is_an_id) {
		shared_ptr<devices::HardwareDevice> found;
		string found_port;
		bool ambiguous = false;

		for (const shared_ptr<devices::HardwareDevice> &dev : devices_) {
			const shared_ptr<sigrok::Device> sr_dev = dev->device();
			if (!sr_dev || sr_dev->serial_number() != sn->second ||
					sr_dev->model() != model->second)
				continue;

			if (!found) {
				found = dev;
				found_port = sr_dev->connection_id();
			} else if (sr_dev->connection_id() != found_port) {
				/* One serial, two ports: it is not a serial of its own
				 * after all, and the port is what tells them apart. */
				ambiguous = true;
			}
		}

		/* Nothing found means that board is not here, and nothing else may
		 * be put in its place. */
		if (!ambiguous)
			return found;
	}

	if (conn != info.end())
		for (const shared_ptr<devices::HardwareDevice> &dev : devices_) {
			const shared_ptr<sigrok::Device> sr_dev = dev->device();
			if (sr_dev && sr_dev->connection_id() == conn->second)
				return dev;
		}

	/*
	 * Such a board cannot be told from its brothers anyway, so one of the
	 * same model in another port is the best this can do.
	 */
	return find_device_from_info(info);
}

bool DeviceManager::compare_devices(shared_ptr<devices::Device> a,
	shared_ptr<devices::Device> b)
{
	assert(a);
	assert(b);
	return a->display_name(*this).compare(b->display_name(*this)) < 0;
}

} // namespace pv
