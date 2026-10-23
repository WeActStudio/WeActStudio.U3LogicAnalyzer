/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2012-2015 Joel Holdsworth <joel@airwebreathe.org.uk>
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
#ifndef ENABLE_DECODE
#define ENABLE_DECODE
#endif
#include <extdef.h>
#include <QApplication>
#include "pv/application.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <set>
#include <string>
#include <cstdarg>

#include <QDir>
#include <QFileInfo>

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHelpEvent>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QTimer>
#include <QToolTip>
#include <QString>
#include "mainbar.hpp"
#include <thread>
#include <boost/algorithm/string/join.hpp>
#include <QDateTime>
#include <QTime>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QEventLoop>
#include <QCoreApplication>
#include <pv/data/mathsignal.hpp>
#include <pv/devicemanager.hpp>
#include <pv/devices/hardwaredevice.hpp>
#include <pv/devices/inputfile.hpp>
#include <pv/devices/sessionfile.hpp>
#include <pv/dialogs/connect.hpp>
#include <pv/dialogs/inputoutputoptions.hpp>
#include <pv/dialogs/storeprogress.hpp>
#include <pv/dialogs/settings.hpp>
#include <pv/globalsettings.hpp>
#include <pv/mainwindow.hpp>
#include <pv/popups/channels.hpp>
#include <pv/popups/deviceoptions.hpp>
#include <pv/usbapi.hpp>
#include "../../../libsigrok/src/hardware/wch-ch32h417-libusb/libusb_wrapper.h"
#include <pv/util.hpp>
#include <pv/views/trace/view.hpp>
#include <pv/widgets/exportmenu.hpp>
#include <pv/widgets/importmenu.hpp>
#include "../dialogs/iapdialog.hpp"
#ifdef ENABLE_DECODE
#include <pv/data/decodesignal.hpp>
#include <QtGlobal>
#include <QScreen>

#endif

#include <libsigrokcxx/libsigrokcxx.hpp>

#include "../util.hpp"
using namespace pv::util;

using std::back_inserter;
using std::copy;
using std::list;
using std::make_pair;
using std::make_shared;
using std::map;
using std::max;
using std::min;
using std::pair;
using std::set;
using std::shared_ptr;
using std::string;
using std::vector;

using sigrok::Capability;
using sigrok::ConfigKey;
using sigrok::Error;
using sigrok::InputFormat;
using sigrok::OutputFormat;

using boost::algorithm::join;
#define WCH_VID 0x1a86
#define WCH_PID_CH569 0x8025
#define WCH_PID_CH32H417 0x5537
#define WCH_PID_CH32H417_HID 0xFE17

namespace {

/* One wording for the mask: a title, and a hint under it. */
QString mask_html(const QString &title, const QString &hint)
{
	return QString("<div style=\"text-align:center;\">"
		"<span style=\"font-size:14pt;font-weight:600;\">%1</span>"
		"<br/><br/>%2</div>").arg(title, hint);
}

} // namespace

namespace pv
{

	namespace toolbars
	{
	namespace {

	/* Both wrappers cap their enumeration at 16 boards. */
	const unsigned long MaxUpgradeableBoards = 16;

	/* How long an upgraded board is given before the bus is read: right at the
	 * reset it still reports the port it sat in before the firmware change. */
	const int UpgradeSettleMs = 1000;

	/* How long the mask stays up after the last block went out: the board is
	 * resetting, and a mask that waits for word of it may wait for good. */
	const int UpgradeMaskGraceMs = 3000;

	/* Whether this page is on a board the upgrade tool can talk to at all.
	 * An FX2 running the sigrok firmware is not one of them. */
	bool upgradeable_device(const shared_ptr<devices::Device> &device)
	{
		const shared_ptr<devices::HardwareDevice> hw =
			dynamic_pointer_cast<devices::HardwareDevice>(device);

		if (!hw)
			return false;

		return util::get_wch_device_type_by_driver(
			hw->hardware_device()->driver()->name()) ==
			util::WchDeviceType::CH32H417;
	}

	/*
	 * Whether a bus event's chain is the place this identity sits in. Only the
	 * libusb back end names a place by port chain ("usb/4-2.2", chain "2.2");
	 * the vendor one writes "ch375/<hub>-<port>", which no chain of ports
	 * matches - such a board is noted by the scan instead (note_gone).
	 */
	bool chain_matches_identity(const QString &identity, const QString &chain)
	{
		if (chain.isEmpty())
			return false;

		const int slash = identity.indexOf('/');
		if (slash < 0)
			return false;

		const QString place = identity.mid(slash + 1);
		if (place == chain)
			return true;

		const int dash = place.indexOf('-');
		return dash >= 0 && place.mid(dash + 1) == chain;
	}

	/*
	 * Where the upgrade tool finds this board. Both backends number what they
	 * enumerate, and that number moves as boards come and go - so the port
	 * chain the page is known by is what is looked for, not an order.
	 */
	int upgrade_index_for(const QString &identity, bool libusb_backend)
	{
		if (identity.isEmpty())
			return -1;

#ifdef _WIN32
		/* The DLL is unloaded with the last page that watched the bus; the
		 * wrap of the enumeration below needs it back. Loading it twice is
		 * a no-op. */
		if (!libusb_backend && ch375_init() != 0) {
			qWarning() << "Firmware upgrade: the CH375 DLL is not available";
			return -1;
		}
#endif

		for (unsigned long i = 0; i < MaxUpgradeableBoards; i++) {
			char path[128] = { 0 };

			if (libusb_backend) {
				/* The libusb wrapper reports "usb/<bus>-<port>", which is
				 * the identity as the driver wrote it. */
				if (ch32h417_libusb_get_usb_path(i, path,
						sizeof(path)) != 0)
					continue;
				if (identity == QString::fromLatin1(path))
					return (int)i;
				continue;
			}

#ifdef _WIN32
			/* The vendor wrapper spells the identity itself - "ch375/<hub>-<port>",
			 * exactly as the driver reports it. */
			if (ch375_get_usb_path(i, path, sizeof(path)) < 0)
				continue;
			if (identity == QString::fromLatin1(path))
				return (int)i;
#endif
		}

		return -1;
	}

	} // namespace

	MainBar *MainBar::s_mainbar_instance = nullptr;
	atomic<int> MainBar::s_captures_running{ 0 };
	atomic<bool> MainBar::s_attach_deferred{ false };
	std::mutex MainBar::s_capture_mutex;
	QSet<QString> MainBar::s_capturing_identities;
	std::mutex MainBar::s_gone_mutex;
	QSet<QString> MainBar::s_declared_gone;
	std::mutex MainBar::s_gone_at_mutex;
	QHash<QString, qint64> MainBar::s_gone_at;
	std::mutex MainBar::s_arrived_mutex;
	QHash<QString, qint64> MainBar::s_arrived;
	std::mutex MainBar::s_upgrade_mutex;
	QString MainBar::s_upgrade_target;
	atomic<int64_t> MainBar::s_upgrade_deadline{ 0 };

	bool MainBar::upgrade_running()
	{
		std::lock_guard<std::mutex> lock(s_upgrade_mutex);
		return !s_upgrade_target.isEmpty();
	}

	bool MainBar::captures_running()
	{
		return s_captures_running > 0;
	}

	void MainBar::begin_upgrade(const QString &identity)
	{
		std::lock_guard<std::mutex> lock(s_upgrade_mutex);
		s_upgrade_target = identity;
		s_upgrade_deadline.store(g_get_monotonic_time() + 60 * G_USEC_PER_SEC);
	}

	void MainBar::end_upgrade()
	{
		std::lock_guard<std::mutex> lock(s_upgrade_mutex);
		s_upgrade_target.clear();
	}

	bool MainBar::is_device_upgrading() const
	{
		return upgrade_mask_up_;
	}

	void MainBar::declare_gone(const QStringList &identities)
	{
		if (identities.isEmpty())
			return;

		const qint64 now = QDateTime::currentMSecsSinceEpoch();

		std::lock_guard<std::mutex> lock(s_gone_mutex);
		for (const QString &identity : identities)
			s_declared_gone.insert(identity);

		/*
		 * Kept apart from the set above, which the pass takes and clears: a
		 * capture that ended because of the pull reads this afterwards.
		 */
		std::lock_guard<std::mutex> at_lock(s_gone_at_mutex);
		for (const QString &identity : identities)
			s_gone_at.insert(identity, now);
	}

	qint64 MainBar::gone_at(const QString &identity)
	{
		std::lock_guard<std::mutex> lock(s_gone_at_mutex);

		const qint64 now = QDateTime::currentMSecsSinceEpoch();
		for (auto it = s_gone_at.begin(); it != s_gone_at.end(); )
			if (now - it.value() > GoneMemoryMs)
				it = s_gone_at.erase(it);
			else
				++it;

		return s_gone_at.value(identity, 0);
	}

	/*
	 * A board a scan no longer lists. Kept where the bus events keep theirs:
	 * that is what the link warning reads before it blames the rate.
	 */
	void MainBar::note_gone(const QString &identity)
	{
		if (identity.isEmpty())
			return;

		const qint64 now = QDateTime::currentMSecsSinceEpoch();

		std::lock_guard<std::mutex> at_lock(s_gone_at_mutex);
		s_gone_at.insert(identity, now);
	}

	QSet<QString> MainBar::take_declared_gone()
	{
		std::lock_guard<std::mutex> lock(s_gone_mutex);
		const QSet<QString> taken = s_declared_gone;
		s_declared_gone.clear();
		return taken;
	}

	void MainBar::declare_arrived(const QStringList &chains)
	{
		if (chains.isEmpty())
			return;

		const qint64 now = QDateTime::currentMSecsSinceEpoch();

		std::lock_guard<std::mutex> lock(s_arrived_mutex);
		for (const QString &chain : chains)
			s_arrived.insert(chain, now);
	}

	qint64 MainBar::arrived_at(const QString &chain)
	{
		std::lock_guard<std::mutex> lock(s_arrived_mutex);

		const qint64 now = QDateTime::currentMSecsSinceEpoch();
		for (auto it = s_arrived.begin(); it != s_arrived.end(); )
			if (now - it.value() > ArrivalMemoryMs)
				it = s_arrived.erase(it);
			else
				++it;

		const auto hit = s_arrived.constFind(chain);
		if (hit != s_arrived.constEnd())
			return hit.value();

		/*
		 * The asker may spell the place without its bus, or with one this
		 * context numbers differently: the ports are what has to agree.
		 */
		for (auto it = s_arrived.constBegin(); it != s_arrived.constEnd();
			++it) {
			const int dash = it.key().indexOf(QLatin1Char('-'));
			if (dash >= 0 && it.key().mid(dash + 1) == chain)
				return it.value();
		}

		return 0;
	}

	bool MainBar::take_deferred_if_idle()
	{
		std::lock_guard<std::mutex> lock(s_capture_mutex);

		if (s_captures_running > 0)
			return false;

		return s_attach_deferred.exchange(false);
	}
		const uint64_t MainBar::MinSampleCount = 12500ULL;
		const uint64_t MainBar::MaxSampleCount = 1000000000000ULL;
		const uint64_t MainBar::DefaultSampleCount = 1000000;
		const uint64_t MainBar::MinSampleRate = 25000ULL;
		const char *MainBar::SettingOpenDirectory = "MainWindow/OpenDirectory";
		const char *MainBar::SettingSaveDirectory = "MainWindow/SaveDirectory";

		MainBar::MainBar(Session &session, QWidget *parent, pv::views::trace::View *view) : StandardBar(session, parent, view, false),
																							channel_dlg(parent),
																							action_new_view_(new QAction(this)),
																							action_open_(new QAction(this)),
																							action_save_(new QAction(this)),
																							action_save_as_(new QAction(this)),
																							action_save_selection_as_(new QAction(this)),
																							action_restore_setup_(new QAction(this)),
																							action_save_setup_(new QAction(this)),
																							action_connect_(new QAction(this)),
																							run_stop_button_(new QToolButton()),
																							channels_button_ex_(new QToolButton()),
																							model_change_button_(new pv::widgets::HoverToolButton()),
																							new_view_button_(new pv::widgets::HoverToolButton()),
																							open_button_(new QToolButton()),
																							save_button_(new QToolButton()),
																							help_button_(new pv::widgets::HoverToolButton()),
																							device_status_icon_(new QToolButton()),
																							device_selector_(parent, session.device_manager_ptr(), action_connect_),
																							// configure_button_(this),
																							// configure_button_action_(nullptr),
																							// channels_button_(this),
																							channels_button_(nullptr),
																							channels_button_action_(nullptr),
																							// sample_count_(" samples", this),
																							sample_count_("s", this),
																							sample_rate_("Hz", this),
																							updating_sample_rate_(false),
																							updating_sample_count_(false),
																							sample_count_supported_(false),
#ifdef ENABLE_DECODE
																							add_decoder_button_(new QToolButton())
#endif
		// add_math_signal_button_(new QToolButton())
		{
			setIconSize(QSize(32, 32));
			channel_number_ = 16;
			channels_ = nullptr;

			// Initialize the device status icon
			device_status_icon_->setObjectName(QString::fromUtf8("device_status_icon"));
			device_status_icon_->setIcon(QIcon(":/icons/Demo.png"));
			device_status_icon_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			device_status_icon_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			setObjectName(QString::fromUtf8("MainBar"));

			setContextMenuPolicy(Qt::PreventContextMenu);

			setStyleSheet(
				"QToolBar {"
				"    spacing: 2px;"
				"    padding: 4px;"
				"}"
				"QToolBar QToolButton {"
				"    margin: 0px;"
				"    padding: 2px;"
				"    border: 1px solid transparent;"
				"    border-radius: 2px;"
				"}"
				"QToolBar QToolButton:hover {"
				"    background: rgba(180, 180, 180, 60);"
				"    border: 1px solid rgba(120, 120, 120, 100);"
				"}"
				"QToolBar QToolButton:pressed {"
				"    background: rgba(150, 150, 150, 80);"
				"}"
				// A button that stands for a mode - whether the cursors are
				// shown, whether the view follows the capture - has to show
				// that it is on, and the style leaves the checked state as
				// little more than a hairline. MainBar sets a style sheet of
				// its own, which is what takes the place of the one the
				// StandardBar it derives from had set, so the rule has to be
				// here as well as there.
				"QToolBar QToolButton:checked {"
				"    background: rgba(150, 150, 150, 110);"
				"    border: 1px solid rgba(140, 140, 140, 160);"
				"}"
				"QToolBar QToolButton#device_status_icon {"
				"}"
				"QToolBar QToolButton#device_status_icon:disabled {"
				"}"
				"QToolBar QToolButton[popupMode='1'] {"  // MenuButtonPopup
				"    padding-right: 8px;"
				"}"
				"QToolBar QToolButton[popupMode='2'] {"  // InstantPopup
				"    padding-right: 6px;"
				"}"
				"QToolBar QToolButton::menu-button {"
				"    width: 12px;"
				"    border: none;"
				"    background: transparent;"
				"}"
				"QToolBar QToolButton::menu-button:hover {"
				"    background: rgba(180, 180, 180, 60);"
				"    border: none;"
				"}"
				"QToolBar QToolButton::menu-arrow {"
				"    width: 8px;"
				"}"
				"QToolBar QToolButton::menu-arrow:hover {"
				"    background: rgba(180, 180, 180, 60);"
				"}"
				// The overflow button Qt adds when the bar is too narrow paints
				// its arrow with palette(Button) - the very colour it is filled
				// with - so the stock arrow is invisible (measured contrast of
				// only 2..19 out of 255, on both light and dark themes). Qt
				// ignores icon and text for that button, hence the arrow is
				// supplied as a background image with a mid grey stroke that
				// reads well on either background.
				"QToolBar QToolButton#qt_toolbar_ext_button {"
				"    background-image: url(:/icons/toolbar-arrow-down.svg);"
				"    background-position: center;"
				"    background-repeat: no-repeat;"
				"    border: 1px solid palette(mid);"
				"    border-radius: 2px;"
				"}"
				"QToolBar QToolButton#qt_toolbar_ext_button:hover {"
				"    background-color: rgba(180, 180, 180, 60);"
				"    border: 1px solid rgba(120, 120, 120, 100);"
				"}"
				// Flip the arrow while the overflow menu is open, so the button
				// reads "collapse" instead of "expand". :checked covers the menu
				// being open, :pressed covers the moment it is being clicked.
				"QToolBar QToolButton#qt_toolbar_ext_button:checked,"
				"QToolBar QToolButton#qt_toolbar_ext_button:pressed {"
				"    background-image: url(:/icons/toolbar-arrow-up.svg);"
				"}"
			);

			// Actions
			action_new_view_->setIcon(QIcon(":/icons/decode_result.png"));
			connect(action_new_view_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionNewView_triggered()));

