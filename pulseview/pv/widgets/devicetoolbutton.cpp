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

#include <cassert>
#include <algorithm>

#include <QEvent>
#include <QHBoxLayout>
#include <QStyle>
#include <QTimer>
#include <QToolTip>
#include <QWidgetAction>
#include <QDebug>
#include <libsigrokcxx/libsigrokcxx.hpp>

#include <pv/devicemanager.hpp>
#include <pv/devices/device.hpp>
#include <pv/devices/hardwaredevice.hpp>

#include "devicetoolbutton.hpp"

using std::dynamic_pointer_cast;
using std::list;
using std::shared_ptr;
using std::string;
using std::weak_ptr;
using std::vector;

using pv::devices::Device;

namespace pv {
namespace widgets {


DeviceToolButton::DeviceToolButton(QWidget *parent,
	DeviceManager *device_manager,
	QAction *connect_action) :
	QToolButton(parent),
	device_manager_(device_manager),
	connect_action_(connect_action),
	menu_(this),
	devices_(),
	refresh_action_(new QAction(this))
{
	setPopupMode(QToolButton::MenuButtonPopup);
	setMenu(&menu_);
	setToolTip("");

	connect(this, SIGNAL(clicked()), this, SLOT(on_button_clicked()));

	connect(&menu_, SIGNAL(hovered(QAction*)),
		this, SLOT(on_menu_hovered(QAction*)));

	connect(refresh_action_, SIGNAL(triggered()),
		this, SIGNAL(refresh_requested()));

	update_minimum_width();
}

void DeviceToolButton::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		/* The menu is rebuilt from the device list whenever it changes,
		 * but the label on the button keeps whatever text it was given. */
		refresh_action_->setText(tr("Refresh Device List"));

		if (selected_device_ && device_manager_)
			set_label(QString::fromStdString(
				selected_device_->display_name(*device_manager_)));
		else
			set_label(tr("<No Device>"));
	}

	QToolButton::changeEvent(event);
}

shared_ptr<Device> DeviceToolButton::selected_device()
{
	return selected_device_;
}

void DeviceToolButton::set_device_list(
	const list< shared_ptr<Device> > &devices, shared_ptr<Device> selected)
{
	selected_device_ = selected;
	set_label(selected ? QString::fromStdString(
		selected->display_name(*device_manager_)) : tr("<No Device>"));
	devices_ = vector< weak_ptr<Device> >(devices.begin(), devices.end());
	update_device_list();
}

void DeviceToolButton::reset()
{
	set_label(tr("<No Device>"));
}

void DeviceToolButton::set_device_list_name(QString &device_list_name)
{
	set_label(device_list_name);
}

