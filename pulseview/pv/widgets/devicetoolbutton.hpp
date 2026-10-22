/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnalyzer is based on PulseView.
 *
 * Copyright (C) 2014 Joel Holdsworth <joel@airwebreathe.org.uk>
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

#ifndef PULSEVIEW_PV_WIDGETS_DEVICETOOLBUTTON_HPP
#define PULSEVIEW_PV_WIDGETS_DEVICETOOLBUTTON_HPP

#include <list>
#include <memory>
#include <vector>

#include <QAction>
#include <QMenu>
#include <QSet>
#include <QToolButton>
#include "../devicemanager.hpp"
using std::list;
using std::shared_ptr;
using std::vector;
using std::weak_ptr;

class QTimer;
struct srd_decoder;

namespace pv {

class DeviceManager;

namespace devices {
class Device;
}

namespace widgets {

class DeviceToolButton : public QToolButton
{
	Q_OBJECT;

public:
	/**
	 * Constructor
	 * @param parent the parent widget.
	 * @param device_manager the device manager.
	 * @param connect_action the connect-to-device action.
	 */
	DeviceToolButton(QWidget *parent, DeviceManager *device_manager,
		QAction *connect_action);

	/**
	 * Returns a reference to the selected device.
	 */
	shared_ptr<devices::Device> selected_device();

	/**
	 * Sets the current list of devices.
	 * @param device the list of devices.
	 * @param selected_device the currently active device.
	 */
	void set_device_list(
		const list< shared_ptr<devices::Device> > &devices,
		shared_ptr<devices::Device> selected);

	/**
	 * Replaces what the menu offers, leaving the name of the device in use
	 * alone. A pass that could not hand a page its board back uses this:
	 * the menu still belongs to a scan that is gone, while the label is
	 * about a board that never came back.
	 */
	void set_available_devices(
		const list< shared_ptr<devices::Device> > &devices);

	/**
	 * Sets the current device to "no device". Useful for when a selected
	 * device fails to open.
	 */
	void reset();
	void set_device_list_name(QString &device_list_name);

	/**
	 * Decides what the menu offers besides picking a device.
	 * @param new_session_allowed show the "open in a new session" wheel;
	 *        only the capture page that owns every device does that.
	 * @param single_device_only list nothing but the device in use.
	 */
	void set_device_selector_role(bool new_session_allowed,
		bool single_device_only);

	/**
	 * Identities of the devices another session already holds. Their
	 * "open in a new session" wheel is greyed out: one board cannot be
	 * captured by two sessions at the same time.
	 */
	/**
	 * Boards another session holds are not offered for switching to: two
	 * pages on one board would both try to run it. The one this session is
	 * on is not one of them - it is where the session already is.
	 */
	void set_busy_devices(const QSet<QString> &busy, const QString &own);

	/**
	 * Makes the button stand for this device. The label is kept from then
	 * on, which it otherwise is not: a language change puts the name of the
	 * device it was told about last, not the one the session is on.
	 */
	void set_current_device(const shared_ptr<devices::Device> &device);

	/**
	 * Drops the selection: the button shows no device. Used when the one
	 * that was picked could not be opened, so the session holds nothing.
	 */
	void clear_selection();

	/**
	 * A stable name for a device, so a session can be matched to the board
	 * it holds across a rescan.
	 */
	static QString device_identity(const shared_ptr<devices::Device> &device);

	void update_device_mamager(DeviceManager * dev_manager);

private:
	/**
	 * Repopulates the menu from the device list.
	 */
	void update_device_list();

	/**
	 * Sizes the button to its current label plus the drop-down arrow.
	 */
	void update_minimum_width();

	/**
	 * Sets the label, eliding it when it is too long, and resizes the
	 * button accordingly.
	 */
	void set_label(const QString &label);

	/**
	 * Returns true when the device list holds nothing but the demo
	 * device, so that scanning the buses again is the only way to get a
	 * usable device.
	 */
	bool only_demo_device() const;

	/** Makes @c device the one this button shows, then tells the owner. */
	void activate_device(shared_ptr<devices::Device> device);

protected:
	/* Called when the application language changed. */
	void changeEvent(QEvent *event) override;

private Q_SLOTS:
	void on_menu_hovered(QAction *action);
	void on_menu_hover_timeout();
	void on_button_clicked();  // Handle a button click and show the drop-down menu

Q_SIGNALS:
	void device_selected();

	/**
	 * Emitted when the user asked for a new device scan because no
	 * hardware device was available.
	 */
	void refresh_requested();

	/** Open this device in a session of its own. */
	void device_new_session_requested(QString identity);

private:
	/* Applies the style to the menu and lays it out, so that the first
	 * click does not have to. */
	void prepare_menu();
	/* Runs prepare_menu() once the list has stopped being rebuilt. */
	QTimer *menu_prepare_timer_ = nullptr;
	bool menu_prepared_once_ = false;

	DeviceManager * device_manager_;
	QAction *const connect_action_;

	QMenu menu_;

	shared_ptr<devices::Device> selected_device_;

	QString full_label_;
	vector< weak_ptr<devices::Device> > devices_;
	QSet<QString> busy_devices_;
	QString own_identity_;
	bool new_session_allowed_ = false;
	bool single_device_only_ = false;

	QString device_tooltip_;

	/* Kept alive across menu rebuilds: deleting an action while its own
	 * triggered() signal is being handled is not safe. */
	QAction *const refresh_action_;
};

}  // namespace widgets
}  // namespace pv

#endif // PULSEVIEW_PV_WIDGETS_DEVICETOOLBUTTON_HPP