			action_open_->setIcon(QIcon(":/icons/document-open.png"));
			action_open_->setShortcut(pv::util::key_sequence(Qt::CTRL, Qt::Key_O));
			connect(action_open_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionOpen_triggered()));

			connect(action_restore_setup_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionRestoreSetup_triggered()));

			action_save_->setIcon(QIcon(":/icons/document-save-as.png"));
			action_save_->setShortcut(pv::util::key_sequence(Qt::CTRL, Qt::Key_S));
			connect(action_save_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionSave_triggered()));

			action_save_as_->setIcon(QIcon(":/icons/document-save-as.png"));
			connect(action_save_as_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionSaveAs_triggered()));

			action_save_selection_as_->setIcon(QIcon(":/icons/document-save-as.png"));
			action_save_selection_as_->setShortcut(pv::util::key_sequence(Qt::CTRL, Qt::Key_R));
			connect(action_save_selection_as_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionSaveSelectionAs_triggered()));

			connect(action_save_setup_, SIGNAL(triggered(bool)),
					this, SLOT(on_actionSaveSetup_triggered()));

			menu_file_export_ = new widgets::ExportMenu(this,
																			session.device_manager().context());
			connect(menu_file_export_, SIGNAL(format_selected(shared_ptr<sigrok::OutputFormat>)),
					this, SLOT(export_file(shared_ptr<sigrok::OutputFormat>)));

			menu_file_import_ = new widgets::ImportMenu(this,
																			session.device_manager().context());
			connect(menu_file_import_, SIGNAL(format_selected(shared_ptr<sigrok::InputFormat>)),
					this, SLOT(import_file(shared_ptr<sigrok::InputFormat>)));
			action_connect_->setToolTip(""); // Remove the tooltip
			// connect(action_connect_, SIGNAL(triggered(bool)),
			// 	this, SLOT(on_actionConnect_triggered()));

			// New view button
			menu_new_view_ = new QMenu();
			connect(menu_new_view_, SIGNAL(triggered(QAction *)),
					this, SLOT(on_actionNewView_triggered(QAction *)));

			for (int i = 1; i < views::ViewTypeCount; i++)
			{
				QAction *const action = menu_new_view_->addAction(QString());
				action->setData(QVariant::fromValue(i));
			}

			new_view_button_->setMenu(menu_new_view_);
			new_view_button_->setDefaultAction(action_new_view_);
			new_view_button_->setPopupMode(QToolButton::MenuButtonPopup);
			new_view_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

			// model_ button
			menu_model_view_ = new QMenu();
			action_single_ = menu_model_view_->addAction(QString());
			action_repeat_ = menu_model_view_->addAction(QString());
			action_single_->setIcon(QIcon(":/icons/Single.png"));
			action_repeat_->setIcon(QIcon(":/icons/Repeat.png"));

			connect(action_single_, SIGNAL(triggered(bool)), this, SLOT(on_actionSingle_triggered()));
			connect(action_repeat_, SIGNAL(triggered(bool)), this, SLOT(on_actionRepeat_triggered()));

			// help button menu
			menu_help_view_ = new QMenu();
			action_settings_ = menu_help_view_->addAction(QString());
			action_version_info_ = menu_help_view_->addAction(QString());
			// QAction *action_feedback = menu_help_view_->addAction(tr("feedback"));
			action_update_ = menu_help_view_->addAction(QString());
			// QAction *action_use_guide = menu_help_view_->addAction(tr("User Guide"));
			action_about_ = menu_help_view_->addAction(QString());
			connect(action_settings_, SIGNAL(triggered(bool)), this, SIGNAL(settings_triggered()));
			connect(action_version_info_, SIGNAL(triggered(bool)), this, SLOT(on_action_version_info_triggered()));
			// connect(action_feedback, SIGNAL(triggered(bool)), this, SLOT(on_action_feedback_triggered()));
			connect(action_update_, SIGNAL(triggered(bool)), this, SLOT(on_action_update_triggered()));
			// connect(action_use_guide, SIGNAL(triggered(bool)), this, SLOT(on_action_use_guide_triggered()));
			connect(action_about_, SIGNAL(triggered(bool)), this, SLOT(on_action_about_triggered()));
			// Open button
			vector<QAction *> open_actions;
			open_actions.push_back(action_open_);
			QAction *separator_o = new QAction(this);
			separator_o->setSeparator(true);
			open_actions.push_back(separator_o);
			open_actions.push_back(action_restore_setup_);

			// help button
			help_button_->setMenu(menu_help_view_);
			help_button_->setIcon(QIcon(":/icons/Help.png"));
			help_button_->setPopupMode(QToolButton::MenuButtonPopup);
			help_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			// connect(help_button_, &QToolButton::clicked, help_button_, &QToolButton::showMenu);

			widgets::ImportMenu *import_menu = new widgets::ImportMenu(this,
																	   session.device_manager().context(), open_actions);
			connect(import_menu, SIGNAL(format_selected(shared_ptr<sigrok::InputFormat>)),
					this, SLOT(import_file(shared_ptr<sigrok::InputFormat>)));

			// run stop button
			run_stop_button_->setIcon(QIcon(":/icons/Start.png"));
			run_stop_button_->setShortcut(QKeySequence(Qt::Key_S));
			run_stop_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			connect(run_stop_button_, SIGNAL(clicked()), this, SIGNAL(run_stop_button_clicked()));
			// model change button
			model_change_button_->setMenu(menu_model_view_);
			model_change_button_->setIcon(QIcon(":/icons/Single.png")); // default single model
			// model_change_button_->setDefaultAction(action_single_);
			model_change_button_->setPopupMode(QToolButton::MenuButtonPopup);
			model_change_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			// connect(model_change_button_, &QToolButton::clicked, model_change_button_, &QToolButton::showMenu);
			// open button
			open_button_->setMenu(import_menu);
			open_button_->setDefaultAction(action_open_);
			open_button_->setPopupMode(QToolButton::MenuButtonPopup);
			open_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			// Save button
			vector<QAction *> save_actions;
			save_actions.push_back(action_save_);
			save_actions.push_back(action_save_as_);
			save_actions.push_back(action_save_selection_as_);
			QAction *separator_s = new QAction(this);
			separator_s->setSeparator(true);
			save_actions.push_back(separator_s);
			save_actions.push_back(action_save_setup_);

			widgets::ExportMenu *export_menu = new widgets::ExportMenu(this,
																	   session.device_manager().context(), save_actions);
			connect(export_menu, SIGNAL(format_selected(shared_ptr<sigrok::OutputFormat>)),
					this, SLOT(export_file(shared_ptr<sigrok::OutputFormat>)));

			save_button_->setMenu(export_menu);
			save_button_->setDefaultAction(action_save_);
			save_button_->setPopupMode(QToolButton::MenuButtonPopup);
			save_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

			/*
			 * Putting what was captured into a session of its own is not an
			 * export, so it sits above the formats, and only where there is
			 * something to copy: a file session has no capture of its own.
			 */
			action_copy_to_new_session_ =
				new QAction(tr("Export to a New Session"), this);
			connect(action_copy_to_new_session_, &QAction::triggered,
					this, &MainBar::copy_session_requested);
			connect(export_menu, &QMenu::aboutToShow, this, [this, export_menu]() {
				action_copy_to_new_session_->setVisible(!session_.using_file_device());
				action_copy_to_new_session_->setEnabled(
					session_.can_export_capture());
			});
			if (!export_menu->actions().isEmpty()) {
				export_menu->insertAction(export_menu->actions().first(),
					action_copy_to_new_session_);
				export_menu->insertSeparator(export_menu->actions().value(1));
			} else {
				export_menu->addAction(action_copy_to_new_session_);
			}

			// Device selector menu
			connect(&device_selector_, SIGNAL(device_selected()),
					this, SLOT(on_device_selected()));

			/* Rescanning is the same as hot-plug detection, so route the request
			 * through the same signal. Queued so that the device manager is only
			 * rebuilt once the popup menu is gone. */
			connect(&device_selector_, SIGNAL(refresh_requested()),
					this, SIGNAL(device_attached()), Qt::QueuedConnection);

			/* Opening a board in a session of its own is the window's
			 * business: the bar only passes the request on. */
			connect(&device_selector_, &widgets::DeviceToolButton::device_new_session_requested,
					this, &MainBar::device_new_session_requested);

			/* A change that does not involve the device in use only needs the
			 * list rebuilt. Queued: the scan may not run on the watcher thread. */
			connect(this, SIGNAL(refresh_device_list()),
					this, SLOT(on_refresh_device_list()), Qt::QueuedConnection);

			// Setup the decoder button
#ifdef ENABLE_DECODE
			add_decoder_button_->setIcon(QIcon(":/icons/add-decoder.png"));
			add_decoder_button_->setPopupMode(QToolButton::InstantPopup);
			/* Stays pressed while the decoder dock it opens is up, the way
			 * the cursor button does for its own. */
			add_decoder_button_->setCheckable(true);
			add_decoder_button_->setShortcut(QKeySequence(Qt::Key_D));
			add_decoder_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

			connect(add_decoder_button_, SIGNAL(clicked()),
					this, SLOT(on_add_decoder_clicked()));