void DeviceToolButton::update_minimum_width()
{
	/* Label width plus the menu indicator. The label itself is already
	 * capped by set_label(), so the button can never grow arbitrarily
	 * wide. Fixed policy keeps the layout from squeezing it either. */
	const QFontMetrics fm(font());
	const int width = fm.boundingRect(text()).width() +
		fm.averageCharWidth() * 2 + 24;   /* 24: menu indicator */

	setMinimumWidth(width);
	setMaximumWidth(width);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void DeviceToolButton::set_label(const QString &label)
{
	full_label_ = label;

	const QFontMetrics fm(font());
	const int max_text_width = fm.averageCharWidth() * 16;
	const QString shown = fm.elidedText(label, Qt::ElideMiddle, max_text_width);

	QToolButton::setText(shown);
	if (shown != label)
		setToolTip(label);   /* keep the full name reachable */

	update_minimum_width();
}

static bool is_demo_device(const shared_ptr<Device> &dev)
{
	const shared_ptr<devices::HardwareDevice> hw_device =
		dynamic_pointer_cast<devices::HardwareDevice>(dev);

	return hw_device && (hw_device->hardware_device()->driver()->name() == "demo");
}

bool DeviceToolButton::only_demo_device() const
{
	/* A file device in use is not something a rescan would replace, so
	 * keep the entry out of the way while one is loaded. */
	if (selected_device_ && !is_demo_device(selected_device_))
		return false;

	for (const weak_ptr<Device> &dev_weak_ptr : devices_) {
		shared_ptr<Device> dev(dev_weak_ptr.lock());
		if (dev && !is_demo_device(dev))
			return false;
	}

	return true;
}

void DeviceToolButton::set_device_selector_role(bool new_session_allowed,
	bool single_device_only)
{
	new_session_allowed_ = new_session_allowed;
	single_device_only_ = single_device_only;
	update_device_list();
}

void DeviceToolButton::set_busy_devices(const QSet<QString> &busy,
	const QString &own)
{
	busy_devices_ = busy;
	own_identity_ = own;
	update_device_list();
}

QString DeviceToolButton::device_identity(const shared_ptr<Device> &device)
{
	if (!device)
		return QString();

	const shared_ptr<sigrok::Device> sr_dev = device->device();
	if (!sr_dev)
		return QString();

	/*
	 * The connection id comes first: it is unique for every device on the
	 * bus, while a serial number can be repeated by firmware that writes
	 * the same one into every board. A device that reports neither falls
	 * back to what it says about itself.
	 */
	const string conn = sr_dev->connection_id();
	if (!conn.empty())
		return QString::fromStdString(conn);

	const string serial = sr_dev->serial_number();
	if (!serial.empty())
		return QString::fromStdString(serial);

	return QString::fromStdString(sr_dev->vendor()) + "/" +
		QString::fromStdString(sr_dev->model());
}

void DeviceToolButton::set_current_device(
	const shared_ptr<devices::Device> &device)
{
	selected_device_ = device;
	set_label(device ? QString::fromStdString(
		device->display_name(*device_manager_)) : tr("<No Device>"));
}

void DeviceToolButton::activate_device(shared_ptr<Device> device)
{
	selected_device_ = device;
	set_label(device ? QString::fromStdString(
		device->display_name(*device_manager_)) : tr("<No Device>"));
	update_device_list();
	device_selected();
}

void DeviceToolButton::clear_selection()
{
	activate_device(nullptr);
}

void DeviceToolButton::update_device_list()
{
	menu_.clear();

	vector< shared_ptr<Device> > shown;
	for (weak_ptr<Device>& dev_weak_ptr : devices_) {
		shared_ptr<Device> dev(dev_weak_ptr.lock());
		if (!dev)
			continue;

		/* A session opened for one board stays with it: the menu is not a
		 * way to swap that board for another one. */
		if (single_device_only_ && (dev != selected_device_))
			continue;

		shown.push_back(dev);
	}

	/*
	 * The demo device is the stand-in for a board that was not there. Once
	 * one is in the list the stand-in is only noise, so it goes - unless it
	 * is all there is, in which case it is what the menu has to offer.
	 */
	const bool has_hardware = std::any_of(shown.begin(), shown.end(),
		[](const shared_ptr<Device> &dev) { return !is_demo_device(dev); });
	if (has_hardware)
		shown.erase(std::remove_if(shown.begin(), shown.end(),
			[](const shared_ptr<Device> &dev) { return is_demo_device(dev); }),
			shown.end());

	for (const shared_ptr<Device> &dev : shown) {
		const bool current = (dev == selected_device_);
		const QString identity = device_identity(dev);

		QWidget *const row = new QWidget(&menu_);
		QHBoxLayout *const row_layout = new QHBoxLayout(row);
		row_layout->setContentsMargins(6, 0, 4, 0);
		row_layout->setSpacing(0);

		QToolButton *const pick = new QToolButton(row);
		pick->setAutoRaise(true);
		pick->setToolButtonStyle(Qt::ToolButtonTextOnly);
		/* A check mark for the one in use, spaces for the rest, so the
		 * names stay in one column. */
		pick->setText((current ? QString::fromUtf8("\xE2\x9C\x93  ")
			: QString("      ")) +
			QString::fromStdString(dev->display_name(*device_manager_)));
		pick->setToolTip(QString::fromStdString(dev->full_name()));
		pick->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		QFont pick_font = pick->font();
		pick_font.setBold(current);
		pick->setFont(pick_font);

		/*
		 * A board another session is on is not for this page to move to:
		 * the session that holds it is the one that runs it.
		 */
		if (!current && !identity.isEmpty() &&
			busy_devices_.contains(identity) && (identity != own_identity_)) {
			pick->setEnabled(false);
			pick->setToolTip(tr("This device is in use by another session"));
		}

		row_layout->addWidget(pick, 1);

		if (new_session_allowed_) {
			QToolButton *const open_new = new QToolButton(row);
			open_new->setAutoRaise(true);
			open_new->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
			open_new->setFixedSize(18, 18);
			const bool busy = identity.isEmpty() || busy_devices_.contains(identity);
			open_new->setEnabled(!busy);
			open_new->setToolTip(busy
				? tr("This device is already open in another session")
				: tr("Open this device in a new session"));
			row_layout->addWidget(open_new, 0);
			connect(open_new, &QToolButton::clicked, this, [this, identity]() {
				menu_.close();
				/* Queued: the answer rebuilds this menu, and the row this
				 * click came from is gone once the handler returns. */
				QTimer::singleShot(0, this, [this, identity]() {
					device_new_session_requested(identity);
				});
			});
		}

		connect(pick, &QToolButton::clicked, this, [this, dev]() {
			menu_.close();
			QTimer::singleShot(0, this, [this, dev]() {
				activate_device(dev);
			});
		});

		QWidgetAction *const action = new QWidgetAction(&menu_);
		action->setDefaultWidget(row);
		menu_.addAction(action);
	}

	if (only_demo_device()) {
		/* No hardware device was found. Offer a rescan where the devices
		 * would be listed, as that is the only way to get one. */
		refresh_action_->setText(tr("Refresh Device List"));
		menu_.addSeparator();
		menu_.addAction(refresh_action_);
	}

	if (selected_device_ != nullptr)
		setToolTip(QString::fromStdString(selected_device_->full_name()));

	/* New rows need their style again. Only the first list gets it ahead of
	 * time: a device coming or going must not cost half a second. */
	if (!menu_prepared_once_) {
		if (!menu_prepare_timer_) {
			menu_prepare_timer_ = new QTimer(this);
			menu_prepare_timer_->setSingleShot(true);
			connect(menu_prepare_timer_, &QTimer::timeout, this, [this]() {
				menu_prepared_once_ = true;
				prepare_menu();
			});
		}

		/* Restarted by every rebuild: the start-up list is built twice, and
		 * preparing rows that are about to go is wasted work. */
		menu_prepare_timer_->start(200);
	}
}

void DeviceToolButton::update_device_mamager(DeviceManager * dev_manager)
{
	device_manager_ = dev_manager;
}


void DeviceToolButton::on_menu_hovered(QAction *action)
{
	assert(action);

	// Only show the tooltip for device entries (they hold
	// device pointers in their data field)
	if (!action->data().isValid())
		return;

	device_tooltip_ = action->toolTip();

	if (QToolTip::isVisible())
		on_menu_hover_timeout();
	else
		QTimer::singleShot(500, this, SLOT(on_menu_hover_timeout()));
}

void DeviceToolButton::on_menu_hover_timeout()
{
	if (device_tooltip_.isEmpty())
		return;

	QToolTip::showText(QCursor::pos(), device_tooltip_);
}

/* The rows are styled the first time the menu is shown, which is what
 * made the first click feel slow. Done ahead of it, nothing waits. */
void DeviceToolButton::prepare_menu()
{
	menu_.ensurePolished();
	for (QWidget *row : menu_.findChildren<QWidget *>())
		row->ensurePolished();
	menu_.sizeHint();
}

void DeviceToolButton::on_button_clicked()
{
	menu_.exec(mapToGlobal(QPoint(0, height())));
}

}  // namespace widgets
}  // namespace pv