#endif

			// Setup the math signal button
			// add_math_signal_button_->setIcon(QIcon(":/icons/add-math-signal.svg"));
			// add_math_signal_button_->setPopupMode(QToolButton::InstantPopup);
			// add_math_signal_button_->setToolTip(tr("Add math signal"));
			// add_math_signal_button_->setShortcut(QKeySequence(Qt::Key_M));

			// connect(add_math_signal_button_, SIGNAL(clicked()),
			// 	this, SLOT(on_add_math_signal_clicked()));

			connect(&sample_count_, SIGNAL(value_changed()),
					this, SLOT(on_sample_count_changed()));
			connect(&sample_rate_, SIGNAL(value_changed()),
					this, SLOT(on_sample_rate_changed()));

			sample_count_.show_min_max_step(0, UINT64_MAX, 1);

			set_capture_state(pv::Session::Stopped);

			// configure_button_.setToolTip(tr("Configure Device"));
			// configure_button_.setIcon(QIcon::fromTheme("preferences-system",
			// 	QIcon(":/icons/preferences-system.png")));

			channels_button_.setIcon(QIcon(":/icons/channels.svg"));
			// channels_button_.set_text_style(tr("Option"), Qt::ToolButtonTextUnderIcon);
			channels_button_.setToolButtonStyle((Qt::ToolButtonStyle)Qt::ToolButtonTextUnderIcon);

			// run_stop_button_->setIcon(QIcon(":/icons/add-decoder.svg"));
			// 	run_stop_button_->setShortcut(QKeySequence(Qt::Key_S));
			// 	run_stop_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			// 	run_stop_button_->setText(tr("Start"));
			// connect(run_stop_button_, SIGNAL(clicked()), this, SIGNAL(run_stop_button_clicked()));
			channels_button_ex_->setIcon(QIcon(":/icons/channels.png"));
			channels_button_ex_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			channels_button_ex_->setToolButtonStyle((Qt::ToolButtonStyle)Qt::ToolButtonTextUnderIcon);
			connect(channels_button_ex_, SIGNAL(clicked()), this, SLOT(channels_button_ex_clicked()));
			add_toolbar_widgets();

			sample_count_.installEventFilter(this);
			sample_rate_.installEventFilter(this);

			// Setup session_ events
			connect(&session_, SIGNAL(capture_state_changed(int)),
					this, SLOT(on_capture_state_changed(int)));
			connect(&session, SIGNAL(device_changed()),
					this, SLOT(on_device_changed()));

			/* The watcher reads this from its own thread. Seeded here as well:
			 * the page can already hold a board by the time the bar exists. */
			set_device_identity(
				widgets::DeviceToolButton::device_identity(session_.device()));

			update_device_list();
			is_rate_setting_default_ = false;
			is_count_setting_default_ = false;
			retranslate_ui();
			update_device_status_icon();
			update_upgrade_action();
		}

		void MainBar::retranslate_ui()
	{
		/* The base class owns the zoom and cursor entries. */
		StandardBar::retranslate_ui();

		action_new_view_->setText(tr("Decoding Result"));
		action_open_->setText(tr("&Open..."));
		action_restore_setup_->setText(tr("Restore Session Setu&p..."));
		action_save_->setText(tr("Save"));
		action_save_as_->setText(tr("Save &As..."));
		action_save_selection_as_->setText(tr("Save Selected &Range As..."));
		action_save_setup_->setText(tr("Save Session Setu&p..."));
		action_connect_->setText(tr("&Connect to Device..."));

		if (menu_file_export_)
			menu_file_export_->setTitle(tr("&Export"));
		if (menu_file_import_)
			menu_file_import_->setTitle(tr("&Import"));

		/* The mask is built once and keeps its words; they are set again
		 * here so that a language change reaches it too. */
		if (offline_mask_label_)
			offline_mask_label_->setText(offline_mask_text());

		/* New view entries are created empty and named here, so that
		 * they follow the language as well. */
		for (QAction *action : menu_new_view_->actions()) {
			const int type = action->data().toInt();
			if (type > 0 && type < views::ViewTypeCount)
				action->setText(QCoreApplication::translate(
					"pv::views", views::ViewTypeNames[type]));
		}

		action_single_->setText(tr("Single"));
		action_repeat_->setText(tr("Repeat"));
		action_settings_->setText(tr("Settings"));
		action_version_info_->setText(tr("Version Info"));
		action_update_->setText(tr("FW Update"));
		action_about_->setText(tr("About"));

		help_button_->setText(tr("Help"));
		model_change_button_->setText(tr("Model"));
		open_button_->setText(tr("Open"));
		save_button_->setText(tr("Save"));
#ifdef ENABLE_DECODE
		add_decoder_button_->setToolTip(tr("Add protocol decoder"));
		add_decoder_button_->setText(tr("Decoder"));
#endif
		channels_button_.setToolTip(tr("Configure Channels"));
		channels_button_.setText(tr("Option"));
		channels_button_ex_->setToolTip(tr("Configure Channels"));
		channels_button_ex_->setText(tr("Option"));

		/* Both of these carry the state of the device and of the
		 * acquisition, so ask the code that set them for the text. */
		update_device_status_icon();
		update_runstop_status(session_.get_capture_state());
	}

	void MainBar::changeEvent(QEvent *event)
	{
		/* Not passed on to the base class: it would only translate its own
		 * entries, which retranslate_ui() has already done. */
		if (event->type() == QEvent::LanguageChange)
			retranslate_ui();

		QToolBar::changeEvent(event);
	}

	MainBar::~MainBar()
		{
			/*
			 * Hot-plug belongs to the bar that started the watcher. Another
			 * session's bar being destroyed must not take it down: clearing
			 * the shared instance here would leave the callback with
			 * nowhere to report to, and boards would stop being noticed.
			 */
			/* A page closed while it was capturing must not leave the count up. */
			if (capture_running_)
			{
				capture_running_ = false;

				std::lock_guard<std::mutex> lock(s_capture_mutex);
				--s_captures_running;

				const QString id = get_device_identity();
				if (!id.isEmpty())
					s_capturing_identities.remove(id);
			}

			const bool owns_listener = (s_mainbar_instance == this);
			if (owns_listener)
			{
				s_mainbar_instance = nullptr;
			}

			if (channels_)
			{
				delete channels_;
				channels_ = nullptr;
			}

			if (device_options_)
			{
				delete device_options_;
				device_options_ = nullptr;
			}

			/* Stop the hot-plug thread */
			if (t_hot_plug.joinable())
			{
				hot_plug_listen_ = false;
				t_hot_plug.join();
			}

			if (owns_listener)
			{
#ifdef _WIN32
				/* Release the CH375DLL resources */
				ch375_cleanup();
#endif
			}
			commit_channel_number(0);
		}

		void MainBar::set_vendorName(QString vendorName)
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			m_vendorName = vendorName;
		}

		QString MainBar::get_vendorName()
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			return m_vendorName;
		}

		void MainBar::set_device_driver(QString driver)
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			m_device_driver = driver;
		}

		QString MainBar::get_device_driver()
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			return m_device_driver;
		}

		void MainBar::set_device_identity(QString identity)
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			m_device_identity = identity;
		}

		QString MainBar::get_device_identity()
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			return m_device_identity;
		}

		QString MainBar::offline_mask_text() const
		{
			return mask_html(tr("Device offline"),
				tr("Waiting for the capture in the other session to end"));
		}

		QString MainBar::initializing_mask_text() const
		{
			return mask_html(tr("Initializing device"),
				tr("Waiting for the board to come back"));
		}

		QString MainBar::upgrade_mask_text(int percent) const
		{
			/* The number is what the eye has to land on: it is the only thing
			 * that moves while the image is written, and it is read from a
			 * step away. */
			return QString("<div style=\"text-align:center;\">"
				"<span style=\"font-size:14pt;font-weight:600;\">%1</span>"
				"<br/><br/>"
				"<span style=\"font-size:28pt;font-weight:600;\">%2%</span>"
				"<br/><br/>%3</div>").arg(tr("Writing firmware")).arg(percent)
				.arg(tr("Do not disconnect the USB connection"));
		}

		bool MainBar::is_device_offline() const
		{
			return device_offline_;
		}

		/*
		 * The mask is put up and taken down in one place each. The kind says
		 * why it is there, and every caller that stops needing it goes through
		 * hide_mask(): a mask left standing because another caller had meanwhile
		 * changed the state is what kept a finished upgrade under "writing
		 * firmware" for good.
		 */
		void MainBar::show_mask(MaskKind kind, const QString &html, QWidget *page)
		{
			device_offline_ = true;
			mask_kind_ = kind;

			/* A mask already up stays on the page it was put on: taking it
			 * down or re-texting it happens without a page to hand over, and
			 * falling back to the window put it over the whole program. */
			QWidget *const frame = page ? page :
				(offline_mask_ ? offline_mask_->parentWidget() : window());
			if (!frame)
				return;

			if (offline_mask_ && offline_mask_page_ != frame)
			{
				offline_mask_->setParent(frame);
				offline_mask_page_ = frame;
				frame->installEventFilter(this);
			}

			if (!offline_mask_)
			{
				QWidget *const mask = new QWidget(frame);
				mask->setAutoFillBackground(true);

				QVBoxLayout *const layout = new QVBoxLayout(mask);
				layout->setContentsMargins(24, 24, 24, 24);
				QLabel *const label = new QLabel(mask);
				label->setAlignment(Qt::AlignCenter);
				label->setWordWrap(true);
				label->setText(html);
				layout->addWidget(label);

				/* A shade of the window colour: the mask follows a light and a dark
				 * theme without a colour of its own. */
				QPalette pal = mask->palette();
				QColor shade = pal.color(QPalette::Window);
				shade.setAlpha(210);
				pal.setColor(QPalette::Window, shade);
				mask->setPalette(pal);

				frame->installEventFilter(this);
				offline_mask_ = mask;
				offline_mask_label_ = label;
				offline_mask_page_ = frame;
			}

			if (offline_mask_label_)
				offline_mask_label_->setText(html);

			/* Eats the clicks: nothing on a page whose board is gone works. */
			offline_mask_->setGeometry(frame->rect());
			offline_mask_->show();
			offline_mask_->raise();
		}

		void MainBar::hide_mask()
		{
			device_offline_ = false;
			mask_kind_ = MaskKind::None;
			mask_busy_ = false;
			upgrade_mask_up_ = false;

			if (offline_mask_)
				offline_mask_->hide();
		}

		void MainBar::set_device_upgrading(bool upgrading, QWidget *page)
		{
			if (!upgrading)
			{
				/* Idempotent, and it does not ask the kind first: a mask put up
				 * for an upgrade comes down here even if something else has
				 * claimed the state since. */
				if (!upgrade_mask_up_ && mask_kind_ != MaskKind::Upgrade)
					return;

				upgrade_writing_ = false;
				upgrade_percent_ = -1;
				hide_mask();
				return;
			}

			if (upgrade_mask_up_)
				return;

			/* Nothing on the page can be used while the board is being
			 * written: the mask is the offline one, with its own text. */
			upgrade_mask_up_ = true;
			upgrade_writing_ = true;
			show_mask(MaskKind::Upgrade, upgrade_mask_text(0), page);
		}

		void MainBar::set_upgrade_progress(int percent)
		{
			if (!upgrade_mask_up_ || !offline_mask_label_)
				return;

			offline_mask_label_->setText(upgrade_mask_text(percent));
		}

		void MainBar::set_device_busy(bool busy, QWidget *page)
		{
			/* The upgrade owns the mask while it runs: a pass that finds the
			 * board in its bootloader must not turn the progress away. */
			if (mask_kind_ == MaskKind::Upgrade)
				return;

			/* A page waiting for its board says so already, and that is more
			 * than "a scan is running" - the window takes it down. */
			if (mask_kind_ == MaskKind::Initializing)
				return;

			if (busy == mask_busy_)
				return;
			mask_busy_ = busy;

			if (busy)
			{
				/* The mask is what a scan looks like from here: it eats the
				 * input for as long as it runs, and says why. */
				show_mask(MaskKind::Busy,
					mask_html(tr("Reinitializing devices"),
						tr("The board is being re-opened")), page);
			}
			else if (offline_mask_)
			{
				/* Off, but the page is still masked: what it says now is
				 * the offline text, and the caller says when that goes. */
				show_mask(MaskKind::Offline, offline_mask_text(), page);
			}
		}

		void MainBar::set_device_initializing(bool initializing, QWidget *page)
		{
			if (mask_kind_ == MaskKind::Upgrade)
				return;

			if (initializing)
			{
				if (mask_kind_ == MaskKind::Initializing && offline_mask_)
					return;

				show_mask(MaskKind::Initializing, initializing_mask_text(), page);
				return;
			}

			/* Idempotent, and only its own mask: the window may have put the
			 * offline one up meanwhile. */
			if (mask_kind_ == MaskKind::Initializing)
				hide_mask();
		}

		void MainBar::set_device_offline(bool offline, QWidget *page)
		{
			if (!offline)
			{
				/* A running upgrade keeps the mask it put there: an image is
				 * being written, and a pass settling the list has nothing to
				 * say about that page until the board is back. */
				if (mask_kind_ == MaskKind::Upgrade)
					return;

				if (!device_offline_ && !offline_mask_)
					return;

				hide_mask();
				return;
			}

			if (mask_kind_ == MaskKind::Upgrade)
				return;

			/* A scan says more than "offline" does, and it is about to take
			 * its own mask down: leave it alone. A page waiting for its board
			 * says more as well. */
			if (mask_kind_ == MaskKind::Busy ||
				mask_kind_ == MaskKind::Initializing)
				return;

			/* Already masked as offline: nothing to do. */
			if (device_offline_ && mask_kind_ == MaskKind::Offline && offline_mask_)
				return;

			show_mask(MaskKind::Offline, offline_mask_text(), page);
		}

		uint16_t MainBar::get_channel_number() const
		{
			return channel_number_;
		}

		namespace {

		/*
		 * Whether the list already offers this board. Identity, not the
		 * pointer: until the session has taken the instance the scan made,
		 * it still holds the one that scan replaced - the same board, and
		 * adding it would put the board in the list twice.
		 */
		bool has_device(const list<shared_ptr<devices::Device>> &devs,
			const shared_ptr<devices::Device> &device)
		{
			const QString identity =
				widgets::DeviceToolButton::device_identity(device);

			for (const shared_ptr<devices::Device> &dev : devs)
			{
				if (dev == device)
					return true;

				if (!identity.isEmpty() &&
					widgets::DeviceToolButton::device_identity(dev) == identity)
					return true;
			}

			return false;
		}

		} // namespace

		void MainBar::rebuild_device_list(bool keep_label)
		{
			/* The manager the names are read through changes on every pass: a
			 * selector left pointing at the one before shows a freed board,
			 * or nothing at all. */
			device_selector_.update_device_mamager(session_.device_manager_ptr());

			DeviceManager &mgr = session_.device_manager();
			const shared_ptr<devices::Device> selected = session_.device();

			list<shared_ptr<devices::Device>> devs;
			copy(mgr.devices().begin(), mgr.devices().end(), back_inserter(devs));
			/* The board this page holds is in the menu whatever the scan said:
			 * it is the one the page is on. */
			if (selected && !has_device(devs, selected))
				devs.push_back(selected);

			if (keep_label)
				device_selector_.set_available_devices(devs);
			else
				device_selector_.set_device_list(devs, selected);
		}

		void MainBar::update_device_list()
		{
			rebuild_device_list();
			update_device_config_widgets();
		}

		void MainBar::refresh_device_selector()
		{
			rebuild_device_list();
			update_device_status_icon();
		}

		void MainBar::on_refresh_device_list()
		{
			DeviceManager *manager = session_.device_manager_ptr();
			if (!manager)
				return;

			/*
			 * Only the drivers that are not in use are scanned again. A change
			 * in one of them cannot have touched the analyzer the session
			 * holds, and rescanning its driver would close it - and stop a
			 * capture running on it - for nothing.
			 */
			const QString in_use = get_device_driver();
			const auto drivers = manager->context()->drivers();
			for (const auto &entry : drivers)
				if (in_use.isEmpty() ||
					QString::fromStdString(entry.first) != in_use)
					manager->driver_scan(entry.second,
						map<const sigrok::ConfigKey *, Glib::VariantBase>());

			update_device_list();
		}

		void MainBar::set_capture_state(pv::Session::capture_state state)
		{
			const bool running = (state != pv::Session::Stopped);
			bool resume = false;

			if (running != capture_running_)
			{
				capture_running_ = running;

				/* The count, the boards it is about and the flag that says a pass
				 * is owed are one reading to the watcher, so they change together. */
				std::lock_guard<std::mutex> lock(s_capture_mutex);
				if (running)
					++s_captures_running;
				else
					--s_captures_running;

				const QString id = get_device_identity();
				if (!id.isEmpty())
				{
					if (running)
						s_capturing_identities.insert(id);
					else
						s_capturing_identities.remove(id);
				}

				/* The last capture ended: the pass it held back is run now. */
				if (!running && s_captures_running <= 0 &&
					s_attach_deferred.exchange(false))
					resume = true;
			}

			if (resume)
			{
				set_device_offline(false);
				device_attached();
			}

			bool ui_enabled = (state == pv::Session::Stopped) ? true : false;

			device_selector_.setEnabled(ui_enabled);
			// configure_button_.setEnabled(ui_enabled);
			// channels_button_.setEnabled(ui_enabled);
			channels_button_ex_->setEnabled(ui_enabled);
			help_button_->setEnabled(ui_enabled);
			open_button_->setEnabled(ui_enabled);
			save_button_->setEnabled(ui_enabled);
			// Check if demo device - sample rate/count should stay disabled
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			shared_ptr<devices::HardwareDevice> hw_device =
				std::dynamic_pointer_cast<devices::HardwareDevice>(device);
			bool is_demo = false;
			if (hw_device) {
				string driver_name = hw_device->hardware_device()->driver()->name();
				is_demo = (driver_name == "demo");
			}
			if (is_demo) {
				sample_count_.setEnabled(false);
				sample_rate_.setEnabled(false);
			} else {
				sample_count_.setEnabled(ui_enabled);
				sample_rate_.setEnabled(ui_enabled);
			}
			if (model_change_button_)
				model_change_button_->setEnabled(ui_enabled);
			/* Its menu asks the same question when it opens; this is what makes
			 * the entry gray for a capture that starts while it is shown. */
			if (action_copy_to_new_session_)
				action_copy_to_new_session_->setEnabled(
					session_.can_export_capture());
		}

		void MainBar::reset_device_selector()
		{
			device_selector_.reset();
		}

		void MainBar::set_device_selector_name(QString device_selector)
		{
			device_selector_.set_device_list_name(device_selector);
		}

		void MainBar::set_device_selector_role(bool new_session_allowed,
			bool single_device_only)
		{
			device_selector_.set_device_selector_role(new_session_allowed,
				single_device_only);
		}

		void MainBar::set_busy_devices(const QSet<QString> &busy,
			const QString &own)
		{
			/* The watcher needs the whole set: a chain that went away is only a
			 * board it can point at if it knows whose board it was. */
			{
				std::lock_guard<std::mutex> lock(vendorName_mutex);
				m_busy_identities = busy;
			}

			device_selector_.set_busy_devices(busy, own);
		}

		QSet<QString> MainBar::get_busy_identities()
		{
			std::lock_guard<std::mutex> lock(vendorName_mutex);
			return m_busy_identities;
		}

		QString MainBar::last_save_directory()
		{
			return QSettings().value(SettingSaveDirectory).toString();
		}

		void MainBar::set_decoder_button_checked(bool on)
		{
			if (add_decoder_button_)
				add_decoder_button_->setChecked(on);
		}

		void MainBar::remember_save_directory(const QString &path)
		{
			QSettings().setValue(SettingSaveDirectory,
				QFileInfo(path).absolutePath());
		}

		void MainBar::set_capture_buttons_visible(bool mode, bool run)
		{
			/*
			 * The action as well as the widget: a tool bar holds what
			 * addWidget() hands back, and that action shows its widget
			 * again whenever it is shown itself.
			 */
			if (model_change_button_)
				model_change_button_->setVisible(mode);
			if (model_change_action_)
				model_change_action_->setVisible(mode);
			if (run_stop_button_)
				run_stop_button_->setVisible(run);
			if (run_stop_action_)
				run_stop_action_->setVisible(run);
		}

		void MainBar::set_device_selector_visible(bool visible)
		{
			/* Both the widget and the action that holds it: hiding only the
			 * widget leaves a gap in the tool bar where it used to be. */
			device_selector_.setVisible(visible);
			if (device_selector_action_)
				device_selector_action_->setVisible(visible);
		}

		void MainBar::update_device_manager(DeviceManager *manager)
		{
			/*
			 * The list the selector holds belongs to the manager that is
			 * gone, and the board this session shows is about to be taken
			 * back from this new manager. Until that happens the session
			 * still holds the instance the scan freed, and reading the list
			 * now would put that board in twice - once from the manager,
			 * once from the session. Taking the board back, or the detach
			 * that follows when it cannot, is what rebuilds the list.
			 */
			device_selector_.update_device_mamager(manager);
		}

		QAction *MainBar::action_open() const
		{
			return action_open_;
		}

		QAction *MainBar::action_save() const
		{
			return action_save_;
		}

		QAction *MainBar::action_save_as() const
		{
			return action_save_as_;
		}

		QAction *MainBar::action_save_selection_as() const
		{
			return action_save_selection_as_;
		}

		QAction *MainBar::action_connect() const
		{
			return action_connect_;
		}

		void MainBar::update_sample_rate_selector()
		{
			Glib::VariantContainerBase gvar_dict;
			GVariant *gvar_list;
			const uint64_t *elements = nullptr;
			gsize num_elements;
			map<const ConfigKey *, set<Capability>> keys;
			if (updating_sample_rate_)
			{
				sample_rate_.show_none();
				return;
			}

			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			assert(!updating_sample_rate_);
			updating_sample_rate_ = true;

			const shared_ptr<sigrok::Device> sr_dev = device->device();

			sample_rate_.allow_user_entered_values(false);
			if (sr_dev->config_check(ConfigKey::EXTERNAL_CLOCK, Capability::GET))
			{
				try
				{
					auto gvar = sr_dev->config_get(ConfigKey::EXTERNAL_CLOCK);
					if (gvar.gobj())
					{
						bool value = Glib::VariantBase::cast_dynamic<Glib::Variant<bool>>(
										 gvar)
										 .get();
						sample_rate_.allow_user_entered_values(value);
					}
				}
				catch (Error &error)
				{
					// Do nothing
				}
			}

			if (sr_dev->config_check(ConfigKey::SAMPLERATE, Capability::LIST))
			{
				try
				{
					gvar_dict = sr_dev->config_list(ConfigKey::SAMPLERATE);
				}
				catch (Error &error)
				{
					qDebug() << tr("Failed to get sample rate list:") << error.what();
				}
			}
			else
			{
				sample_rate_.show_none();
				updating_sample_rate_ = false;
				return;
			}
			if ((gvar_list = g_variant_lookup_value(gvar_dict.gobj(),
													"samplerate-steps", G_VARIANT_TYPE("at"))))
			{
				elements = (const uint64_t *)g_variant_get_fixed_array(
					gvar_list, &num_elements, sizeof(uint64_t));

				uint64_t minRate = elements[0];
				uint64_t maxRate = elements[1];
				uint64_t stepRate = elements[2];

				g_variant_unref(gvar_list);

				assert(minRate > 0);
				assert(maxRate > 0);
				assert(maxRate > minRate);
				assert(stepRate > 0);

				minRate = max(minRate, MinSampleRate);
				maxRate = min(maxRate, max_sample_rate_);
				if (stepRate == 1)
					sample_rate_.show_125_list(minRate, maxRate);
				else
				{
					// When the step is not 1, we cam't make a 1-2-5-10
					// list of sample rates, because we may not be able to
					// make round numbers. Therefore in this case, show a
					// spin box.
					sample_rate_.show_min_max_step(minRate, maxRate, stepRate);
				}
			}
			else if ((gvar_list = g_variant_lookup_value(gvar_dict.gobj(),
														 "samplerates", G_VARIANT_TYPE("at"))))
			{
				elements = (const uint64_t *)g_variant_get_fixed_array(
					gvar_list, &num_elements, sizeof(uint64_t));
				QString vendor = QString::fromStdString(sr_dev->vendor());
				if (vendor == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)])
				{
					// CH569: the number of sample rates depends on the channel count
					switch (channel_number_)
					{
					case 2:
						num_elements = 18;
						break;
					case 4:
						num_elements = 17;
						break;
					case 8:
						num_elements = 16;
						break;
					default:
						num_elements = 15;
						break;
					}
				}
				/*
				 * The number of entries is whatever the driver lists: the
				 * table follows the sample width, and the width follows
				 * the channel count. A fixed number ran past the end of
				 * the narrower table, so the list held rates the board
				 * does not have and lost the ones it does.
				 */
				sample_rate_.show_list(elements, num_elements);
				// if (is_rate_setting_default_){
				// 	sample_rate_.set_current_index(sample_rate_default_index_);
				// }
				g_variant_unref(gvar_list);
			}
			updating_sample_rate_ = false;

			/*
			 * A rebuilt list starts at its first entry, not at the rate the device
			 * runs at, so the value goes back in before it is committed - and this
			 * runs every time the options are closed.
			 */
			update_sample_rate_selector_value();
			commit_sample_rate();
		}

		void MainBar::update_sample_rate_selector_value()
		{
			if (updating_sample_rate_)
				return;

			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			try
			{
				auto gvar = device->device()->config_get(ConfigKey::SAMPLERATE);
				uint64_t samplerate =
					Glib::VariantBase::cast_dynamic<Glib::Variant<guint64>>(gvar).get();
				assert(!updating_sample_rate_);
				updating_sample_rate_ = true;
				sample_rate_.set_value(samplerate);
				updating_sample_rate_ = false;
			}
			catch (Error &error)
			{
				// Do nothing
				qDebug() << tr("Failed to get sample rate value:") << error.what();	
			}
		}

		void MainBar::update_sample_count_selector()
		{
			if (updating_sample_count_)
				return;

			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (sr_dev != NULL)
			{
			}
			else
			{
				return;
			}
			assert(!updating_sample_count_);
			updating_sample_count_ = true;

			if (!sample_count_supported_)
			{
				sample_count_.show_none();
				updating_sample_count_ = false;
				return;
			}

			// static uint64_t sample_count = ((double)sample_count_.get_current_data(sample_count_.get_current_index())) / 10000.00 * (double)sample_rate_.get_current_data(sample_rate_.get_current_index());
			static uint64_t sample_count = 0;
			uint64_t min_sample_count = 0;
			uint64_t max_sample_count = MaxSampleCount;
			bool default_count_set = false;

			if (sample_count == 0)
			{
				sample_count = DefaultSampleCount;
				default_count_set = true;
			}

			if (sr_dev->config_check(ConfigKey::LIMIT_SAMPLES, Capability::LIST))
			{
				try
				{
					auto gvar = sr_dev->config_list(ConfigKey::LIMIT_SAMPLES);
					if (gvar.gobj())
						g_variant_get(gvar.gobj(), "(tt)",
									  &min_sample_count, &max_sample_count);
				}
				catch (Error &error)
				{
					qDebug() << tr("Failed to get sample limit list:") << error.what();
				}
			}
			min_sample_count = min(max(min_sample_count, MinSampleCount),
								   max_sample_count);

			// Max data: 18,000,000,000 bytes   Min data: 100 bytes
			// max_time = 18000000000 / sample_rate_.get_current_data(sample_rate_.get_current_index())
			// min_time = 0.0001S = 100us
			uint64_t sampleRate = sample_rate_.get_current_data(sample_rate_.get_current_index());
			if (sampleRate < 1ULL || sampleRate > 1000005000ULL)
			{
				sampleRate = 25000;
			}
			double max_time = 18000000000.00 / ((double)sampleRate * (double)(channel_number_ / 8.00));
			shared_ptr<devices::HardwareDevice> hw_dev_for_min_time =
				std::dynamic_pointer_cast<devices::HardwareDevice>(device);
			bool is_demo_min_time = false;
			if (hw_dev_for_min_time) {
				string driver_name = hw_dev_for_min_time->hardware_device()->driver()->name();
				is_demo_min_time = (driver_name == "demo");
			}
			double min_time = is_demo_min_time ? 0.004 : 0.01;
			sample_count_.show_125_list(min_time * 10000, max_time * 10000); // commit sample count

			if (sr_dev->config_check(ConfigKey::LIMIT_SAMPLES, Capability::GET))
			{
				auto gvar = sr_dev->config_get(ConfigKey::LIMIT_SAMPLES);
				sample_count = g_variant_get_uint64(gvar.gobj());
			}
			if (sample_count > 0)
			{
				sample_count_.set_time_value(sample_count, sampleRate);
				if (sample_count != ((double)sample_count_.get_current_data(sample_count_.get_current_index())) / 10000.00 * (double)sample_rate_.get_current_data(sample_rate_.get_current_index()))
				{
					commit_sample_count();
				}
			}
			updating_sample_count_ = false;

			// If we show the default rate then make sure the device uses the same
			if (default_count_set)
			{
				commit_sample_count();
			}
		}

		void MainBar::on_update_max_sample_rate(uint64_t maxSampleRate)
		{
			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			QString vendor = QString::fromStdString(device->device()->vendor());
			QString model = QString::fromStdString(device->device()->model());
			bool is_ch32h417 = (vendor == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)] || model == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)]);

			/*
			 * CH32H417: nothing to work out. The channel count has already
			 * been handed to the board by the channel popup, and the board
			 * picked its rate table from that. Both lists are read back
			 * from the board, so they say what it can do now.
			 */
			if (is_ch32h417)
			{
				update_sample_rate_selector();
				update_sample_count_selector();
				return;
			}

			// CH569 and other devices
			uint16_t channel_number = 16;
			uint64_t sample_rate_channel_max = 0;
			max_sample_rate_ = maxSampleRate;
			if (max_sample_rate_ == SR_GHZ(1))
			{
				channel_number = 2;
			}
			else if (max_sample_rate_ == SR_MHZ(500))
			{
				channel_number = 4;
			}
			else if (max_sample_rate_ == SR_MHZ(250))
			{
				channel_number = 8;
			}
			else
			{
				channel_number = 16;
			}
			if (channel_number == channel_number_)
			{
				return;
			}
			// commit channel number
			commit_channel_number(channel_number);
			channel_number_ = channel_number;
			switch (channel_number)
			{
			case 2:
				sample_rate_channel_max = SR_GHZ(1);
				break;
			case 4:
				sample_rate_channel_max = SR_MHZ(500);
				break;
			case 8:
				sample_rate_channel_max = SR_MHZ(250);
				break;
			default:
				sample_rate_channel_max = SR_MHZ(125);
				break;
			}

			if (sample_rate_value_ > sample_rate_channel_max)
			{
				update_sample_rate_selector();
				update_sample_count_selector();
			}
			else
			{
				renew_samplecount_samplerate(sample_rate_value_, sample_rate_channel_max);
			}
		}

		void MainBar::on_update_channel_count(uint16_t channelCount)
		{
			// CH32H417 specific: handle a channel count change
			// Only 8 or 16 channels are accepted
			if (channelCount != 8 && channelCount != 16)
			{
				return;
			}

			/*
			 * Handed over even when the page already holds this count:
			 * this runs whenever the page is rebuilt, and a rebuilt page
			 * may sit on a board the session has only just taken over. A
			 * fresh instance starts at the default count - and so at the
			 * narrow rate table that goes with it - whatever the page
			 * shows, and the rates of the wider table would be refused.
			 */
			commit_channel_number(channelCount);
			channel_number_ = channelCount;

			update_sample_count_selector();
			set_setting_default();
		}

		void MainBar::on_update_threshold_value_1(float thresholdValue)
		{
			commit_threshold_value(1, thresholdValue);
			thresholdValue_1_ = thresholdValue;
			set_setting_default();
		}

		void MainBar::on_update_threshold_value_2(float thresholdValue)
		{
			commit_threshold_value(2, thresholdValue);
			thresholdValue_2_ = thresholdValue;
			set_setting_default();
		}

		void MainBar::on_update_threshold_value_ch32h417(float thresholdValue)
		{
			// Only store the threshold here; the hardware is updated by on_threshold_value_raw_changed
			thresholdValue_ch32h417_ = thresholdValue;
			// Write it out now: a board that is pulled out never reaches the
			// save on exit, and the next one comes up with the old level.
			set_setting_default();
		}

		void MainBar::on_update_pre_trigger(uint8_t value)
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;

			const shared_ptr<sigrok::Device> sr_dev = device->device();

			pre_trigger_ = value;

			try
			{
				sr_dev->config_set(ConfigKey::CAPTURE_RATIO,
								   Glib::Variant<guint64>::create(value));

				qDebug() << "Device pre-trigger set to:" << value;
			}
			catch (Error &error)
			{
				qDebug() << tr("Failed to configure pre-trigger:") << error.what();
			}
			set_setting_default();
		}

		void MainBar::on_mode_changed(int mode)
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;

			const shared_ptr<sigrok::Device> sr_dev = device->device();

			// Mode names: 0 = Logic Analyzer, 1 = ADC
			QString mode_str = (mode == 0) ? "Logic Analyzer" : "ADC";

			try
			{
				sr_dev->config_set(ConfigKey::PATTERN_MODE,
								   Glib::Variant<Glib::ustring>::create(mode_str.toStdString()));

				qDebug() << "Device mode set to:" << mode_str;

				// Update the sample rate list after a mode switch
				update_sample_rate_selector();
				update_sample_rate_selector_value();
			}
			catch (Error &error)
			{
				qDebug() << tr("Failed to configure pattern mode:") << error.what();
			}
		}

		void MainBar::on_adc_channel_changed(int channel)
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;

			const shared_ptr<sigrok::Device> sr_dev = device->device();

			// ADC channel: 0 = Channel 0, 1 = Channel 1
			try
			{
				sr_dev->config_set(ConfigKey::ADC_CHANNEL,
								   Glib::Variant<guint64>::create((uint64_t)channel));
				qDebug() << "ADC channel set to:" << channel;
			}
			catch (Error &error)
			{
				qDebug() << tr("Failed to configure ADC channel:") << error.what();
			}
		}

		void MainBar::on_adc_precision_changed(int precision)
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;

			const shared_ptr<sigrok::Device> sr_dev = device->device();

			// ADC resolution: 0 = 8-bit, 1 = 10-bit
			uint64_t precision_value = (precision == 0) ? 8 : 10;

			try
			{
				sr_dev->config_set(ConfigKey::ADC_PRECISION,
								   Glib::Variant<guint64>::create(precision_value));
				qDebug() << "ADC precision set to:" << precision_value << "bit";
			}
			catch (Error &error)
			{
				qDebug() << tr("Failed to configure ADC precision:") << error.what();
			}
		}

		void MainBar::on_threshold_value_raw_changed(double value)
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			try
			{
				sr_dev->config_set(ConfigKey::VOLTAGE_THRESHOLD,
					Glib::Variant<std::tuple<double, double>>::create(
						std::make_tuple(value, value)));
				qDebug() << "VOLTAGE_THRESHOLD set to:" << value;
			}
			catch (Error &error)
			{
				qDebug() << "Failed to set VOLTAGE_THRESHOLD:" << error.what();
			}
		}

		void MainBar::on_channel_selection_changed()
		{
			/*
			 * Which of the modes is showing now: the board is told a
			 * channel count, but the choice the page shows is the one that
			 * has to come back after the page is built again.
			 */
			if (channels_) {
				for (int i = 0; i < 4; i++)
					if (channels_->radioChannels[i] &&
						channels_->radioChannels[i]->isChecked()) {
						channel_mode_index_ = i;
						break;
					}
			}

			if (view_) {
				view_->update_signals_height();
			}
		}

		void MainBar::update_device_status_icon()
		{
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device) {
				device_status_icon_->setIcon(QIcon(":/icons/Demo.png"));
				device_status_icon_->setText(tr("Demo"));
				device_status_icon_->setToolTip(tr("Device Status: Demo"));
				return;
			}

			if (session_.using_file_device()) {
				device_status_icon_->setIcon(QIcon(":/icons/File.png"));
				device_status_icon_->setText(tr("File"));
				device_status_icon_->setToolTip(tr("Device Status: File"));
				return;
			}

			shared_ptr<devices::HardwareDevice> hw_device =
				std::dynamic_pointer_cast<devices::HardwareDevice>(device);

			if (hw_device) {
				string driver_name = hw_device->hardware_device()->driver()->name();
				if (driver_name == "demo") {
					device_status_icon_->setIcon(QIcon(":/icons/Demo.png"));
					device_status_icon_->setText(tr("Demo"));
					device_status_icon_->setToolTip(tr("Device Status: Demo"));
					return;
				}
			}

			const shared_ptr<sigrok::Device> sr_dev = device->device();
			QString vendor = QString::fromStdString(sr_dev->vendor());

			/*
			 * Ask the device which USB version it is on: every real device
			 * answers this, so the icon follows the link in use rather than the
			 * driver behind it.
			 */
			bool have_usb_version = false;
			uint64_t usb_version = 0;
			if (sr_dev->config_check(ConfigKey::USB_VERSION, Capability::GET)) {
				try {
					auto gvar = sr_dev->config_get(ConfigKey::USB_VERSION);
					if (gvar.gobj()) {
						usb_version = Glib::VariantBase::cast_dynamic<Glib::Variant<guint64>>(gvar).get();
						have_usb_version = true;
					}
				} catch (Error &error) {
					// Default to USB 3.0
					have_usb_version = true;
					usb_version = 3;
				}
			}

			if (have_usb_version) {
				if (usb_version == 2) {
					device_status_icon_->setIcon(QIcon(":/icons/USB20.png"));
					device_status_icon_->setText(tr("USB2.0"));
					device_status_icon_->setToolTip(tr("Device Status: USB 2.0"));
				} else {
					device_status_icon_->setIcon(QIcon(":/icons/USB30.png"));
					device_status_icon_->setText(tr("USB3.0"));
					device_status_icon_->setToolTip(tr("Device Status: USB 3.0"));
				}
			} else if (vendor == "USB3.0(CH569)") {
				device_status_icon_->setIcon(QIcon(":/icons/USB30.png"));
				device_status_icon_->setText(tr("USB3.0"));
				device_status_icon_->setToolTip(tr("Device Status: USB 3.0"));
			} else {
				device_status_icon_->setIcon(QIcon(":/icons/Demo.png"));
				device_status_icon_->setText(tr("Demo"));
				device_status_icon_->setToolTip(tr("Device Status: Demo"));
			}
		}

		void MainBar::renew_samplecount_samplerate(uint64_t samplerate, uint64_t samplerate_max)
		{
			uint16_t sampelrate_index = sample_rate_.get_current_index();
			uint16_t samplecout_index = sample_count_.get_current_index();
			const uint64_t *elements = nullptr;
			Glib::VariantContainerBase gvar_dict;
			GVariant *gvar_list;
			gsize num_elements;
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (sr_dev->config_check(ConfigKey::SAMPLERATE, Capability::LIST))
			{
				try
				{
					gvar_dict = sr_dev->config_list(ConfigKey::SAMPLERATE);
				}
				catch (Error &error)
				{
					qDebug() << tr("Failed to get sample rate list:") << error.what();
				}
			}
			else
			{
				sample_rate_.show_none();
				updating_sample_rate_ = false;
				return;
			}
			if ((gvar_list = g_variant_lookup_value(gvar_dict.gobj(),
													"samplerates", G_VARIANT_TYPE("at"))))
			{
				elements = (const uint64_t *)g_variant_get_fixed_array(
					gvar_list, &num_elements, sizeof(uint64_t));
				switch (channel_number_)
				{
				case 2:
					num_elements = 18;
					break;
				case 4:
					num_elements = 17;
					break;
				case 8:
					num_elements = 16;
					break;
				default:
					num_elements = 15;
					break;
				}
				sample_rate_.show_list(elements, num_elements);
				sample_rate_.set_current_index(sampelrate_index);
				sample_count_.set_current_index(samplecout_index);
			}
		}

		void MainBar::set_setting_default()
		{
			if (restoring_defaults_)
				return;

			/*
			 * Every board keeps its settings between runs now. The group
			 * carries the model and the port, so the values only ever go
			 * back to the board that wrote them. An open file has nothing
			 * to write.
			 */
			if (session_.using_file_device() || !session_.device())
				return;

			/*
			 * Dragging a slider calls this on every step, and every call
			 * wrote the whole settings file. Hold the write back a little:
			 * a drag is one change, not thirty.
			 */
			if (!setting_save_timer_) {
				setting_save_timer_ = new QTimer(this);
				setting_save_timer_->setSingleShot(true);
				connect(setting_save_timer_, &QTimer::timeout,
					this, &MainBar::write_setting_default);
			}
			setting_save_timer_->start(SettingSaveDelayMs);
		}

		void MainBar::write_setting_default()
		{
			if (restoring_defaults_)
				return;

			const shared_ptr<devices::Device> device = session_.device();
			if (!device || !device->device() || session_.using_file_device())
				return;

			/*
			 * The demo device stands in for a board that is not there, and
			 * writing for it would put its own model into the page's group -
			 * the board that comes back would then find settings that are
			 * not its own and start from the defaults.
			 */
			const shared_ptr<devices::HardwareDevice> hw_device =
				dynamic_pointer_cast<devices::HardwareDevice>(device);
			if (!hw_device ||
					hw_device->hardware_device()->driver()->name() == "demo")
				return;

			const QString vendorName = get_vendorName();
			const bool is_wch =
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)] ||
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)];

			QSettings settings;
			// Store the configuration
			settings.beginGroup(setting_group());

			/*
			 * Which board these belong to travels with them: the page may
			 * come up on another one - a session that was switched, or a
			 * board that was replaced - and nothing may be put back then.
			 * The serial number is what names a board; the port is only
			 * there for a board that reports the same one as every other.
			 */
			settings.setValue("model", QString::fromStdString(device->device()->model()));
			settings.setValue("serial_num", QString::fromStdString(device->device()->serial_number()));
			settings.setValue("connection_id", QString::fromStdString(device->device()->connection_id()));

			if (vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)])
			{
				/* This page's own, and the value last used anywhere stays
				 * what a page nobody has set up yet starts from. */
				settings.setValue("ThresholdLevel", thresholdValue_ch32h417_);
				GlobalSettings gs;
				gs.setValue(GlobalSettings::Key_ThresholdLevel, thresholdValue_ch32h417_);
			}
			else if (is_wch)
			{
				// CH569 keeps its original two-threshold settings
				settings.setValue("Threshold_bank_1", QString::number(thresholdValue_1_));
				settings.setValue("Threshold_bank_2", QString::number(thresholdValue_2_));
			}

			settings.setValue("sample_count_index", QString::number(sample_count_.get_current_index()));
			settings.setValue("sample_rate_index", QString::number(sample_rate_.get_current_index()));
			settings.setValue("pre_trigger", QString::number(pre_trigger_));

			/* The channel width and the mode are WCH hardware only. */
			if (is_wch)
			{
				settings.setValue("channel_number", QString::number(channel_number_));
				settings.setValue("channel_mode_index", QString::number(channel_mode_index_));
			}
			settings.endGroup();
		}

		void MainBar::get_decoder_list_default(QString decoder_name)
		{
			string String = decoder_name.toLocal8Bit().toStdString();
			std::istringstream iss(String);
			std::string word;
			decoder_list_default_.clear();
			while (iss >> word)
			{
				decoder_list_default_ << QString::fromLocal8Bit(word.data());
			}
		}

		void MainBar::save_setup(QSettings &settings) const
		{
			// Save sample count and sample rate values
			settings.setValue("sample_count", QVariant::fromValue(sample_count_value_));
			settings.setValue("sample_rate", QVariant::fromValue(sample_rate_value_));
		}

		void MainBar::restore_setup(QSettings &settings)
		{
			// Restore sample count and sample rate values
			if (settings.contains("sample_count")) {
				sample_count_value_ = settings.value("sample_count").value<uint64_t>();
			}
			if (settings.contains("sample_rate")) {
				sample_rate_value_ = settings.value("sample_rate").value<uint64_t>();
			}
		}

		QString MainBar::setting_group() const
		{
			/*
			 * The settings belong to the board rather than to the page: a page
			 * can be pointed at another board, and two boards take turns on
			 * one page, so a group per page would have them overwrite each
			 * other. A WeAct board carries a serial number of its own; every
			 * other board is told apart by the port it sits in.
			 */
			const shared_ptr<devices::Device> device = session_.device();
			const shared_ptr<sigrok::Device> sr_dev =
				device ? device->device() : nullptr;

			if (sr_dev) {
				QString board = QString::fromStdString(sr_dev->model());
				const QString sn =
					QString::fromStdString(sr_dev->serial_number());
				const QString port =
					QString::fromStdString(sr_dev->connection_id());

				if (board.contains(util::WeActV2ModelName) &&
						!sn.isEmpty() && sn != "0")
					board += "_" + sn;
				else if (!port.isEmpty())
					board += "_" + port;

				/* A group name is a key: the separators of a path cannot be
				 * in it. */
				for (QChar &ch : board)
					if (ch == '/' || ch == '\\' || ch == ' ' || ch == ':')
						ch = '_';

				if (!board.isEmpty())
					return "Settings/Board_" + board;
			}

			/* No board behind the page - a file, or one that did not open. */
			return QString("Settings/%1").arg(session_.id());
		}

		/*
		 * A page's settings belong to the board that wrote them. Sessions
		 * can be pointed at another board, and a board can be replaced by
		 * one of a different model, so the board has to be the same one
		 * before anything is put back. The serial number says which board
		 * it is; a board that reports the same serial as every other one
		 * of its kind falls back to the port it is plugged into.
		 */
		bool MainBar::setting_group_is_ours(QSettings &settings, const QString &group)
		{
			const shared_ptr<devices::Device> device = session_.device();
			if (!device || !device->device())
				return false;

			if (!settings.contains(group + "/model"))
			{
				/* Groups written before the model was stored came from a
				 * WCH board, and only those ever wrote one. */
				const QString vendorName = get_vendorName();
				return vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)] ||
					vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)];
			}

			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (settings.value(group + "/model").toString() !=
					QString::fromStdString(sr_dev->model()))
				return false;

			const QString saved_sn = settings.value(group + "/serial_num").toString();
			/*
			 * Only the WeAct Studio Logic Analyzer V2 gives every board a
			 * serial number of its own; every other board of this vendor
			 * may report one and the same, and then the port decides.
			 */
			const bool serial_is_an_id = QString::fromStdString(sr_dev->model())
				.contains(util::WeActV2ModelName);
			if (serial_is_an_id && !saved_sn.isEmpty() && saved_sn != "0")
				return saved_sn == QString::fromStdString(sr_dev->serial_number());

			return settings.value(group + "/connection_id").toString() ==
				QString::fromStdString(sr_dev->connection_id());
		}

		void MainBar::get_setting_default()
		{
			QSettings settings;
			QString group = setting_group();

			const QString vendorName = get_vendorName();
			const bool is_wch =
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)] ||
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)];

			/*
			 * Builds before this one wrote the settings per page. A board
			 * whose own group is empty takes over what its page still holds,
			 * so an installation does not come up with the defaults once.
			 */
			if (!settings.contains(group + "/sample_count_index")) {
				const QString page_group =
					QString("Settings/%1").arg(session_.id());
				if (page_group != group &&
						settings.contains(page_group + "/sample_count_index") &&
						setting_group_is_ours(settings, page_group)) {
					settings.beginGroup(page_group);
					const QStringList keys = settings.childKeys();
					settings.endGroup();
					for (const QString &key : keys)
						settings.setValue(group + "/" + key,
							settings.value(page_group + "/" + key));
				}
			}

			/*
			 * The first page also reads the group older versions kept for
			 * the whole program, so that an installation does not come up
			 * with the defaults once. Any other page starts from its own.
			 */
			if (!settings.contains(group + "/sample_count_index") &&
				session_.id() == 0 && is_wch)
				group = "Settings";

			if (settings.contains(group + "/sample_count_index") &&
				setting_group_is_ours(settings, group))
			{
				// CH569 threshold settings
				threshold_bank_1_default_ = settings.value(group + "/Threshold_bank_1").toFloat();
				threshold_bank_2_default_ = settings.value(group + "/Threshold_bank_2").toFloat();
				channel_number_default_ = settings.value(group + "/channel_number").toInt();
				sample_count_default_index_ = settings.value(group + "/sample_count_index").toUInt();
				sample_rate_default_index_ = settings.value(group + "/sample_rate_index").toUInt();
				is_repeat_acq_default_ = settings.value(group + "/repeat_acquisition").toInt();
				pre_trigger_default_ = settings.value(group + "/pre_trigger").toUInt();
				channel_mode_index_ = settings.value(group + "/channel_mode_index", 3).toInt();
				if (channel_mode_index_ < 0 || channel_mode_index_ > 3)
					channel_mode_index_ = 3;
				is_count_setting_default_ = true;
				is_rate_setting_default_ = true;
				has_setting_default_ = true;
			}
			else
			{
				is_count_setting_default_ = false;
				is_rate_setting_default_ = false;
				has_setting_default_ = false;
				pre_trigger_default_ = 0;
				return;
			}
		}

		/*
		 * Rebuilding the device page puts every selector in it back to its
		 * default, and the signals that come out of that would write those
		 * defaults over what the user saved. Hold the "restoring" flag for
		 * as long as the rebuild runs - on every way out of it - so that
		 * nothing is written while it does.
		 */
		struct RestoringGuard {
			bool &flag_;
			bool was_;
			RestoringGuard(bool &flag) : flag_(flag), was_(flag) { flag_ = true; }
			~RestoringGuard() { flag_ = was_; }
		};

		void MainBar::update_device_config_widgets()
		{
			const RestoringGuard restoring(restoring_defaults_);

			// get default settings
			get_setting_default();
			using namespace pv::popups;

			/*
			 * What the session holds, not what the selector shows: the two
			 * differ when a picked device could not be opened, and the page
			 * has to describe the device that is really there.
			 */
			const shared_ptr<devices::Device> device = session_.device();

			// Hide the widgets if no device is selected
			// channels_button_action_->setVisible(!!device);
			if (!device || !device->device())
			{
				// configure_button_action_->setVisible(false);
				sample_count_.show_none();
				sample_rate_.show_none();

				/* The page belongs to a device that is gone. A stale one
				 * would keep showing its settings and offer its controls,
				 * and the vendor name is what tells the rest of the bar
				 * which settings apply. */
				if (channels_ && !channels_->parent_dialog_) {
					delete channels_;
					channels_ = nullptr;
				}
				set_vendorName(QString());
				return;
			}

			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (!sr_dev)
				return;

			set_vendorName(QString::fromStdString(sr_dev->vendor()));

			// Check if this is a demo device
			shared_ptr<devices::HardwareDevice> hw_device =
				std::dynamic_pointer_cast<devices::HardwareDevice>(device);
			bool is_demo_device = false;
			if (hw_device) {
				string driver_name = hw_device->hardware_device()->driver()->name();
				if (driver_name == "demo") {
					is_demo_device = true;
				}
			}

			// Disable sample rate and sample count selectors for demo device
			if (is_demo_device) {
				sample_count_.setEnabled(false);
				sample_rate_.setEnabled(false);
			} else {
				sample_count_.setEnabled(true);
				sample_rate_.setEnabled(true);
			}

			// Update the configure popup
			/* Kept, not leaked: the binding it holds is what reports a
			 * configuration change, so it has to outlive this call. */
			if (device_options_)
				delete device_options_;
			device_options_ = new DeviceOptions(sr_dev, this);
			// configure_button_action_->setVisible(!device_options_->binding().properties().empty());
			// configure_button_.set_popup(device_options_);

			// Update the channels popup
			if (channels_)
			{
				/*
				 * The dialog that shows this page runs a nested event loop,
				 * so a device event lands here while the user is looking at
				 * it. Deleting the page would leave the dialog drawing a
				 * widget that is gone; the next device change rebuilds it.
				 */
				if (channels_->parent_dialog_)
					return;

				delete channels_;
				channels_ = nullptr;
			}
			/* A session between two devices has none for a moment, and the
			 * page reads it while it is built. */
			if (!session_.device())
				return;
			channels_ = new Channels(session_, QString::fromStdString(sr_dev->vendor()), this);

			// Read the USB version and apply it
			if (sr_dev->config_check(ConfigKey::USB_VERSION, Capability::GET))
			{
				try
				{
					auto gvar = sr_dev->config_get(ConfigKey::USB_VERSION);
					if (gvar.gobj())
					{
						uint64_t usb_version = Glib::VariantBase::cast_dynamic<Glib::Variant<guint64>>(gvar).get();
						channels_->set_usb2_mode(usb_version == 2);
					}
				}
				catch (Error &error)
				{
					qDebug() << "Failed to get USB version:" << error.what();
				}
			}
			connect(channels_, SIGNAL(max_sample_rate_change(uint64_t)), this, SLOT(on_update_max_sample_rate(uint64_t)));
			connect(channels_, SIGNAL(channel_count_change(uint16_t)), this, SLOT(on_update_channel_count(uint16_t)));
			connect(channels_, SIGNAL(threshold_value_change_1(float)), this, SLOT(on_update_threshold_value_1(float)));
			connect(channels_, SIGNAL(threshold_value_change_2(float)), this, SLOT(on_update_threshold_value_2(float)));
			connect(channels_, SIGNAL(threshold_value_change_ch32h417(float)), this, SLOT(on_update_threshold_value_ch32h417(float)));
			connect(channels_, SIGNAL(pre_trigger_changed(uint8_t)), this, SLOT(on_update_pre_trigger(uint8_t)));
			connect(channels_, SIGNAL(mode_changed(int)), this, SLOT(on_mode_changed(int)));
			connect(channels_, SIGNAL(adc_channel_changed(int)), this, SLOT(on_adc_channel_changed(int)));
			connect(channels_, SIGNAL(adc_precision_changed(int)), this, SLOT(on_adc_precision_changed(int)));
			connect(channels_, SIGNAL(threshold_value_raw_changed(double)), this, SLOT(on_threshold_value_raw_changed(double)));
			connect(channels_, SIGNAL(channel_selection_changed()), this, SLOT(on_channel_selection_changed()));
			// end
			//  channels_button_.set_popup(channels_);
			//  Update supported options.
			sample_count_supported_ = false;

			if (sr_dev->config_check(ConfigKey::LIMIT_SAMPLES, Capability::SET))
				sample_count_supported_ = true;

			// Add notification of reconfigure events
			// Note: No need to disconnect the previous signal as that QObject instance is destroyed
			connect(&device_options_->binding(), SIGNAL(config_changed()),
					this, SLOT(on_config_changed()));

			// Update sweep timing widgets.
			update_sample_count_selector();
			update_sample_rate_selector();

			/*
			 * The page was built with every control at its default - sixteen
			 * channels, the first sample count - so the settings have to go
			 * back into it. This is the one place a page is made, so it is
			 * the one place that has to put them back; callers no longer
			 * have to remember to do it themselves.
			 */
			renew_setting_default();
		}

		void MainBar::renew_default_bank_threshold()
		{
			// channels_->set_default_bank_threshold(threshold_bank_1_default_, threshold_bank_2_default_);
		}

		void MainBar::renew_setting_default()
		{
			/*
			 * Without a page there is nothing to put the values into: the
			 * device may have failed to open, and the page is only built
			 * for a device that did.
			 */
			if (!channels_)
				return;

			/*
			 * The rate, the capture length and the pre-trigger are put back
			 * for every board: they mean the same thing whatever driver is
			 * behind it. Only the thresholds, the channel width and the mode
			 * belong to the WCH hardware.
			 */
			const QString vendorName = get_vendorName();
			const bool is_wch =
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)] ||
				vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)];

			restoring_defaults_ = true;

			// CH32H417 uses the ThresholdLevel key of GlobalSettings
			if (vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)])
			{
				/*
				 * This page's own value first. The value last used anywhere
				 * is only there for the first page - what older versions
				 * kept for the whole program - so that an installation does
				 * not come up with the hardware default once.
				 */
				QSettings settings;
				const QString key = setting_group() + "/ThresholdLevel";
				GlobalSettings gs;
				if (settings.contains(key))
					threshold_ch32h417_default_ = settings.value(key).toFloat();
				else if (session_.id() == 0 &&
					gs.contains(GlobalSettings::Key_ThresholdLevel))
					threshold_ch32h417_default_ = gs.value(GlobalSettings::Key_ThresholdLevel).toFloat();
				else
					// The CH32H417 hardware default threshold is 2.3 V
					threshold_ch32h417_default_ = 2.3f;

				// Disable analog channels A0/A1 in CH32H417 logic analyzer mode
				for (const auto &sig : session_.signalbases())
				{
					if (sig->type() == data::SignalBase::AnalogChannel)
					{
						sig->set_enabled(false);
					}
				}
				channels_->set_default_bank_threshold(threshold_ch32h417_default_, threshold_bank_2_default_);
			}
			else if (is_wch)
			{
				channels_->set_default_bank_threshold(threshold_bank_1_default_, threshold_bank_2_default_);
			}

			/*
			 * A new page comes up with the last of the modes picked, so the
			 * one the user chose has to be put back. Only when it is not
			 * already the one showing: pressing the button runs
			 * channelSelect(), which drops every logic trigger and rebuilds
			 * the signals - and a board that only got a new instance from
			 * a rescan was still using all of that.
			 */
			if (is_wch)
			{
				int showing = -1;
				for (int i = 0; i < 4; i++)
					if (channels_->radioChannels[i] &&
						channels_->radioChannels[i]->isChecked()) {
						showing = i;
						break;
					}

				if (showing != channel_mode_index_ &&
					channels_->radioChannels[channel_mode_index_])
					channels_->radioChannels[channel_mode_index_]->click();

				commit_channel_number(channel_number_);
			}

			if (is_rate_setting_default_)
			{
				sample_rate_.set_current_index(sample_rate_default_index_);
			}
			updating_sample_rate_ = false;
			commit_sample_rate();
			update_sample_rate_selector_value();
			if (is_count_setting_default_)
			{
				sample_count_.set_current_index(sample_count_default_index_);
				commit_sample_count();
			}

			/*
			 * Nothing saved for this board means it keeps what its driver
			 * holds - which is what the page was built from.
			 */
			if (has_setting_default_)
			{
				channels_->set_default_pre_trigger(pre_trigger_default_);
				on_update_pre_trigger(pre_trigger_default_);
			}
			restoring_defaults_ = false;
			// end
		}

		bool MainBar::push_sample_rate(const shared_ptr<sigrok::Device> &sr_dev)
		{
			if (!sr_dev)
				return false;

			try
			{
				sr_dev->config_set(ConfigKey::SAMPLERATE,
								   Glib::Variant<guint64>::create(sample_rate_.value()));
			}
			catch (Error &error)
			{
				qDebug() << tr("Failed to configure samplerate:") << error.what();
				return false;
			}

			sample_rate_value_ = sample_rate_.value();
			return true;
		}

		void MainBar::apply_capture_limits()
		{
			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			/*
			 * The channel count first: it decides the sample width, the
			 * width decides the rate table, and a rate outside that table
			 * is refused. A board that came back as a new instance holds
			 * the default count again, so its table is the narrow one
			 * until it is told otherwise - and the rate the page shows
			 * would be turned down.
			 */
			commit_channel_number(channel_number_);

			/*
			 * Each value is handed over on its own. A rate the device no
			 * longer offers must not keep the count from being set: the
			 * count is what the capture ends on, and a capture started
			 * without one runs until it is stopped by hand.
			 */
			const bool rate_ok = push_sample_rate(device->device());

			/* Starting a capture tells the device; it does not change what
			 * the page is set up with, so nothing is written down. */
			commit_sample_count(false);
		}

		void MainBar::commit_sample_rate()
		{
			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;

			/* A file has no device behind it to configure, and the rate
			 * read from its header is not one to hand over. */
			if (session_.using_file_device())
				return;

			if (!push_sample_rate(device->device()))
				return;

			/*
			 * The count first, the list after: the list is rebuilt from the
			 * count the device holds and the rate it now runs at, so the
			 * count has to be the one belonging to the new rate by then.
			 * The other way round reads the old count against the new rate
			 * - a time that is out of range - and the page drops to the
			 * shortest capture.
			 */
			commit_sample_count();

			update_sample_count_selector();
		}

		void MainBar::commit_channel_number(uint64_t channelNum)
		{
			/*
			 * The clean up hands 0 over on the way out, and no device
			 * takes it, so nothing is sent for it.
			 */
			if (channelNum == 0)
				return;

			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			try
			{
				sr_dev->config_set(ConfigKey::NUM_LOGIC_CHANNELS,
								   Glib::Variant<gint32>::create((int32_t)channelNum));
			}
			catch (Error &error)
			{
			}
		}

		void MainBar::commit_threshold_value(int index, float value)
		{
			value *= 10;
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			try
			{
				if (index == 1)
				{
					sr_dev->config_set(ConfigKey::THRESHOLD_VALUE_1,
									   Glib::Variant<guint64>::create((uint64_t)value));
				}
				else
				{
					sr_dev->config_set(ConfigKey::THRESHOLD_VALUE_2,
									   Glib::Variant<guint64>::create((uint64_t)value));
				}
			}
			catch (Error &error)
			{
				// Do nothing...
			}
		}

		uint64_t MainBar::get_trigger_offset()
		{
			uint64_t offset = 0;
			const shared_ptr<devices::Device> device = device_selector_.selected_device();
			if (!device)
				return 0;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (!sr_dev)
				return 0;
			try
			{
				auto gvar = sr_dev->config_get(ConfigKey::TRIGGER_OFFSET);
				if (gvar.gobj())
				{
					offset = Glib::VariantBase::cast_dynamic<Glib::Variant<guint64>>(
								 gvar)
								 .get();
				}
			}
			catch (Error &error)
			{
				// Do nothing
			}
			return offset;
		}

		void MainBar::show_hardware_version()
		{
			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (!sr_dev)
				return;
			try
			{
				auto gvar = sr_dev->config_get(ConfigKey::HARDWARE_VERSION);
				hardware_version_ =
					Glib::VariantBase::cast_dynamic<Glib::Variant<guint64>>(gvar).get();
			}
			catch (Error &error)
			{
				qDebug() << "auto gvar = sr_dev->config_get(ConfigKey::HARDWARE_VERSION)";
			}
			Application *a = qobject_cast<Application *>(QApplication::instance());
			a->set_hardware_version(hardware_version_);
			a->renew_version_info();
		}

		void MainBar::commit_sample_count(bool remember)
		{
			uint64_t sample_count = 0;
			const shared_ptr<devices::Device> device = session_.device();
			if (!device)
				return;
			const shared_ptr<sigrok::Device> sr_dev = device->device();

			sample_count = ((double)sample_count_.get_current_data(sample_count_.get_current_index())) / 10000.00 * (double)sample_rate_.get_current_data(sample_rate_.get_current_index());
			{
				const double bytes_per_sample = (double)(channel_number_ / 8.00);
				const double max_samples =
					18000000000.00 / (bytes_per_sample > 0.0 ? bytes_per_sample : 1.0);
				if (sample_count <= 0)
					sample_count = 0.5 * (double)sample_rate_.get_current_data(sample_rate_.get_current_index());
				else if (sample_count > max_samples)
					sample_count = (uint64_t)max_samples;
			}
			/*
			 * The selectors hold no value yet while the bar is being set
			 * up, which leaves the count at 0; no device takes that, so
			 * wait for a call that has a count instead of failing here.
			 */
			if (sample_count == 0)
				return;

			if (sample_count_supported_)
			{
				try
				{
					sr_dev->config_set(ConfigKey::LIMIT_SAMPLES,
									   Glib::Variant<guint64>::create(sample_count));
					//  update_sample_count_selector();
				}
				catch (Error &error)
				{
					qDebug() << tr("Failed to configure sample count:") << error.what();
					return;
				}
			}
			sample_count_value_ = sample_count;
			// Devices with built-in memory might impose limits on certain
			// configurations, so let's check what sample rate the driver
			// lets us use now.
			// update_sample_rate_selector();
			if (remember)
				set_setting_default();
		}

		void MainBar::show_session_error(const QString text, const QString info_text)
		{
			pv::util::show_message(this, tr("Error"), text + "\n\n" + info_text,
				pv::util::message_box_icon(QMessageBox::Warning));
		}

		/*
		 * A page is named after its board, and that name carries the port -
		 * "usb/2-2.2", "ch375/4-8". A slash in it would make a path point
		 * into a directory that does not exist, so the name is turned into
		 * a plain file name here, with the separators turned into '_'.
		 */
		static QString to_file_name(const QString &name, const QString &suffix)
		{
			QString file = name;
			for (QChar &ch : file)
				if (QString("/\\:*?\"<>|").contains(ch))
					ch = '_';
			if (!suffix.isEmpty() && !file.endsWith(suffix, Qt::CaseInsensitive))
				file += suffix;
			return file;
		}

		void MainBar::export_file(shared_ptr<OutputFormat> format, bool selection_only, QString file_name)
		{
			using pv::dialogs::StoreProgress;

			// Stop any currently running capture session
			session_.stop_capture();

			QSettings settings;
			const QString dir = settings.value(SettingSaveDirectory).toString();

			pair<uint64_t, uint64_t> sample_range;

			// Selection only? Verify that the cursors are active and fetch their values
			if (selection_only)
			{
				views::trace::View *trace_view =
					qobject_cast<views::trace::View *>(session_.main_view().get());

				if (!trace_view->cursors()->enabled())
				{
					show_session_error(tr("Missing Cursors"), tr("You need to set the "
																 "cursors before you can save the data enclosed by them "
																 "to a session file (e.g. using the Show Cursors button)."));
					return;
				}

				const double samplerate = session_.get_samplerate();

				const pv::util::Timestamp &start_time = trace_view->cursors()->first()->time();
				const pv::util::Timestamp &end_time = trace_view->cursors()->second()->time();

				const uint64_t start_sample = (uint64_t)max(
					0.0, start_time.convert_to<double>() * samplerate);
				const uint64_t end_sample = (uint64_t)max(
					0.0, end_time.convert_to<double>() * samplerate);

				if ((start_sample == 0) && (end_sample == 0))
				{
					// Both cursors are negative and were clamped to 0
					show_session_error(tr("Invalid Range"), tr("The cursors don't "
															   "define a valid range of samples."));
					return;
				}

				sample_range = make_pair(start_sample, end_sample);
			}
			else
			{
				sample_range = make_pair(0, 0);
			}

			// Construct the filter
			const vector<string> exts = format->extensions();
			QString filter = tr("%1 files ").arg(QString::fromStdString(format->description()));

			if (exts.empty())
				filter += "(*)";
			else
				filter += QString("(*.%1);;%2 (*)").arg(QString::fromStdString(join(exts, ", *.")), tr("All Files"));

			/* The whole name is offered, made into a file name; a slash in
			 * it would otherwise cut the suggestion short at the port. */
			const QString suggested = to_file_name(session_.name(),
				exts.empty() ? QString() :
					"." + QString::fromStdString(exts.front()));

			// Show the file dialog
			if (file_name.isEmpty())
				file_name = QFileDialog::getSaveFileName(this, tr("Save File"),
					QDir(dir).filePath(suggested), filter);

			if (file_name.isEmpty())
				return;

			const QString abs_path = QFileInfo(file_name).absolutePath();
			settings.setValue(SettingSaveDirectory, abs_path);

			// Show the options dialog
			map<string, Glib::VariantBase> options;
			if (!format->options().empty())
			{
				dialogs::InputOutputOptions dlg(
					tr("Export %1").arg(QString::fromStdString(format->description())),
					format->options(), this);
				if (!dlg.exec())
					return;
				options = dlg.options();
			}

			if (!selection_only)
			{
				if (format == session_.device_manager().context()->output_formats()["srzip"])
				{
					session_.set_save_path(QFileInfo(file_name).absolutePath());
					session_.set_name(QFileInfo(file_name).fileName());
				}
				else
					session_.set_save_path("");
			}

			StoreProgress *dlg = new StoreProgress(file_name, format, options,
												   sample_range, session_, this);
			dlg->run();
		}

		void MainBar::import_file(shared_ptr<InputFormat> format)
		{
			assert(format);

			QSettings settings;
			const QString dir = settings.value(SettingOpenDirectory).toString();

			// Construct the filter
			const vector<string> exts = format->extensions();
			const QString filter_exts = exts.empty() ? "" : QString::fromStdString("%1 (%2)").arg(tr("%1 files").arg(QString::fromStdString(format->description())), QString::fromStdString("*.%1").arg(QString::fromStdString(join(exts, " *."))));
			const QString filter_all = QString::fromStdString("%1 (%2)").arg(
				tr("All Files"), QString::fromStdString("*"));
			const QString filter = QString::fromStdString("%1%2%3").arg(
				exts.empty() ? "" : filter_exts,
				exts.empty() ? "" : ";;",
				filter_all);

			// Show the file dialog
			const QString file_name = QFileDialog::getOpenFileName(
				this, tr("Import File"), dir, filter);

			if (file_name.isEmpty())
				return;

			// Show the options dialog
			map<string, Glib::VariantBase> options;
			if (!format->options().empty())
			{
				dialogs::InputOutputOptions dlg(
					tr("Import %1").arg(QString::fromStdString(format->description())),
					format->options(), this);
				if (!dlg.exec())
					return;
				options = dlg.options();
			}

			session_.load_file(file_name, "", format, options);
			set_vendorName(file_name);
			const QString abs_path = QFileInfo(file_name).absolutePath();
			settings.setValue(SettingOpenDirectory, abs_path);
		}

		void MainBar::on_device_selected()
		{
			switch_to_device(device_selector_.selected_device(), true);
		}

		bool MainBar::switch_to_device(shared_ptr<devices::Device> device, bool ask)
		{
			if (!device)
				return false;

			/* Picking the board that is already in use changes nothing. In
			 * a session opened for one board that is the only entry there
			 * is, and it must not throw the capture away. */
			if (device == session_.device())
				return false;

			/*
			 * A scan hands out a fresh instance for a board that stayed
			 * plugged, so the entry just clicked is never the object the page
			 * holds. Same board means same entry: compare what it is, not which
			 * object it is. Closing it to open it again throws the capture away
			 * and races the FX2 - its version read then times out.
			 *
			 * A board that failed to open is the exception: clicking it again
			 * is the retry, and the session has to open it once more.
			 */
			const QString clicked_id =
				widgets::DeviceToolButton::device_identity(device);
			if (session_.device() && !session_.device_open_failed() &&
				!clicked_id.isEmpty() &&
				clicked_id ==
				widgets::DeviceToolButton::device_identity(session_.device())) {
				update_device_list();
				return false;
			}

			/*
			 * Everything set up for the device in hand goes when another
			 * one is taken, so ask before that happens - but only when
			 * there is something to lose. Same test as closing the
			 * window uses: capture that has not been saved. A decoder
			 * merely set up and waiting has no data behind it.
			 */
			if (ask && !device_switch_allowed_ && session_.data_held()) {
				const QMessageBox::StandardButton answer = QMessageBox::question(
					this, tr("Switch device"),
					tr("Switch device? The captured data and its decoders "
					   "will be lost."),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

				if (answer != QMessageBox::Yes) {
					/* Put the selector back to the device in use. */
					update_device_list();
					return false;
				}

				device_switch_allowed_ = true;
			}

			const shared_ptr<sigrok::Device> sr_dev = device->device();
			if (!sr_dev) {
				return false;
			}

			const QString vendor = QString::fromStdString(sr_dev->vendor());
			const bool wch_device =
				(vendor == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)] ||
				 vendor == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)]);

			/*
			 * The vendor is what the settings logic keys off, so for a WCH
			 * board it must stay the bare model name; the label the user
			 * sees is set further down and may carry the port.
			 */
			if (wch_device)
				set_vendorName(vendor);

			session_.select_device(device);

			/*
			 * A board that refused to open is not the one in use. The page
			 * keeps the board it had - dropping it left the device list
			 * with nothing in it - so the failure itself is what has to be
			 * asked, not an empty device.
			 */
			if (session_.device_open_failed()) {
				update_device_list();
				return false;
			}

			device_selector_.set_current_device(session_.device());

			if (vendor == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)]) {
				/*
				 * A board can come back in the ADC mode it was left in, and
				 * the capture path here is the logic analyzer. Done after the
				 * open: the mode is a device setting, not a session one.
				 */
				try {
					sr_dev->config_set(ConfigKey::PATTERN_MODE,
						Glib::Variant<Glib::ustring>::create("Logic Analyzer"));
					qDebug() << "CH32H417 device mode set to Logic Analyzer on device switch";
					update_sample_rate_selector();
					update_sample_rate_selector_value();
				} catch (Error &error) {
					qDebug() << tr("Failed to set CH32H417 mode on device switch:")
						 << error.what();
				}
			}

			return true;
		}


		void MainBar::hot_plug_listen()
		{
			/*
			 * libusb reports every arrival and every removal of the builds below,
			 * carrying the board it happened to. Nothing is polled: this thread
			 * sleeps in libusb's event wait and is woken by a change.
			 *
			 * Only the chain of ports is compared - a second libusb context
			 * numbers buses its own way - and which page a chain belongs to is
			 * the main thread's work.
			 */
			struct HotPlugId {
				uint16_t vid;
				uint16_t pid;
			};

			const HotPlugId hot_plug_ids[] = {
				{ WCH_VID, WCH_PID_CH32H417 },
				{ WCH_VID, WCH_PID_CH569 },
				{ 0x1d50, 0x608c, },
				{ 0x1d50, 0x608d, },
				/* A blank FX2 arrives under Cypress ids: that arrival is
				 * what gets a scan going, and the scan is what flashes it. */
				{ 0x04b4, 0x8613, },
			};
			const size_t hot_plug_count = G_N_ELEMENTS(hot_plug_ids);

			const UsbApi *const usb = usb_api();

			if (!usb || !usb->can_watch())
			{
				qWarning() << "hot_plug_listen: libusb cannot watch the bus";
				return;
			}

			libusb_context *libusb_ctx = NULL;

			/* The context must be initialized explicitly: an unwatched bus has
			 * nothing to report, and a NULL context is not the same thing. */
			if (usb->init(&libusb_ctx) != 0)
			{
				qWarning() << "hot_plug_listen: libusb_init failed";
				return;
			}

			if (usb->has_capability(LIBUSB_CAP_HAS_HOTPLUG) != 1)
			{
				qWarning() << "hot_plug_listen: hot-plug is not available";
				usb->exit(libusb_ctx);
				return;
			}

			libusb_hotplug_callback_handle callbacks[G_N_ELEMENTS(hot_plug_ids)];
			size_t callbacks_count = 0;

			for (size_t i = 0; i < hot_plug_count; i++)
			{
				libusb_hotplug_callback_handle handle = 0;

				const int rc = usb->hotplug_register(libusb_ctx,
					LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
						LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
					0, hot_plug_ids[i].vid, hot_plug_ids[i].pid,
					LIBUSB_HOTPLUG_MATCH_ANY, on_usb_bus_change, NULL,
					&handle);

				if (rc == 0)
					callbacks[callbacks_count++] = handle;
				else
					qWarning() << "hot_plug_listen: hot-plug registration"
							   << "failed for" << hot_plug_ids[i].vid
							   << hot_plug_ids[i].pid << rc;
			}

			if (callbacks_count == 0)
			{
				qWarning() << "hot_plug_listen: no board is being watched";
				usb->exit(libusb_ctx);
				return;
			}

			/*
			 * No quiet period here: a pass closes and re-flashes the boards it
			 * finds, and the events that follow are piled up by the front end's
			 * own debounce instead - one pass answers them all, and a board
			 * that is already running is only re-opened.
			 */

			while (hot_plug_listen_)
			{
				/* The wait is where this thread lives; the timeout only lets the
				 * loop notice that it is over. */
				struct timeval wait = { 0, 200000 };
				usb->handle_events_timeout(libusb_ctx, &wait);

				/*
				 * Safety net: the upgrade ends when the board comes back, and
				 * a device that never does would leave the board's page
				 * silent about it for good. Give up after a minute.
				 */
				if (upgrade_running() &&
					g_get_monotonic_time() >= s_upgrade_deadline.load())
				{
					qWarning() << "Firmware upgrade flag timed out, "
								"resuming CH32H417 hot-plug detection";
					end_upgrade();
					/* The page it was started from is still masked, and its
					 * tab cannot be closed until the mask goes: the window
					 * takes it down, so this has to cross threads. */
					QMetaObject::invokeMethod(this, [this]() {
						Q_EMIT device_upgrading(false, -1);
					}, Qt::QueuedConnection);
				}

				const std::vector<UsbBusEvent> events = usb_events_take();

				if (events.empty())
					continue;

				const QSet<QString> busy = get_busy_identities();

				bool wch_arrived = false;
				bool went = false;
				QStringList gone;
				QStringList arrived;

				for (const UsbBusEvent &event : events)
				{
					/*
					 * An arrival is kept the way a page identity spells it,
					 * "<bus>-<chain>": the ports alone read alike for two
					 * boards on the same port of different buses.
					 */
					if (event.event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED &&
						!event.chain.empty()) {
						QString arrived_at =
							QString::fromStdString(event.chain);

						if (event.bus > 0)
							arrived_at = QString::number(event.bus) +
								QLatin1Char('-') + arrived_at;

						arrived << arrived_at;
					}

					if (event.vid == WCH_VID &&
						event.event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
						wch_arrived = true;

					if (event.event != LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
						continue;

					went = true;

					if (event.chain.empty())
						continue;

					/* Which pages lost their board: the ones whose identity
					 * carries this chain. */
					const QString chain =
						QString::fromStdString(event.chain);
					for (const QString &identity : busy)
					{
						if (chain_matches_identity(identity, chain) &&
							!gone.contains(identity))
							gone << identity;
					}
				}

				/*
				 * An upgrade makes the CH32H417 re-enumerate as a HID
				 * (PID 0xFE17); that window is ignored. The board coming back
				 * goes to the slot the upgrade flow uses: it clears the flag,
				 * reports success and refreshes the list.
				 */
				if (upgrade_running() && wch_arrived)
				{
					QMetaObject::invokeMethod(this, "on_ch32h417_device_event",
											  Qt::QueuedConnection,
											  Q_ARG(unsigned long, CH375_DEVICE_ARRIVAL));
					continue;
				}

				/* The pages are told below; the pass is told as well, because a
				 * scan can still list a board the host has only just taken
				 * away - or miss one that is still being flashed. */
				declare_arrived(arrived);
				declare_gone(gone);

				qInfo() << "[HOTPLUG] gone" << gone << "arrived" << arrived
					<< "captures" << s_captures_running.load()
					<< "busy" << busy;

				/*
				 * A pass closes the device in use, so while a capture runs it waits
				 * for that capture to end - unless the board behind that capture
				 * was the one that went: nothing is left to capture on then and it
				 * runs at once.
				 *
				 * The pages that did lose a board are told, so that they can say so
				 * instead of sitting there looking alive.
				 */
				/*
				 * Decided and recorded under the lock the count is under: a capture
				 * that ends while this is deciding has to see the flag, or the pass
				 * it should have triggered is lost and the pages stay masked until
				 * something else happens.
				 */
				bool defer = false;
				QSet<QString> capturing;
				{
					std::lock_guard<std::mutex> lock(s_capture_mutex);

					const int captures = s_captures_running;
					/*
					 * A capture whose board went is ended at once. Only a
					 * board a bus event names by port chain can be told that
					 * way; a vendor one is noted by the scan that lost it.
					 */
					bool captured_went = false;
					QStringList captured_gone;
					for (const QString &id : s_capturing_identities) {
						if (gone.contains(id)) {
							captured_went = true;
							captured_gone << id;
						}
						capturing.insert(id);
					}

					/*
					 * A capture whose board went ends right there, the way the
					 * stop button ends it: the pass that follows asks about the
					 * page. Nothing is closed while it runs.
					 */
					if (!captured_gone.isEmpty())
						Q_EMIT capture_board_gone(captured_gone);

					defer = (captures > 0 && !captured_went);
					s_attach_deferred = defer;
				}

				if (defer)
				{
					/*
					 * The board of the capture is not masked: its capture ends
					 * by itself and the pass that follows asks about the page.
					 * Only a board that went beside it is.
					 */
					QStringList notify;
					for (const QString &id : gone)
						if (!capturing.contains(id))
							notify << id;

					if (!notify.isEmpty())
						Q_EMIT devices_went_offline(notify);
					else if (gone.isEmpty() && went)
					{
						/*
						 * The vendor backend carries no ports to match a board
						 * on, so the one that went cannot be named. The board
						 * of the capture is still there (that is what waiting
						 * means), so what went belongs to one of the pages that
						 * are not capturing: they are masked now, and the pass
						 * corrects the list.
						 */
						QStringList suspects;
						for (const QString &id : busy)
							if (id.startsWith("ch375/") &&
								!capturing.contains(id))
								suspects << id;
						if (!suspects.isEmpty())
							Q_EMIT devices_went_offline(suspects);
					}
				}
				else
					device_attached();
			}

			for (size_t i = 0; i < callbacks_count; i++)
				if (usb->hotplug_deregister)
					usb->hotplug_deregister(libusb_ctx, callbacks[i]);

			usb->exit(libusb_ctx);
		}

		void MainBar::update_upgrade_action()
		{
			/* An FX2 page has nothing the upgrade tool knows how to talk to;
			 * hiding the entry says so without an error box. */
			if (action_update_)
				action_update_->setVisible(
					upgradeable_device(session_.device()));
		}

		void MainBar::on_device_changed()
		{
			/* The page is on another device now: an upgrade mask left over
			 * from the one before would have nothing left to say. A board
			 * that merely came back does not come through here - taking an
			 * instance over is not a device change. */
			if (is_device_upgrading())
				set_device_upgrading(false);

			set_device_offline(false);

			/* The change is through: the next one asks again. */
			device_switch_allowed_ = false;

			const shared_ptr<devices::HardwareDevice> hw_device =
				dynamic_pointer_cast<devices::HardwareDevice>(session_.device());
			set_device_driver(hw_device ?
				QString::fromStdString(
					hw_device->hardware_device()->driver()->name()) : QString());
			set_device_identity(
				widgets::DeviceToolButton::device_identity(session_.device()));

			/* Rebuilds the selector, which ends by rebuilding the device
			 * page as well; calling that again here would throw the page
			 * away and build it a second time. Putting the settings back
			 * is part of building the page, so it happens there. */
			update_device_list();
			update_device_status_icon();
			update_upgrade_action();

			/*
			 * The zoom and pan limits are derived from the sample rate and
			 * sample count of the device, so they belong to the device that
			 * just went away. Recalculate them here, otherwise the view
			 * keeps the range of the previous device until the next
			 * acquisition starts.
			 */
			if (view_)
				view_->update_scale_offset(false);
		}

		void MainBar::refresh_device_widgets()
		{
			/* The device went away and nothing took its place: the page that
			 * described it has to go as well, or it keeps showing a board
			 * that is no longer on any bus. */
			update_device_config_widgets();
			update_device_status_icon();
		}

		MainBar *MainBar::hot_plug_owner()
		{
			return s_mainbar_instance;
		}

		void MainBar::hot_plug_start()
		{
			/* Set the static instance pointer */
			s_mainbar_instance = this;

			/* Register unsigned long as a Qt metatype for cross-thread signal delivery */
			qRegisterMetaType<unsigned long>("unsigned long");

			/* Start the bus watcher */
			if (t_hot_plug.joinable())
			{
				hot_plug_listen_ = false;
				t_hot_plug.join();
			}
			hot_plug_listen_ = true;
			t_hot_plug = std::thread(&MainBar::hot_plug_listen, this);
		}

		void MainBar::hot_plug_stop()
		{
			/* Clear the static instance pointer */
			s_mainbar_instance = nullptr;

			/* Stop the bus watcher */
			if (t_hot_plug.joinable())
			{
				hot_plug_listen_ = false;
				t_hot_plug.join();
			}

#ifdef _WIN32
			/* Release CH375DLL */
			ch375_cleanup();
#endif
		}

		void MainBar::on_capture_state_changed(int state)
		{
			set_capture_state(session_.is_repeat_acquisition_? pv::Session::Running : (pv::Session::capture_state)state);
		}

		void MainBar::on_sample_count_changed()
		{
			if (!updating_sample_count_)
				commit_sample_count();
		}

		void MainBar::on_sample_rate_changed()
		{
			if (!updating_sample_rate_)
				commit_sample_rate();
		}

		void MainBar::on_config_changed()
		{
			// We want to also call update_sample_rate_selector() here in case
			// the user changed the SR_CONF_EXTERNAL_CLOCK option. However,
			// commit_sample_rate() does this already, so we don't call it here

			commit_sample_count();
			commit_sample_rate();
		}

		void MainBar::on_actionSingle_triggered()
		{
			capture_mode_ = Session::Single;
			model_change_button_->setIcon(QIcon(":/icons/Single.png"));
			// model_change_button_->setText(tr("Single"));
			model_change_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		}

		void MainBar::on_actionRepeat_triggered()
		{
			capture_mode_ = Session::Repeat;
			model_change_button_->setIcon(QIcon(":/icons/Repeat.png"));
			// model_change_button_->setText(tr("Repeat"));
			model_change_button_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		}

		void MainBar::update_runstop_status(int state)
		{
			if (state == Session::Stopped)
			{
				// set start icon
				run_stop_button_->setIcon(QIcon(":/icons/Start.png"));
				run_stop_button_->setText(tr("Start"));
			}
			else
			{
				run_stop_button_->setIcon(QIcon(":/icons/Stop.png"));
				run_stop_button_->setText(tr("Stop"));
			}
		}

		void MainBar::on_actionNewView_triggered(QAction *action)
		{
			if (action)
				new_view(&session_, action->data().toInt());
			else
				// When the icon of the button is clicked, we create a trace view
				new_view(&session_, views::ViewTypeTabularDecoder);
		}

		void MainBar::on_actionOpen_triggered()
		{
			QSettings settings;
			const QString dir = settings.value(SettingOpenDirectory).toString();

			// Show the dialog
			const QString file_name = QFileDialog::getOpenFileName(
				this, tr("Open File"), dir, tr("sigrok Sessions (*.sr);;"
											   "All Files (*)"));

			if (!file_name.isEmpty())
			{
				const QString abs_path = QFileInfo(file_name).absolutePath();
				settings.setValue(SettingOpenDirectory, abs_path);

				/*
				 * A file opened while a trace is up is a second trace, not a
				 * replacement for the first: the window puts it in a session
				 * of its own (and reuses the one in front when that still
				 * holds nothing).
				 */
				Q_EMIT open_file_requested(file_name);
			}
		}

		void MainBar::on_actionSave_triggered()
		{
			// A path is only set if we loaded/saved an srzip file before
			if (session_.save_path().isEmpty())
			{
				on_actionSaveAs_triggered();
				return;
			}

			QFileInfo fi = QFileInfo(QDir(session_.save_path()),
				to_file_name(session_.name(), ".sr"));
			export_file(session_.device_manager().context()->output_formats()["srzip"], false,
						fi.absoluteFilePath());
		}

		void MainBar::on_actionSaveAs_triggered()
		{
			export_file(session_.device_manager().context()->output_formats()["srzip"]);
		}

		void MainBar::on_actionSaveSelectionAs_triggered()
		{
			export_file(session_.device_manager().context()->output_formats()["srzip"], true);
		}

		void MainBar::on_actionSaveSetup_triggered()
		{
			QSettings settings;
			const QString dir = settings.value(SettingSaveDirectory).toString();

			const QString file_name = QFileDialog::getSaveFileName(
				this, tr("Save File"), dir, tr("PulseView Session Setups (*.pvs);;"
											   "All Files (*)"));

			if (file_name.isEmpty())
				return;

			QSettings settings_storage(file_name, QSettings::IniFormat);
			session_.save_setup(settings_storage);
		}

		void MainBar::on_actionRestoreSetup_triggered()
		{
			QSettings settings;
			const QString dir = settings.value(SettingSaveDirectory).toString();

			const QString file_name = QFileDialog::getOpenFileName(
				this, tr("Open File"), dir, tr("PulseView Session Setups (*.pvs);;"
											   "All Files (*)"));

			if (file_name.isEmpty())
				return;

			QSettings settings_storage(file_name, QSettings::IniFormat);
			session_.restore_setup(settings_storage);
		}

		void MainBar::on_actionConnect_triggered()
		{
			// Stop any currently running capture session
			session_.stop_capture();

			dialogs::Connect dlg(this, session_.device_manager());

			// If the user selected a device, select it in the device list. Select the
			// current device otherwise.
			if (dlg.exec())
				session_.select_device(dlg.get_selected_device());

			update_device_list();
		}

		void MainBar::on_add_decoder_clicked()
		{
			show_decoder_selector(&session_);
		}

		void MainBar::on_add_math_signal_clicked()
		{
			// shared_ptr<data::SignalBase> signal = make_shared<data::MathSignal>(session_);
			// session_.add_generated_signal(signal);
		}

		void MainBar::add_toolbar_widgets()
		{
			addWidget(device_status_icon_);
			addWidget(open_button_);
			addWidget(save_button_);
			addSeparator();

			device_selector_action_ = addWidget(&device_selector_);
			if (channels_button_ex_)
				addWidget(channels_button_ex_);
			addWidget(&sample_count_);
			addWidget(&sample_rate_);
			model_change_action_ = addWidget(model_change_button_);
			run_stop_action_ = addWidget(run_stop_button_);
			addSeparator();

			StandardBar::add_toolbar_widgets();
#ifdef ENABLE_DECODE
			addWidget(add_decoder_button_);
#endif
			addWidget(new_view_button_);
			addSeparator();

			addWidget(help_button_);
		}

		bool MainBar::eventFilter(QObject *watched, QEvent *event)
		{
			if (offline_mask_ && offline_mask_page_ == watched &&
				event->type() == QEvent::Resize)
			{
				offline_mask_->setGeometry(offline_mask_page_->rect());
				offline_mask_->raise();
			}

			if (sample_count_supported_ && (watched == &sample_count_ || watched == &sample_rate_) &&
				(event->type() == QEvent::ToolTip))
			{
				uint64_t sampleCount = ((double)sample_count_.get_current_data(sample_count_.get_current_index())) / 10000.00 * (double)sample_rate_.get_current_data(sample_rate_.get_current_index());
				QHelpEvent *help_event = static_cast<QHelpEvent *>(event);

				QString str = tr("Total Sampling Count: %1").arg(sampleCount);
				QToolTip::showText(help_event->globalPos(), str);

				return true;
			}

			return false;
		}

		uint64_t MainBar::get_sample_count()
		{
			return sample_count_value_;
		}

		uint64_t MainBar::get_sample_rate()
		{
			return sample_rate_value_;
		}

		uint16_t MainBar::get_sample_count_index()
		{
			return sample_count_.get_current_index();
		}

		void MainBar::channels_button_ex_clicked()
		{
			/* The page is built per device, so a click that comes before one
			 * is there builds it now: the device may have just arrived. */
			if (!channels_)
				update_device_config_widgets();
			if (!channels_)
				return;

			QDialog *dialog = new QDialog(this);
			dialog->setWindowTitle(tr("Device Options"));
			dialog->setModal(true);

			dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

			const QSize popup_size = channels_->sizeHint();
			if (!popup_size.isEmpty())
				dialog->setFixedSize(popup_size);

			QVBoxLayout *layout = new QVBoxLayout(dialog);
			layout->setContentsMargins(0, 0, 0, 0);
			layout->addWidget(channels_);
			connect(dialog, &QDialog::finished, this, [this]() {
				if (channels_) {
					channels_->setParent(this);
					channels_->parent_dialog_ = nullptr;
				}

				/*
				 * A device change that arrived while the dialog was up was
				 * left alone, so the page may still describe the board that
				 * is gone. Rebuilding it has to wait for the dialog to be
				 * deleted, or the layout would hold a widget that no longer
				 * exists.
				 */
				QTimer::singleShot(0, this, [this]() {
					update_device_config_widgets();
				});
			});
			dialog->setLayout(layout);

			channels_->parent_dialog_ = dialog;

			QWidget *mainWin = this->window();

			/*
			 * The dialog is placed at the centre of the main window, and it
			 * is placed there once it has been shown rather than before it:
			 * it has no size of its own until then. The popup it holds is
			 * handed back to this tool bar every time the dialog is closed,
			 * and the layout of a popup that is nobody's child any more --
			 * hidden, that is -- has no size hint to give, so the hint read
			 * here is empty on the second and every later opening. A dialog
			 * centred on a size of nothing is centred on its corner, and it
			 * came up with that corner at the centre of the main window
			 * instead of with the dialog around it.
			 */
			QTimer::singleShot(0, dialog, [dialog, mainWin]() {
				dialog->move(mainWin->geometry().center() - dialog->rect().center());
			});

			dialog->exec();

			delete dialog;
		}

		void MainBar::on_action_version_info_triggered()
		{
			Application* a = qobject_cast<Application*>(QApplication::instance());
			auto version_info = a->get_version_info();
			QString softwareVersion = version_info[0].second;
			QString firmwareVersion;
			QString fpgaVersion;
			QString text;
			QString vendorName = get_vendorName();

			/*
			 * What this page is on, not what its menu offers: the selector holds a
			 * device of its own, and on a page without one it says nothing about
			 * what to report. A demo device has no firmware to name.
			 */
			const shared_ptr<devices::Device> page_device = session_.device();
			const shared_ptr<devices::HardwareDevice> page_hw =
				std::dynamic_pointer_cast<devices::HardwareDevice>(page_device);
			const QString driver = page_hw ?
				QString::fromStdString(
					page_hw->hardware_device()->driver()->name()) :
				QString();

			// For the demo device or no device, only the software version is shown
			if (driver == QStringLiteral("fx2lafw"))
			{
				/* An FX2 runs the firmware that was flashed into it, and the
				 * version it answers with is the only one there is. Only what a
				 * scan kept is read here: this page has the board open. */
				const std::string fw = pv::fx2_cached_version(
					widgets::DeviceToolButton::device_identity(
						page_device).toStdString());

				text = tr("<b>Software Version:</b> %1<br/>").arg(softwareVersion) +
					   tr("<b>Firmware Version:</b> %1<br/>").arg(
						   fw.empty() ? QStringLiteral("null") :
							   QString::fromStdString(fw));
			}
			else if (vendorName != DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)] && vendorName != DeviceModelNames[static_cast<int>(WchDeviceType::CH569)])
			{
				text = tr("<b>Software Version:</b> %1<br/>").arg(softwareVersion);
			}
			else
			{
				show_hardware_version();

				if (hardware_version_ == 0)
				{
					firmwareVersion = "null";
					fpgaVersion = "null";
				}
				else if (vendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)])
				{
					// CH32H417: fw_version is (firmware version << 8) | hardware version
					// Displayed as <hardware version>.<firmware version> (e.g. 1.1)
					uint8_t fw = (hardware_version_ >> 8) & 0xff; // firmware version
					uint8_t hw = hardware_version_ & 0xff;		  // hardware version
					firmwareVersion = QString::number(hw) + "." + QString::number(fw);

					text = tr("<b>Software Version:</b> %1<br/>").arg(softwareVersion) + \
						   tr("<b>Firmware Version:</b> %1<br/>").arg(firmwareVersion);
				}
				else
				{
					// The CH569 has a firmware version and an FPGA version
					firmwareVersion = QString::number((hardware_version_ >> 16) & 0xff);
					firmwareVersion += ".";
					firmwareVersion += QString::number((hardware_version_ >> 24) & 0xff);
					fpgaVersion = QString::number((hardware_version_ >> 0) & 0xff);
					fpgaVersion += ".";
					fpgaVersion += QString::number((hardware_version_ >> 8) & 0xff);
					text = tr("<b>Software Version:</b> %1<br/>").arg(softwareVersion) + \
						   tr("<b>Firmware Version:</b> %1<br/>").arg(firmwareVersion) + \
						   tr("<b>FPGA Version:</b> %1").arg(fpgaVersion);	   
				}
			}
			pv::util::show_message(this, tr("Version Info"), text,
				pv::util::message_box_icon(QMessageBox::Information),
				400, true);
		}

		void MainBar::on_action_feedback_triggered()
		{
		}

		void MainBar::on_action_update_triggered()
		{
			/*
			 * The board this page is on is the one that gets upgraded. It is
			 * the only one the page can speak for, and an FX2 running the
			 * sigrok firmware takes no CH32H417 image at all.
			 */
			const shared_ptr<devices::Device> page_device = session_.device();

			if (!upgradeable_device(page_device)) {
				pv::util::show_message(this, tr("Warning"),
					tr("The current device does not support firmware upgrade.\nPlease connect the CH32H417 device and try again."),
					pv::util::message_box_icon(QMessageBox::Warning), 350);
				return;
			}

			/*
			 * The command channel has to be the one that owns the board: a
			 * device bound to WinUSB cannot be written through the CH375 DLL.
			 */
			const shared_ptr<devices::HardwareDevice> page_hw =
				std::dynamic_pointer_cast<devices::HardwareDevice>(page_device);
			const bool iap_use_libusb = page_hw &&
				string(page_hw->hardware_device()->driver()->name()) ==
					"wch-ch32h417-libusb";

			/* Which board it is, and where the tool finds it: the ports it
			 * sits in, not the order it happened to be enumerated in. */
			const QString identity =
				widgets::DeviceToolButton::device_identity(page_device);
			const int target = upgrade_index_for(identity, iap_use_libusb);
			qDebug() << "Firmware upgrade:" << identity << "-> index" << target
					 << (iap_use_libusb ? "(libusb)" : "(CH375)");

			if (target < 0) {
				pv::util::show_message(this, tr("Warning"),
					tr("The board this session is on could not be found for the upgrade."),
					pv::util::message_box_icon(QMessageBox::Warning), 350);
				return;
			}

			/* The IAP channel carries no port, so the serial is what the
			 * board is picked by once it is in its bootloader. */
			const shared_ptr<sigrok::Device> page_sr = page_device->device();
			const QString serial = page_sr ?
				QString::fromStdString(page_sr->serial_number()) : QString();

			/* Let the user pick the firmware file */
			QString firmwarePath = QFileDialog::getOpenFileName(this,
																tr("Select firmware file"),
																"",
																tr("Firmware File (*.bin *.BIN *.hex *.HEX);;All Files (*.*)"));

			/* The user cancelled the selection */
			if (firmwarePath.isEmpty())
			{
				return;
			}

			if (session_.get_capture_state() != Session::Stopped)
			{
				session_.stop_capture();
			}

			/*
			 * The board is about to leave the bus for its bootloader, so the
			 * pages have to know it went on purpose: the pass that finds it
			 * missing would otherwise take the page for one whose board was
			 * pulled and ask about saving the capture.
			 */
			begin_upgrade(identity);

			/*
			 * The progress is a mask over the page, not a dialog: the window
			 * stays usable, the page is not, and the tab it sits in cannot be
			 * closed while the image is being written - the page that owns
			 * this worker has to outlive it.
			 */
			/* From here until the worker says it is done, its progress is
			 * what the mask shows, and nothing else. */
			upgrade_writing_ = true;
			upgrade_percent_ = 0;
			Q_EMIT device_upgrading(true, 0);

			pv::dialogs::IAPWorker *worker = new pv::dialogs::IAPWorker(target,
				firmwarePath, WCH_VID, WCH_PID_CH32H417_HID, false,
				iap_use_libusb, serial);
			QThread *workerThread = new QThread();
			worker->moveToThread(workerThread);

			connect(workerThread, &QThread::started, worker, &pv::dialogs::IAPWorker::do_work);
			/* The worker emits from its own thread; the mask is a widget and
			 * only the window may touch it, so this goes through the thread
			 * the bar lives in. */
			connect(worker, &pv::dialogs::IAPWorker::progress_updated, this,
				[this](int percent) {
					/* The last steps are reported while the board resets, and
					 * they can land after the worker said it was done - a
					 * mask put back then would never come down. Only what is
					 * written while it is written is passed on, and only when
					 * the number moves: the worker reports per block. */
					if (!upgrade_writing_ || percent == upgrade_percent_)
						return;
					upgrade_percent_ = percent;
					Q_EMIT device_upgrading(true, percent);
				}, Qt::QueuedConnection);
			/* "this" as the context object: the worker emits from its own
			 * thread, and without it the lambda would build the message box
			 * there and Qt would complain about the parent's thread. */
			connect(worker, &pv::dialogs::IAPWorker::finished, this, [=](bool success)
					{
				upgrade_writing_ = false;
				upgrade_percent_ = -1;

				if (!success) {
					end_upgrade();
					/* The mask comes off before the message: the board is not
					 * coming back on its own, and the question the pass asks
					 * is about this page. */
					Q_EMIT device_upgrading(false, -1);
					pv::util::show_message(this, tr("Update Failed"),
						tr("Firmware Update Failed!\nPlease check the device connection and firmware file."),
						pv::util::message_box_icon(QMessageBox::Critical), 0);
					/* The board is not coming back on its own, so the bus is
					 * read: the page has to find it gone instead of keeping a
					 * device that is not there. */
					device_attached();
				}
				/*
				 * The mask cannot outlive the worker, whatever the outcome: the
				 * arrival that ends a successful upgrade is reported through the
				 * bus watcher - another page, another thread - and that is a
				 * hint, not something a page can wait on. It stays up only for
				 * as long as the reset needs.
				 */
				QTimer::singleShot(UpgradeMaskGraceMs, this, [this]() {
					if (upgrade_writing_)
						return;
					set_device_upgrading(false);
					Q_EMIT device_upgrading(false, -1);
				});

				workerThread->quit(); });
			connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
			connect(workerThread, &QThread::finished, worker, &dialogs::IAPWorker::deleteLater);

			workerThread->start();

			return;
		}

		void MainBar::on_action_use_guide_triggered()
		{
			// Directory that holds the executable
			QString appDir = QCoreApplication::applicationDirPath();

			QString pdfPath = appDir + "/doc/U3LogicAnalyzer逻辑分析仪使用手册.pdf";

			pdfPath = QDir::toNativeSeparators(pdfPath);

			QFileInfo fileInfo(pdfPath);
			if (!fileInfo.exists())
			{
				pv::util::show_message(this, tr("Information"),
					tr("PDF file not found:\n%1\n\nMake sure the doc directory and the user manual are present.").arg(pdfPath),
					pv::util::message_box_icon(QMessageBox::Information), 500);
				return;
			}

			QUrl url = QUrl::fromLocalFile(pdfPath);

			bool opened = QDesktopServices::openUrl(url);
			if (!opened)
			{
				pv::util::show_message(this, tr("Open Failed"),
					tr("The PDF file could not be opened. Make sure a PDF reader is installed."),
					pv::util::message_box_icon(QMessageBox::Warning));
			}
		}

		void MainBar::on_action_about_triggered()
		{
			Application* a = qobject_cast<Application*>(QApplication::instance());
			auto version_info = a->get_version_info();
			QString appName = version_info[0].first;
			QString appVersion = version_info[0].second;
			QString text = QString(
						   "<b>%1 (%2)</b><br/>"
						   "<a href=\"https://oshwhub.com/q2h2\">Modified from: Q2H2</a><br/><br/>"
						   "<a href=\"https://%3\">%3</a>")
						   .arg(appName)
						   .arg(appVersion)
						   .arg(QApplication::organizationDomain());

			pv::util::show_message(this, tr("About"), text,
				QPixmap(":/icons/pulseview.png"),
				500, true, true);
		}

		/* CH32H417 device event handler - runs on the main thread */
		void MainBar::on_ch32h417_device_event(unsigned long eventStatus)
		{
			/* Only an arrival is reported: the driver-side notify is gone, and
			 * this is the upgrade flow saying the board is back. */
			if (eventStatus != CH375_DEVICE_ARRIVAL)
				return;

			if (upgrade_running())
			{
				end_upgrade();
				/* The board is back, so the page is free again: the pass
				 * below takes it over and says what the mask said until now. */
				Q_EMIT device_upgrading(false, -1);
				pv::util::show_message(this, tr("Update Success"),
					tr("Firmware Update Success!\nDevice already restart."),
					pv::util::message_box_icon(QMessageBox::Information),
					400, false, false, false);
			}

			/*
			 * The bus is read once the board has settled, not at the reset:
			 * the port it reports straight after one is still the one from
			 * before it came up, and a page bound to that cannot be found
			 * again - the next upgrade would not know which board to take.
			 * A capture in progress is left alone; the next change settles
			 * the page instead.
			 */
			QTimer::singleShot(UpgradeSettleMs, this, [this]() {
				if (captures_running())
					return;
				device_attached();
			});
		}

} // namespace toolbars

} // namespace pv
