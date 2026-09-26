/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnalyzer is based on PulseView.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
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

#ifdef ENABLE_DECODE
#include <libsigrokdecode/libsigrokdecode.h>
#endif
#include "mainwindow.hpp"
#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <climits>
#include <cstdint>
#include <iterator>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#ifdef __linux__
#include <unistd.h>
#endif
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QWidget>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>

#include <thread>
#include "application.hpp"
#include "devicemanager.hpp"
#include "devices/hardwaredevice.hpp"
#include "dialogs/settings.hpp"
#include "globalsettings.hpp"
#include "toolbars/mainbar.hpp"
#include "util.hpp"
#include "views/trace/view.hpp"
#include "views/trace/standardbar.hpp"
#include <fstream>
#include <QGuiApplication>

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#else
#include <sys/sysinfo.h>
#endif

using namespace std;
using namespace pv::util;
#ifdef ENABLE_DECODE
#include "views/decoder_binary/view.hpp"
#include "views/tabular_decoder/view.hpp"
#endif

#include <libsigrokcxx/libsigrokcxx.hpp>
using std::dynamic_pointer_cast;
using std::make_shared;
using std::shared_ptr;
using std::string;
namespace pv {

using toolbars::MainBar;
#define GB (1024*1024*1024)
QString MainWindow::WindowTitle;


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	session_selector_(this),
	icon_red_(":/icons/status-red.svg"),
	icon_green_(":/icons/status-green.svg"),
	icon_grey_(":/icons/status-grey.svg")
{
	WindowTitle = "WeAct Studio " + Application::applicationName() + " " + Application::applicationVersion();

	installEventFilter(this);
    qApp->installEventFilter(this);

	//Check memory size
	sigrok_context_ = sigrok::Context::create();
	device_manager_ = new pv::DeviceManager(sigrok_context_, "", true);
	sessions_.resize(0);
	setup_ui();
	restore_ui_settings();
}

bool MainWindow::checkSystemRequirements()
{
	int memFree = 0;

#ifdef _WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	memFree = statex.ullAvailPhys * 1.0 / GB;
#elif defined(__APPLE__)
	vm_size_t page_size;
	if (host_page_size(mach_host_self(), &page_size) == KERN_SUCCESS) {
		vm_statistics64_data_t vmstat;
		mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
		if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
		    (host_info64_t)&vmstat, &count) == KERN_SUCCESS) {
			const uint64_t free_mem =
				(vmstat.free_count + vmstat.inactive_count) *
				(uint64_t)page_size;
			memFree = free_mem / GB;
		}
	}
#else
	/*
	 * Linux: prefer /proc/meminfo MemAvailable, the accurate estimate of
	 * allocatable memory. sysinfo().freeram excludes the page cache and
	 * therefore under-reports on almost every system.
	 */
	long avail_kb = -1;
	FILE *f = fopen("/proc/meminfo", "r");
	if (f) {
		char line[256];
		while (fgets(line, sizeof(line), f)) {
			if (sscanf(line, "MemAvailable: %ld kB", &avail_kb) == 1)
				break;
		}
		fclose(f);
	}
	if (avail_kb >= 0)
		memFree = (int)(avail_kb / (1024 * 1024));
	else {
		struct sysinfo si;
		if (sysinfo(&si) == 0)
			memFree = (int)(si.freeram * si.mem_unit / GB);
	}
#endif

	if (memFree < 4){
		QDialog dialog(nullptr);
		dialog.setWindowTitle(tr("Warning"));

		// Use a larger font
		QFont font = dialog.font();
		font.setPointSize(font.pointSize() + 2);
		dialog.setFont(font);
		dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowStaysOnTopHint);

		QVBoxLayout *layout = new QVBoxLayout(&dialog);

		// Create the icon and text layout
		QHBoxLayout *contentLayout = new QHBoxLayout();

		// Create the icon label
		QLabel *iconLabel = new QLabel(&dialog);
		iconLabel->setPixmap(QMessageBox::standardIcon(QMessageBox::Information));
		iconLabel->setAlignment(Qt::AlignTop);
		contentLayout->addWidget(iconLabel);

		// Create the text label
		QLabel *label = new QLabel(&dialog);
		label->setText(tr("Current available memory size is less than 4GB and will affect usage effect. \nDo you want to continue to use?"));
		label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		contentLayout->addWidget(label, 1);

		layout->addLayout(contentLayout);

		// Create the button
		QHBoxLayout *buttonLayout = new QHBoxLayout();
		buttonLayout->addStretch();
		QPushButton *okBtn = new QPushButton(tr("Confirm"), &dialog);
		QPushButton *cancelBtn = new QPushButton(tr("Cancel"), &dialog);
		buttonLayout->addWidget(okBtn);
		buttonLayout->addWidget(cancelBtn);
		layout->addLayout(buttonLayout);

		// Connect the signal
		connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
		connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

		return (dialog.exec() == QDialog::Accepted);
	}
	return true;
}

bool MainWindow::checkUdevRules()
{
#ifndef __linux__
	return true;
#else
	if (pv::util::udev_rules_installed())
		return true;

	/* Already root: no password needed. */
	if (geteuid() == 0) {
		std::string err;
		if (pv::util::install_udev_rules(std::string(), err))
			return true;
	}

	const QString intro = MainWindow::tr(
		"No udev access rules were found for the CH32H417. Without them the "
		"device (VID 1a86 / PID 5537) cannot be opened by an unprivileged "
		"user, so the logic analyzer will not be detected.\n\n"
		"Enter your administrator password to install:\n"
		"/etc/udev/rules.d/99-weact-ch32h417.rules");

	/* Allow a couple of typos before giving up. */
	for (int attempt = 0; attempt < 3; ++attempt) {
		bool ok = false;
		const QString password = QInputDialog::getText(nullptr,
			MainWindow::tr("Administrator privileges required"), intro,
			QLineEdit::Password,
			QString(), &ok);
		if (!ok)
			break;

		std::string err;
		if (pv::util::install_udev_rules(password.toStdString(), err)) {
			pv::util::show_message(nullptr,
				MainWindow::tr("Access rules installed"),
				MainWindow::tr("The udev rules have been installed.\n\n"
					"If the device is already plugged in, unplug and replug "
					"it before scanning again."),
				QMessageBox::Information);
			return true;
		}

		QMessageBox::warning(nullptr, MainWindow::tr("Installation failed"),
			MainWindow::tr("Could not install the udev rules:\n\n%1\n\n"
				"Please try again, or cancel to skip.")
				.arg(QString::fromStdString(err)));
	}

	pv::util::show_message(nullptr,
		MainWindow::tr("Skipped installing access rules"),
		MainWindow::tr("The udev rules are not installed, so the device may "
			"not be detected.\n\nYou can run this application as root "
			"later, or copy 99-weact-ch32h417.rules from the repository to "
			"/etc/udev/rules.d/ and replug the device."),
		QMessageBox::Warning);

	/* Never refuse to start over this. */
	return true;
#endif
}

MainWindow::~MainWindow()
{
	// Make sure we no longer hold any shared pointers to widgets after the
	// destructor finishes (goes for sessions and sub windows alike)

	while (!sessions_.empty())
		remove_session(sessions_.front());
	sessions_.resize(0);
	sub_windows_.clear();
	if (device_manager_) {
		/* Created with new, so it has to go back with delete: free() would
		 * mismatch the allocator and skip the destructor. */
		delete device_manager_.load();
		device_manager_.store(nullptr);
	}
}

void MainWindow::show_session_error(const QString text, const QString info_text,
	QWidget *parent)
{
	// TODO Emulate noquote()
	qDebug() << "Notifying user of session error:" << info_text;

	/*
	 * Without a parent the box is a window of its own: it gets its own task
	 * bar entry, does not stay in front of the main window and does not
	 * inherit its state. Callers with no window at hand fall back to
	 * whatever the user is currently looking at.
	 */
	QWidget *owner = parent ? parent : QApplication::activeWindow();

	QMessageBox msg(owner);
	msg.setText(text + "\n\n" + info_text);
	msg.setStandardButtons(QMessageBox::Ok);
	msg.setIcon(QMessageBox::Warning);
	msg.exec();
}

void MainWindow::show_session_info(const QString info_text)
{
	QMessageBox * box = new QMessageBox(this);
	box->setText(info_text);
	box->setStandardButtons(QMessageBox::Ok);
	box->setIcon(QMessageBox::Warning);
	box->setModal(false);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->show();
}

void MainWindow::show_session_info_with_auto_close(const QString info_text)
{
    QMessageBox * box = new QMessageBox(this);
    box->setText(info_text);
    box->setStandardButtons(QMessageBox::Ok);
    box->setIcon(QMessageBox::Information);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);

    // 1000 ms = closes automatically after 1 second, change the value if needed
    QTimer::singleShot(1000, box, &QMessageBox::close);

    box->show();
}

shared_ptr<views::ViewBase> MainWindow::get_active_view() const
{
	// If there's only one view, use it...
	if (view_docks_.size() == 1)
		return view_docks_.begin()->second;

	// ...otherwise find the dock widget the widget with focus is contained in
	QObject *w = QApplication::focusWidget();
	QDockWidget *dock = nullptr;

	while (w) {
		dock = qobject_cast<QDockWidget*>(w);
		if (dock)
			break;
		w = w->parent();
	}

	// Get the view contained in the dock widget
	for (auto& entry : view_docks_)
		if (entry.first == dock)
			return entry.second;

	return nullptr;
}

shared_ptr<views::ViewBase> MainWindow::add_view(views::ViewType type,
	Session &session)
{
	GlobalSettings settings;
	shared_ptr<views::ViewBase> v;

	QMainWindow *main_window = nullptr;
	for (auto& entry : session_windows_)
		if (entry.first.get() == &session)
			main_window = entry.second;
	assert(main_window);

	shared_ptr<MainBar> main_bar = session.main_bar();

	// Only use the view type in the name if it's not the main view
	QString title;
	if (main_bar) {
		title = QString("%1").arg(tr(views::ViewTypeNames[type]));
	}
	else
		title = session.name();

	QDockWidget* dock = new QDockWidget(title, main_window);
	// dock->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);  //todo ???
	dock->setObjectName(title);

	if (type == views::ViewTypeTrace && !main_bar) {
		main_window->addDockWidget(Qt::LeftDockWidgetArea, dock);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	} else {
		main_window->addDockWidget(Qt::RightDockWidgetArea, dock);
		dock->setAllowedAreas(Qt::RightDockWidgetArea);
	}
	dock->setFeatures(QDockWidget::DockWidgetClosable);

	// Insert a QMainWindow into the dock widget to allow for a tool bar
	QMainWindow *dock_main = new QMainWindow(dock);
	dock_main->setWindowFlags(Qt::Widget);  // Remove Qt::Window flag

	if (type == views::ViewTypeTrace)
		// This view will be the main view if there's no main bar yet
		v = make_shared<views::trace::View>(session, (main_bar ? false : true), dock_main);
#ifdef ENABLE_DECODE
	if (type == views::ViewTypeDecoderBinary)
		v = make_shared<views::decoder_binary::View>(session, false, dock_main);
	if (type == views::ViewTypeTabularDecoder)
		v = make_shared<views::tabular_decoder::View>(session, false, dock_main);
#endif

	if (!v)
		return nullptr;
	if (title == session.name())
		dock->setTitleBarWidget(new QWidget());
	
	view_docks_[dock] = v;
	session.register_view(v);

	dock_main->setCentralWidget(v.get());
	dock->setWidget(dock_main);

	dock->setContextMenuPolicy(Qt::PreventContextMenu);

	QAbstractButton *close_btn =
		dock->findChildren<QAbstractButton*>("qt_dockwidget_closebutton")  // clazy:exclude=detaching-temporary
			.front();

	connect(close_btn, SIGNAL(clicked(bool)),
		this, SLOT(on_view_close_clicked()));

	connect(&session, SIGNAL(trigger_event(int, util::Timestamp)),
		qobject_cast<views::ViewBase*>(v.get()),
		SLOT(trigger_event(int, util::Timestamp)));
	connect(&session, SIGNAL(get_repeat_acquisition(bool&)), this, SLOT(on_get_repeat_acquisition(bool&)));
	connect(&session, SIGNAL(mainwindow_show_error(QString)), this, SLOT(on_show_error(QString)));
	connect(&session, SIGNAL(mainwindow_show_info(QString)), this, SLOT(show_session_info(QString)));
	connect(&session, SIGNAL(mainwindow_close_decoder_dock(Session *)), this, SLOT(on_close_decoder_dock(Session *)));
	if (type == views::ViewTypeTrace) {
		views::trace::View *tv =
			qobject_cast<views::trace::View*>(v.get());

		if (!main_bar) {
			/* Initial view, create the main bar */
			main_bar = make_shared<MainBar>(session, this, tv);
			dock_main->addToolBar(main_bar.get());
			main_bar->setMovable(false);
			// connect(main_bar.get(), SIGNAL(divice_detached()), &session, SLOT(on_device_detach()));
			connect(main_bar.get(), SIGNAL(divice_detached()), this, SLOT(show_device_detach()));
			connect(main_bar.get(), SIGNAL(device_attached()), this, SLOT(on_device_attached()));
			connect(this, SIGNAL(save_session()), main_bar.get(), SLOT(on_actionSave_triggered()));
			connect(main_bar.get(), SIGNAL(new_view(Session*, int)),
				this, SLOT(on_new_view(Session*, int)));
			connect(main_bar.get(), SIGNAL(show_decoder_selector(Session*)),
				this, SLOT(on_show_decoder_selector(Session*)));
			connect(main_bar.get(), SIGNAL(run_stop_button_clicked()), this, SLOT(on_run_stop_clicked()));
			connect(main_bar.get(), SIGNAL(settings_triggered()), this, SLOT(on_settings_clicked()));

			main_bar->action_view_show_cursors()->setChecked(tv->cursors_shown());
			session.set_main_bar(main_bar);
			/* For the main view we need to prevent the dock widget from
			 * closing itself when its close button is clicked. This is
			 * so we can confirm with the user first. Regular views don't
			 * need this */
			close_btn->disconnect(SIGNAL(clicked()), dock, SLOT(close()));
		} else {
			/* Additional view, create a standard bar */
			pv::views::trace::StandardBar *standard_bar =
				new pv::views::trace::StandardBar(session, this, tv);
			dock_main->addToolBar(standard_bar);
			standard_bar->setMovable(false);
			standard_bar->action_view_show_cursors()->setChecked(tv->cursors_shown());
		}
	}
	v->setFocus();
	return v;
}

void MainWindow::remove_view(shared_ptr<views::ViewBase> view)
{
	for (shared_ptr<Session> session : sessions_) {
		if (!session->has_view(view))
			continue;

		// Find the dock the view is contained in and remove it
		for (auto& entry : view_docks_)
			if (entry.second == view) {
				// Remove the view from the session
				session->deregister_view(view);

				// Remove the view from its parent; otherwise, Qt will
				// call deleteLater() on it, which causes a double free
				// since the shared_ptr in view_docks_ doesn't know
				// that Qt keeps a pointer to the view around
				view->setParent(nullptr);

				// Delete the view's dock widget and all widgets inside it
				entry.first->deleteLater();

				// Remove the dock widget from the list and stop iterating
				view_docks_.erase(entry.first);
				break;
			}
	}
}

shared_ptr<subwindows::SubWindowBase> MainWindow::add_subwindow(
	subwindows::SubWindowType type, Session &session)
{
	GlobalSettings settings;
	shared_ptr<subwindows::SubWindowBase> w;

	QMainWindow *main_window = nullptr;
	for (auto& entry : session_windows_)
		if (entry.first.get() == &session)
			main_window = entry.second;

	assert(main_window);

	QString title = "";

	switch (type) {
#ifdef ENABLE_DECODE
		case subwindows::SubWindowTypeDecoderSelector:
			title = tr("Decoder Selector");
			break;
#endif
		default:
			break;
	}

	QDockWidget* dock = new QDockWidget(title, main_window);
	dock->setObjectName(title);

	if (title == "") {
		dock->setTitleBarWidget(new QWidget());
		dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	}

	// main_window->addDockWidget(Qt::TopDockWidgetArea, dock);
	main_window->addDockWidget(Qt::RightDockWidgetArea, dock);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	dock->setFeatures(QDockWidget::DockWidgetClosable);


	// Insert a QMainWindow into the dock widget to allow for a tool bar
	QMainWindow *dock_main = new QMainWindow(dock);
	dock_main->setWindowFlags(Qt::Widget);  // Remove Qt::Window flag

#ifdef ENABLE_DECODE
	if (type == subwindows::SubWindowTypeDecoderSelector){
		w = make_shared<subwindows::decoder_selector::SubWindow>(session, dock_main);
	}
#endif

	if (!w)
		return nullptr;

	sub_windows_[dock] = w;
	dock_main->setCentralWidget(w.get());
	dock->setWidget(dock_main);

	dock->setContextMenuPolicy(Qt::PreventContextMenu);

	QAbstractButton *close_btn =
		dock->findChildren<QAbstractButton*>  // clazy:exclude=detaching-temporary
			("qt_dockwidget_closebutton").front();

	// Allow all subwindows to be closed via ESC.
	close_btn->setShortcut(QKeySequence(Qt::Key_Escape));

	connect(close_btn, SIGNAL(clicked(bool)),
		this, SLOT(on_sub_window_close_clicked()));

	if (w->has_toolbar()) {
		QToolBar *toolbar = w->create_toolbar(dock_main);
		dock_main->addToolBar(toolbar);
		toolbar->setMovable(false);
	}

	if (w->minimum_width() > 0) {
		dock->setMinimumSize(w->minimum_width(), 0);
		dock->setMaximumWidth(350);
	}
	return w;
}

shared_ptr<Session> MainWindow::add_session()
{
	static int last_session_id = 1;
	QString name = tr("Session %1").arg(last_session_id++);

	shared_ptr<Session> session = make_shared<Session>(*device_manager_, name);

	connect(session.get(), SIGNAL(add_view(ViewType, Session*)),
		this, SLOT(on_add_view(ViewType, Session*)));
	connect(session.get(), SIGNAL(name_changed()),
		this, SLOT(on_session_name_changed()));
	connect(session.get(), SIGNAL(device_changed()),
		this, SLOT(on_session_device_changed()));
	connect(session.get(), SIGNAL(capture_state_changed(int)),
		this, SLOT(on_session_capture_state_changed(int)));
	/*
	 * Queued on purpose: the signal comes from the datafeed thread, and a
	 * capture must not be stopped from inside the feed callback.
	 */
	connect(session.get(), SIGNAL(sample_rate_too_low(qulonglong, qulonglong)),
		this, SLOT(on_sample_rate_too_low(qulonglong, qulonglong)),
		Qt::QueuedConnection);

	sessions_.push_back(session);
	
	QMainWindow *window = new QMainWindow();
	window->setWindowFlags(Qt::Widget);  // Remove Qt::Window flag
	session_windows_[session] = window;

	int index = session_selector_.addTab(window, name);
	session_selector_.setCurrentIndex(index);
	last_focused_session_ = session;

	window->setDockNestingEnabled(false);

	add_view(views::ViewTypeTrace, *session);
	return session;
}

/*
 * The CH32H417 is reachable through either the vendor CH375 driver or the
 * libusb one, depending on which driver the OS bound the device to. Prefer
 * whichever backend libsigrok actually has, so the name is not hardcoded to
 * a single transport.
 */
static string ch32h417_driver_name(const shared_ptr<sigrok::Context>& context)
{
	const std::vector<std::string>& names = pv::util::get_ch32h417_driver_names();
	const auto drivers = context->drivers();

	for (const std::string& name : names)
		if (drivers.find(name) != drivers.end())
			return name;

	return names.front();
}

void MainWindow::do_device_detach_switch()
{
	if (!last_focused_session_ || !last_focused_session_->main_bar()) {
		return;
	}

	last_focused_session_->device_detached();

	/* Reuse the global context: a second Context would sr_exit() the
	 * shared backend when destroyed, freeing every device instance. */
	shared_ptr<sigrok::Context> context = sigrok_context_;
	string driver = ch32h417_driver_name(context);
	auto new_device_manager = new pv::DeviceManager(context, driver, true);
	if (device_manager_) {
		delete device_manager_.load();
	}
	device_manager_.store(new_device_manager);
	update_sessions_device_manager(new_device_manager);

	list< shared_ptr<devices::HardwareDevice> > devices = new_device_manager->devices();

	if (!devices.empty()) {
		shared_ptr<devices::Device> default_device = devices.front();
		WchDeviceType found_device_type = WchDeviceType::None;

		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type != WchDeviceType::None) {
				found_device_type = dev_type;
				default_device = dev;
				break;
			}
		}

		bool is_demo_device = false;
		shared_ptr<devices::HardwareDevice> hw_dev =
			std::dynamic_pointer_cast<devices::HardwareDevice>(default_device);
		if (hw_dev && hw_dev->hardware_device()->driver()->name() == "demo") {
			is_demo_device = true;
		}

		last_focused_session_->select_device(default_device);
		if (default_device) {
			if (found_device_type != WchDeviceType::None) {
				QString device_name = DeviceModelNames[static_cast<int>(found_device_type)];
				QString vendor_name = device_name;
				last_focused_session_->main_bar()->set_device_selector_name(device_name);
				last_focused_session_->main_bar()->set_vendorName(vendor_name);
				restore_sessions();
			} else if (is_demo_device) {
				last_focused_session_->main_bar()->set_device_selector_name("Demo device");
				last_focused_session_->main_bar()->set_vendorName("Demo device");
				last_focused_session_->create_demo_uart_decoder();
			}
		}
	} else {
		last_focused_session_->main_bar()->set_vendorName("No Device");
	}
}

void MainWindow::show_device_detach()
{
	if (!last_focused_session_ || !last_focused_session_->main_bar()) {
		return;
	}

	attach_process_done = false;

	QDialog* dialog = new QDialog(this);
	dialog->setWindowTitle(tr("Warning"));

	dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Use a larger font
	QFont font = dialog->font();
	font.setPointSize(font.pointSize() + 2);
	dialog->setFont(font);

	QVBoxLayout *layout = new QVBoxLayout(dialog);
	layout->setContentsMargins(28, 24, 28, 20);
	layout->setSpacing(20);

	// Create the icon and text layout
	QHBoxLayout *contentLayout = new QHBoxLayout();
	contentLayout->setSpacing(18);

	// Create the icon label
	QLabel *iconLabel = new QLabel(dialog);
	iconLabel->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning));
	iconLabel->setAlignment(Qt::AlignTop);
	contentLayout->addWidget(iconLabel);

	// Create the text label
	QLabel *label = new QLabel(dialog);
	label->setText(tr("device detached!\nDo you want to save the session?"));
	label->setStyleSheet(QString("font-size: %1pt;").arg(font.pointSize()));
	label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	contentLayout->addWidget(label, 1);

	layout->addLayout(contentLayout);

	// Create the button
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setSpacing(12);
	buttonLayout->addStretch();
	QPushButton *saveBtn = new QPushButton(tr("Save"), dialog);
	QPushButton *cancelBtn = new QPushButton(tr("Cancel"), dialog);
	saveBtn->setMinimumWidth(90);
	cancelBtn->setMinimumWidth(90);
	buttonLayout->addWidget(saveBtn);
	buttonLayout->addWidget(cancelBtn);
	layout->addLayout(buttonLayout);

	// Size to contents and prevent the window from being resized
	dialog->setFixedSize(dialog->sizeHint());

	// Show modelessly
	dialog->setModal(false);

	if (last_focused_session_->main_bar()->channels_ &&
		is_wch_device(last_focused_session_->main_bar()->channels_->device_name_)) {
		save_sessions();
		last_focused_session_->main_bar()->set_setting_default();
	}

	connect(saveBtn, &QPushButton::clicked, [=](){
		qDebug() << last_focused_session_->main_bar()->channels_->device_name_;
		qDebug() << last_focused_session_->main_bar()->channels_;
		
		last_focused_session_->main_bar()->action_save()->triggered();
		do_device_detach_switch();
		attach_process_done = true;
		dialog->deleteLater();
	});

	connect(cancelBtn, &QPushButton::clicked, [=](){
		do_device_detach_switch();
		attach_process_done = true;
		dialog->deleteLater();
	});

	dialog->show();
}

void sleep_ex(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < dieTime ){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void MainWindow::on_device_attached()
{
	if (!attach_process_done)
		return;
	attach_process_done = false;

	if (last_focused_session_ && last_focused_session_->device()) {
		last_focused_session_->stop_capture();
		/*
		 * The previous device may already be gone, and libsigrok reports that
		 * by throwing. Letting it escape from a Qt slot would terminate the
		 * application, so swallow it: we are about to rescan anyway.
		 */
		try {
			last_focused_session_->device()->close();
		} catch (const sigrok::Error &e) {
			qWarning() << "Error closing the previous device:" << e.what();
		} catch (const QString &msg) {
			qWarning() << "Error closing the previous device:" << msg;
		}
	}

	// Reuse the global context: a second Context would sr_exit() the
	// shared backend when destroyed, freeing every device instance.
	shared_ptr<sigrok::Context> context;
	context = sigrok_context_;
	string driver = ch32h417_driver_name(context);

	auto device_manager = new pv::DeviceManager(context, driver, true);
	if (device_manager_)
		delete device_manager_.load();
	device_manager_.store(device_manager);
	update_sessions_device_manager(device_manager);

	list< shared_ptr<devices::HardwareDevice> > devices =
		device_manager->devices();

	QString currentVendorName = last_focused_session_->main_bar()->get_vendorName();
	bool is_using_ch569 = (currentVendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH569)]);
	bool is_using_ch32h417 = (currentVendorName == DeviceModelNames[static_cast<int>(WchDeviceType::CH32H417)]);

	bool current_device_still_exists = false;
	if (is_using_ch569) {
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type == WchDeviceType::CH569) {
				current_device_still_exists = true;
				break;
			}
		}
	} else if (is_using_ch32h417) {
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type == WchDeviceType::CH32H417) {
				current_device_still_exists = true;
				break;
			}
		}
	}

	if (current_device_still_exists) {
		shared_ptr<devices::Device> current_type_device = nullptr;
		WchDeviceType target_type = is_using_ch569 ? WchDeviceType::CH569 : WchDeviceType::CH32H417;

		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type == target_type) {
				current_type_device = dev;
				break;
			}
		}

		if (current_type_device) {
			last_focused_session_->select_device(current_type_device);
		}
		last_focused_session_->main_bar()->update_device_list();
		restore_sessions();
	} else if (!devices.empty()) {
		shared_ptr<devices::Device> default_device = nullptr;
		WchDeviceType found_device_type = WchDeviceType::None;

		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type != WchDeviceType::None) {
				found_device_type = dev_type;
				default_device = dev;
				break;
			}
		}

		if (default_device) {
			last_focused_session_->select_device(default_device);
			QString device_name = DeviceModelNames[static_cast<int>(found_device_type)];
			QString vendor_name = device_name;
			last_focused_session_->main_bar()->set_device_selector_name(device_name);
			last_focused_session_->main_bar()->set_vendorName(vendor_name);
			restore_sessions();
		} else {
			/*
			 * Only the demo device is left. The device that was in use has
			 * just been closed, so select the demo device again to keep the
			 * session usable. This is where a manual rescan without any
			 * hardware attached ends up.
			 */
			const shared_ptr<devices::Device> previous =
				last_focused_session_->device();
			const shared_ptr<devices::HardwareDevice> previous_hw =
				dynamic_pointer_cast<devices::HardwareDevice>(previous);

			/* Leave a file device that is in use alone. */
			if (!previous || (previous_hw &&
					previous_hw->hardware_device()->driver()->name() == "demo"))
				last_focused_session_->select_device(nullptr);
		}
	}

	attach_process_done = true;
}

void MainWindow::update_sessions_device_manager(pv::DeviceManager *device_manager)
{
	for (const shared_ptr<Session>& session : sessions_)
		if (session)
			session->update_device_manager(device_manager);

	/* The focused session may already have been taken out of the list. */
	if (last_focused_session_)
		last_focused_session_->update_device_manager(device_manager);
}

void MainWindow::remove_session(shared_ptr<Session> session)
{
	// Determine the height of the button before it collapses
	// int h = new_session_button_->height();
	int h = 22;
	// Stop capture while the session still exists so that the UI can be
	// updated in case we're currently running. If so, this will schedule a
	// call to our on_capture_state_changed() slot for the next run of the
	// event loop. We need to have this executed immediately or else it will
	// be dismissed since the session object will be deleted by the time we
	// leave this method and the event loop gets a chance to run again.
	session->stop_capture();
	QApplication::processEvents();

	for (const shared_ptr<views::ViewBase>& view : session->views())
		remove_view(view);

	QMainWindow *window = session_windows_.at(session);
	session_selector_.removeTab(session_selector_.indexOf(window));

	session_windows_.erase(session);

	if (last_focused_session_ == session)
		last_focused_session_.reset();

	// Remove the session from our list of sessions (which also destroys it)
	sessions_.remove_if([&](shared_ptr<Session> s) {
		return s == session; });

	if (sessions_.empty()) {
		// When there are no more tabs, the height of the QTabWidget
		// drops to zero. We must prevent this to keep the static
		// widgets visible
		for (QWidget *w : static_tab_widget_->findChildren<QWidget*>())  // clazy:exclude=range-loop
			w->setMinimumHeight(h);

		int margin = static_tab_widget_->layout()->contentsMargins().bottom();
		static_tab_widget_->setMinimumHeight(h + 2 * margin);
		session_selector_.setMinimumHeight(h + 2 * margin);

		// Update the window title if there is no view left to
		// generate focus change events
		setWindowTitle(WindowTitle);
	}
}

void MainWindow::add_default_setting()
{
	if (!last_focused_session_ || !last_focused_session_->main_bar()) {
		return;
	}

	if (last_focused_session_->main_bar()->channels_ &&
		is_wch_device(last_focused_session_->main_bar()->channels_->device_name_)){
		QSettings settings;
    	if (!settings.contains("Settings/sample_count_index"))   //not exist setting
			last_focused_session_->main_bar()->on_add_decoder_clicked();
		if (last_focused_session_->main_bar()->is_repeat_acq_default_ == 1){
			repeat_acquisition_button_->setChecked(true);
			is_repeat_acq_ = true;
		}
		else{
			repeat_acquisition_button_->setChecked(false);
			is_repeat_acq_ = false;
		}
	}
	last_focused_session_->main_bar()->hot_plug_start();
}

void MainWindow::add_session_with_file(string open_file_name,
	string open_file_format, string open_setup_file_name)
{
	shared_ptr<Session> session = add_session();
	session->load_init_file(open_file_name, open_file_format, open_setup_file_name);
}

void MainWindow::add_default_session()
{
	// Only add the default session if there would be no session otherwise
	// if (sessions_.size() > 0)
	// 	return;
	WchDeviceType found_device_type = WchDeviceType::None;
	list< shared_ptr<devices::HardwareDevice> > devices =
		device_manager_.load()->devices();
	for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
		WchDeviceType dev_type = get_wch_device_type_by_driver(
			dev->hardware_device()->driver()->name());
		if (dev_type != WchDeviceType::None) {
			found_device_type = dev_type;
            break;
        }
	}
	qDebug() << "add_default_session devices size: " << devices.size();
	if (found_device_type == WchDeviceType::None){
		sleep_ex(100);
		/* Reuse the global context: creating a second Context here would
		 * destroy the shared libsigrok backend (sr_exit) as soon as the
		 * old devices are released, freeing the freshly scanned ones too. */
		shared_ptr<sigrok::Context> context;
		context = sigrok_context_;
		string driver = ch32h417_driver_name(context);
		if (device_manager_)
			delete device_manager_.load();
		device_manager_ = new pv::DeviceManager(context, driver, true);
		update_sessions_device_manager(device_manager_.load());
	}

	shared_ptr<Session> session = nullptr;
	if( sessions_.size() == 0){
		session = add_session();
	}else{
		session = sessions_.front();
	}
	session->update_device_manager(device_manager_);
	shared_ptr<devices::HardwareDevice> wch_device = nullptr;
	shared_ptr<devices::HardwareDevice> demo_device = nullptr;
	shared_ptr<devices::HardwareDevice> default_device = nullptr;
	devices = device_manager_.load()->devices();
	// Check the list of available devices. Prefer the one that was
	// found with user supplied scan specs (if applicable). Then try
	// one of the auto detected devices that are not the demo device.
	// Pick demo in the absence of "genuine" hardware devices.
	// shared_ptr<devices::HardwareDevice> user_device, other_device, demo_device;
	if (!devices.empty()) {
		default_device = devices.front();
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			WchDeviceType dev_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			if (dev_type != WchDeviceType::None) {
				found_device_type = dev_type;
				wch_device = dev;
				default_device = dev;
				break;
			}
		}
		// Find demo device as fallback
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			if (string(dev->hardware_device()->driver()->name()) == "demo") {
				demo_device = dev;
				break;
			}
		}
		// If no WCH device found and demo exists, use demo
		if (!wch_device && demo_device)
			default_device = demo_device;
	}
	session->select_device(default_device);

	// If WCH device failed to open, fall back to demo device
	if (wch_device && !session->device() && demo_device) {
		qDebug() << "WCH device open failed, falling back to demo device";
		found_device_type = WchDeviceType::None;
		session->select_device(demo_device);

		if (session->device() && session->main_bar()) {
			session->main_bar()->set_vendorName("Demo device");
			session->main_bar()->set_device_selector_name("Demo device");
		}
	}

	// Check if using demo device
	bool is_demo_device = false;
	if (session->device()) {
		shared_ptr<devices::HardwareDevice> hw_dev =
			std::dynamic_pointer_cast<devices::HardwareDevice>(session->device());
		if (hw_dev && string(hw_dev->hardware_device()->driver()->name()) == "demo") {
			is_demo_device = true;
		}
	}

	if (is_demo_device) {
		// Demo device: create default UART decoder, don't restore saved decoders
		if (session->main_bar()) {
			session->main_bar()->set_vendorName("Demo device");
			session->main_bar()->set_device_selector_name("Demo device");
		}
		session->create_demo_uart_decoder();

		QTimer::singleShot(10, [this, session]() {
			if (session && session->get_capture_state() == Session::Stopped) {
				session->start_capture([](QString message) {
					qDebug() << "Demo device auto-start capture: " << message;
				});
			}
		});
	} else if (found_device_type != WchDeviceType::None) {
		// WCH device: show hardware version
		if (session->main_bar())
			session->main_bar()->show_hardware_version();
		else
			qDebug() << "session->main_bar() == nullptr";
	}
}
void MainWindow::save_sessions()
{
	QSettings settings;
	int ch569_id = 0, ch32h417_id = 0;

	for (shared_ptr<Session>& session : sessions_) {
		// Ignore sessions using the demo device or no device at all
		if (session->device()) {
			shared_ptr<devices::HardwareDevice> device =
				dynamic_pointer_cast< devices::HardwareDevice >
				(session->device());

			if (device && get_wch_device_type_by_driver(
				device->hardware_device()->driver()->name()) == WchDeviceType::None){
				continue;
			}

			WchDeviceType dev_type = get_wch_device_type_by_driver(
				device->hardware_device()->driver()->name());
			QString group_name;
			if (dev_type == WchDeviceType::CH569) {
				group_name = "Session_CH569_" + QString::number(ch569_id++);
			} else if (dev_type == WchDeviceType::CH32H417) {
				group_name = "Session_CH32H417_" + QString::number(ch32h417_id++);
			} else {
				group_name = "Session_" + QString::number(ch569_id + ch32h417_id);
			}

			settings.beginGroup(group_name);
			settings.remove("");  // Remove all keys in this group
			session->save_settings(settings);
			settings.endGroup();
		}
	}
	settings.setValue("sessions_ch569", ch569_id);
	settings.setValue("sessions_ch32h417", ch32h417_id);
}

void MainWindow::restore_sessions()
{
	QSettings settings;
	int ch569_count = settings.value("sessions_ch569", 0).toInt();
	int ch32h417_count = settings.value("sessions_ch32h417", 0).toInt();

	for (int i = 0; i < ch569_count; i++) {
		settings.beginGroup("Session_CH569_" + QString::number(i));
		if (last_focused_session_ != NULL) {
			last_focused_session_->restore_settings(settings);
		} else {
			last_focused_session_ = add_session();
		}
		settings.endGroup();
	}

	for (int i = 0; i < ch32h417_count; i++) {
		settings.beginGroup("Session_CH32H417_" + QString::number(i));
		if (last_focused_session_ != NULL) {
			last_focused_session_->restore_settings(settings);
		} else {
			last_focused_session_ = add_session();
		}
		settings.endGroup();
	}

	if (last_focused_session_)
		last_focused_session_->main_bar()->renew_setting_default();
}

void MainWindow::setup_ui()
{
	setObjectName(QString::fromUtf8("MainWindow"));

	setCentralWidget(&session_selector_);

	// Set the window icon
	QIcon icon;
	icon.addFile(QString(":/icons/pulseview.png"));
	setWindowIcon(icon);

	// Set up keyboard shortcuts that affect all views at once
	view_sticky_scrolling_shortcut_ = new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(on_view_sticky_scrolling_shortcut()));
	view_sticky_scrolling_shortcut_->setAutoRepeat(false);

	view_show_sampling_points_shortcut_ = new QShortcut(QKeySequence(Qt::Key_S), this, SLOT(on_view_show_sampling_points_shortcut()));
	view_show_sampling_points_shortcut_->setAutoRepeat(false);

	view_show_analog_minor_grid_shortcut_ = new QShortcut(QKeySequence(Qt::Key_G), this, SLOT(on_view_show_analog_minor_grid_shortcut()));
	view_show_analog_minor_grid_shortcut_->setAutoRepeat(false);

	view_colored_bg_shortcut_ = new QShortcut(QKeySequence(Qt::Key_B), this, SLOT(on_view_colored_bg_shortcut()));
	view_colored_bg_shortcut_->setAutoRepeat(false);

	// Set up the tab area
	// new_session_button_ = new QToolButton();
	// new_session_button_->setIcon(QIcon::fromTheme("document-new",
		// QIcon(":/icons/document-new.png")));
	// new_session_button_->setToolTip(tr("Create New Session"));
	// new_session_button_->setAutoRaise(true);

	run_stop_button_ = new QToolButton();
	run_stop_button_->setAutoRaise(true);
	run_stop_button_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	run_stop_button_->setToolTip(tr("Start/Stop Acquisition"));
	
	repeat_acquisition_button_ = new QCheckBox();
	repeat_acquisition_button_->setToolTip(tr("Repeat Acquisition"));
	repeat_acquisition_button_->setText(tr("Repeat Acquisition"));

	run_stop_shortcut_ = new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(on_run_stop_clicked()));
	run_stop_shortcut_->setAutoRepeat(false);

	settings_button_ = new QToolButton();
	settings_button_->setIcon(QIcon(":/icons/preferences-system.png"));
	settings_button_->setToolTip(tr("Settings"));
	settings_button_->setAutoRaise(true);

	QFrame *separator = new QFrame();
	separator->setFrameStyle(QFrame::VLine | QFrame::Raised);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	// layout->addWidget(new_session_button_);
	layout->addWidget(run_stop_button_);
	layout->addWidget(repeat_acquisition_button_);
	layout->addWidget(separator);
	layout->addWidget(settings_button_);

	static_tab_widget_ = new QWidget();
	int newHeight = session_selector_.height();
	static_tab_widget_->setFixedHeight(newHeight);
	static_tab_widget_->setLayout(layout);
	session_selector_.tabBar()->setVisible(false);  // hidden session 1
	// session_selector_.setCornerWidget(static_tab_widget_, Qt::TopLeftCorner);
	session_selector_.setTabsClosable(false);
	close_application_shortcut_ = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
	close_application_shortcut_->setAutoRepeat(false);

	// close_current_tab_shortcut_ = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this, SLOT(on_close_current_tab()));

	
	// connect(new_session_button_, SIGNAL(clicked(bool)),
	// 	this, SLOT(on_new_session_clicked()));
	connect(run_stop_button_, SIGNAL(clicked(bool)),
		this, SLOT(on_run_stop_clicked()));
	connect(repeat_acquisition_button_, SIGNAL(clicked()),
		this, SLOT(on_repeat_acquisition_clicked()));
	connect(settings_button_, SIGNAL(clicked(bool)),
		this, SLOT(on_settings_clicked()));

	connect(&session_selector_, SIGNAL(tabCloseRequested(int)),
		this, SLOT(on_tab_close_requested(int)));
	connect(&session_selector_, SIGNAL(currentChanged(int)),
		this, SLOT(on_tab_changed(int)));

	connect(static_cast<QApplication *>(QCoreApplication::instance()),
		SIGNAL(focusChanged(QWidget*, QWidget*)),
		this, SLOT(on_focus_changed()));
	//connect(this, SIGNAL(show_error(QString)), this, SLOT(on_show_error(QString)));
}

void MainWindow::on_repeat_acquisition_clicked()
{
	if (repeat_acquisition_button_->isChecked())
		is_repeat_acq_ = true;
	else
		is_repeat_acq_ = false;
	last_focused_session_->main_bar()->set_setting_default();
}

void MainWindow::update_acq_button(Session *session)
{
	int state;
	QString run_caption;

	if (session) {
		state = session->get_capture_state();
		run_caption = session->using_file_device() ? tr("Reload") : tr("Run");
		if (session->using_file_device()){
			repeat_acquisition_button_->setChecked(false);
			session->is_repeat_acquisition_ = false;
		}
			
	} else {
		state = Session::Stopped;
		run_caption = tr("Run");
	}

	const QIcon *icons[] = {&icon_grey_, &icon_red_, &icon_green_};
	run_stop_button_->setIcon(*icons[state]);
	run_stop_button_->setText((state == pv::Session::Stopped) ?
		run_caption : tr("Stop"));
	if (last_focused_session_ && last_focused_session_->main_bar()) {
		last_focused_session_->main_bar()->update_runstop_status(session->is_repeat_acquisition_?Session::Running : state);
	}
}

void MainWindow::save_ui_settings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");
	settings.setValue("state", saveState());
	settings.setValue("geometry", saveGeometry());
	settings.endGroup();
}

void MainWindow::restore_ui_settings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");

	if (settings.contains("geometry")) {
		restoreGeometry(settings.value("geometry").toByteArray());
		restoreState(settings.value("state").toByteArray());
	} else
		resize(1400, 900);

	settings.endGroup();
}

shared_ptr<Session> MainWindow::get_tab_session(int index) const
{
	// Find the session that belongs to the tab's main window
	for (auto& entry : session_windows_)
		if (entry.second == session_selector_.widget(index))
			return entry.first;

	return nullptr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	bool data_saved = true;
	QSettings global_settings;
	for (auto& entry : session_windows_) {
		if (!entry.first->data_saved()) {
			data_saved = false;
			QString n = entry.first->name();
			if (n.isEmpty())
				n = tr("Untitled");
		}
	}

	if (!data_saved)
	{
		QDialog dialog(this);
		dialog.setWindowTitle(tr("Warning"));

		// Use a larger font
		QFont font = dialog.font();
		font.setPointSize(font.pointSize() + 2);
		dialog.setFont(font);
		dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

		QVBoxLayout *layout = new QVBoxLayout(&dialog);

		// Create the label
		QLabel *label = new QLabel(&dialog);
		label->setText(tr("<h3>Unsaved Capture Data</h3><p>Data will be permanently lost on exit.</p>"));
		label->setTextFormat(Qt::RichText);
		label->setAlignment(Qt::AlignCenter);
		layout->addWidget(label);

		// Create the button
		QHBoxLayout *buttonLayout = new QHBoxLayout();
		QPushButton *discardBtn = new QPushButton(tr("Discard && Exit"), &dialog);
		QPushButton *cancelBtn = new QPushButton(tr("Cancel"), &dialog);

		buttonLayout->addStretch();
		buttonLayout->addWidget(discardBtn);
		buttonLayout->addWidget(cancelBtn);
		layout->addLayout(buttonLayout);

		// Connect the signal
		connect(discardBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
		connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

		// Run the dialog
		if (dialog.exec() == QDialog::Rejected)
		{
			event->ignore();
			return;
		}
	}
	if (last_focused_session_ && is_wch_device(last_focused_session_->main_bar()->channels_->device_name_)){
		save_sessions();
		last_focused_session_->main_bar()->set_setting_default();
	}
	save_ui_settings();
}

QMenu* MainWindow::createPopupMenu()
{
	return nullptr;
}

bool MainWindow::restoreState(const QByteArray &state, int version)
{
	(void)state;
	(void)version;

	// Do nothing. We don't want Qt to handle this, or else it
	// will try to restore all the dock widgets and create havoc.

	return false;
}

void MainWindow::on_show_error(QString err)
{
	// last_focused_session_->device_detach();
	show_session_error(tr("Capture Failed"), err, this);
}

/*
 * Sample rates are spelled the way the rate tables list them (192.5 MHz rather
 * than 193 MHz), so that the number in the message can be compared with the
 * setting directly.
 */
static QString format_sample_rate(qulonglong rate)
{
	QString number = (rate >= 1000000) ?
		QString::number(rate / 1e6, 'f', 3) :
		QString::number(rate / 1e3, 'f', 3);

	while (number.contains('.') && number.endsWith('0'))
		number.chop(1);
	if (number.endsWith('.'))
		number.chop(1);

	return number + ((rate >= 1000000) ? " MHz" : " kHz");
}

/*
 * The driver had to stop the capture because the device stopped sending
 * samples early, which is what a link that cannot carry the configured
 * rate looks like from here. The data that did arrive may be missing
 * samples, and that leaves no trace in the waveform, so the user has to be
 * told instead of being shown a capture that looks complete.
 */
void MainWindow::on_sample_rate_too_low(qulonglong configured, qulonglong reason)
{
	Session *session = qobject_cast<Session *>(sender());
	if (session) {
		/*
		 * Stop a repeated acquisition as well: otherwise the next capture
		 * is started from the event queue while this message is open.
		 */
		session->is_repeat_acquisition_ = false;
		session->stop_capture();

		/*
		 * The button was put back to "Stop" when the state change was
		 * announced, because the repeat was still marked as running at
		 * that point. Refresh it now that the repeat has really ended.
		 */
		if (session->main_bar())
			session->main_bar()->update_runstop_status(Session::Stopped);
		if (session == last_focused_session_.get()) {
			run_stop_button_->setIcon(icon_grey_);
			run_stop_button_->setText(session->using_file_device() ?
				tr("Reload") : tr("Run"));
		}
	}

	/*
	 * The message box below runs its own event loop, so another one of
	 * these can be delivered while it is open. One warning is enough.
	 */
	if (link_stop_message_visible_)
		return;
	link_stop_message_visible_ = true;

	qDebug() << "Capture stopped by the driver, reason" << reason
		<< "at" << configured << "Hz";

	show_session_error(tr("Device stopped sending data"),
		tr("The device stopped sending samples before the capture was due to "
		   "end, usually because the USB link cannot sustain the configured %1.\n\n"
		   "The acquisition was stopped. The data captured before that may "
		   "be incomplete or wrong. Try a lower sample rate, fewer "
		   "channels, or another USB port.")
			.arg(format_sample_rate(configured)), this);

	link_stop_message_visible_ = false;
}

void MainWindow::on_get_repeat_acquisition(bool& is_repeat_acquisition_state)
{
	if (last_focused_session_ && last_focused_session_->main_bar()) {
		is_repeat_acquisition_state = last_focused_session_->main_bar()->capture_mode_ == Session::Repeat;
	} else {
		is_repeat_acquisition_state = false;
	}
}

void MainWindow::on_run_stop_clicked()
{
	GlobalSettings settings;
	bool all_sessions = settings.value(GlobalSettings::Key_General_StartAllSessions).toBool();
	if (all_sessions)
	{
		vector< shared_ptr<Session> > hw_sessions;
		// Make a list of all sessions where a hardware device is used
		for (const shared_ptr<Session>& s : sessions_) {
			shared_ptr<devices::HardwareDevice> hw_device =
					dynamic_pointer_cast< devices::HardwareDevice >(s->device());
			if (!hw_device)
				continue;
			hw_sessions.push_back(s);
		}

		// Stop all acquisitions if there are any running ones, start all otherwise
		bool any_running = any_of(hw_sessions.begin(), hw_sessions.end(),
				[](const shared_ptr<Session> &s)
				{ return (s->get_capture_state() == Session::AwaitingTrigger) ||
						(s->get_capture_state() == Session::Running); });

		for (shared_ptr<Session> s : hw_sessions){
			s->is_repeat_acquisition_ = last_focused_session_->main_bar()->capture_mode_ == Session::Repeat;
			if (any_running)
				s->stop_capture();
			else
				s->start_capture([&](QString message) {
					// show_session_error("Capture failed", message); 
					qDebug() << "Capture failed";
				});
		}		
	} else {
		shared_ptr<Session> session = last_focused_session_;
		if (!session)
			return;
		switch (session->get_capture_state()) {
		case Session::Stopped:
			if(session->is_repeat_acquisition_)
			{
				session->stop_capture();
				break;
			}
			session->is_repeat_acquisition_ = last_focused_session_->main_bar()->capture_mode_ == Session::Repeat;
			if (session->is_repeat_acquisition_)
				qDebug() << "Repeat acquisition is already";
			qDebug() << "Start acquisition";
			session->start_capture([&](QString message) {
				// show_session_error("Capture failed", message);
				qDebug() << "Capture failed";
			 });
			break;
		case Session::AwaitingTrigger:
		case Session::Running:
			session->stop_capture();
			break;
		}
	}
}

void MainWindow::on_add_view(views::ViewType type, Session *session)
{
	// We get a pointer and need a reference
	for (shared_ptr<Session>& s : sessions_)
		if (s.get() == session)
			add_view(type, *s);
}

void MainWindow::on_focus_changed()
{
	shared_ptr<views::ViewBase> view = get_active_view();

	if (view) {
		for (shared_ptr<Session> session : sessions_) {
			if (session->has_view(view)) {
				if (session != last_focused_session_) {
					// Activate correct tab if necessary
					shared_ptr<Session> tab_session = get_tab_session(
						session_selector_.currentIndex());
					if (tab_session != session)
						session_selector_.setCurrentWidget(
							session_windows_.at(session));

					on_focused_session_changed(session);
				}

				break;
			}
		}
	}

	if (sessions_.empty())
		setWindowTitle(WindowTitle);
}

void MainWindow::on_focused_session_changed(shared_ptr<Session> session)
{
	last_focused_session_ = session;

	// setWindowTitle(WindowTitle + " - " + session->name());
	setWindowTitle(WindowTitle);

	// Update the state of the run/stop button, too
	update_acq_button(session.get());
}

void MainWindow::on_new_session_clicked()
{
	add_session();
}

void MainWindow::on_settings_clicked()
{
	dialogs::Settings dlg(*device_manager_);
	dlg.exec();
}

void MainWindow::on_session_name_changed()
{
	// Update the corresponding dock widget's name(s)
	Session *session = qobject_cast<Session*>(QObject::sender());
	assert(session);
	if (session == nullptr)
		return;

	for (const shared_ptr<views::ViewBase>& view : session->views()) {
		// Get the dock that contains the view
		for (auto& entry : view_docks_)
			if (entry.second == view) {
				entry.first->setObjectName(session->name());
				entry.first->setWindowTitle(session->name());
				entry.first->setWindowTitle("");
				// entry.first->setTitleBarWidget(nullptr);
			}
	}

	// Update the tab widget by finding the main window and the tab from that
	for (auto& entry : session_windows_)
		if (entry.first.get() == session) {
			QMainWindow *window = entry.second;
			const int index = session_selector_.indexOf(window);
			session_selector_.setTabText(index, session->name());
		}

	// Refresh window title if the affected session has focus
	if (session == last_focused_session_.get()) {
		// setWindowTitle(WindowTitle + " - " + session->name());
		setWindowTitle(WindowTitle);
	}
}

void MainWindow::on_session_device_changed()
{
	Session *session = qobject_cast<Session*>(QObject::sender());
	assert(session);

	// Ignore if caller is not the currently focused session
	// unless there is only one session
	if ((sessions_.size() > 1) && (session != last_focused_session_.get()))
		return;

	update_acq_button(session);
}

void MainWindow::on_session_capture_state_changed(int state)
{
	(void)state;

	Session *session = qobject_cast<Session*>(QObject::sender());
	assert(session);

	// Ignore if caller is not the currently focused session
	// unless there is only one session
	if ((sessions_.size() > 1) && (session != last_focused_session_.get()))
		return;

	update_acq_button(session);
}

void MainWindow::on_new_view(Session *session, int view_type)
{
	// We get a pointer and need a reference
	for (shared_ptr<Session>& s : sessions_)
		if (s.get() == session)
			add_view((views::ViewType)view_type, *s);
}

void MainWindow::on_view_close_clicked()
{
	// Find the dock widget that contains the close button that was clicked
	QObject *w = QObject::sender();
	QDockWidget *dock = nullptr;

	while (w) {
	    dock = qobject_cast<QDockWidget*>(w);
	    if (dock)
	        break;
	    w = w->parent();
	}

	// Get the view contained in the dock widget
	shared_ptr<views::ViewBase> view;

	for (auto& entry : view_docks_)
		if (entry.first == dock)
			view = entry.second;

	// Deregister the view
	for (shared_ptr<Session> session : sessions_) {
		if (!session->has_view(view))
			continue;

		// Also destroy the entire session if its main view is closing...
		if (view == session->main_view()) {
			// ...but only if data is saved or the user confirms closing
			if (session->data_saved() || (QMessageBox::question(this, tr("Confirm"),
				tr("This session contains unsaved data. Close anyway?"),
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes))
				remove_session(session);
			break;
		} else
			// All other views can be closed at any time as no data will be lost
			remove_view(view);
	}
}

void MainWindow::on_tab_changed(int index)
{
	shared_ptr<Session> session = get_tab_session(index);

	if (session)
		on_focused_session_changed(session);
}

void MainWindow::on_tab_close_requested(int index)
{
	shared_ptr<Session> session = get_tab_session(index);

	if (!session)
		return;

	if (session->data_saved() || (QMessageBox::question(this, tr("Confirm"),
		tr("This session contains unsaved data. Close anyway?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes))
		remove_session(session);

	if (sessions_.empty())
		update_acq_button(nullptr);
}

void MainWindow::on_show_decoder_selector(Session *session)
{
#ifdef ENABLE_DECODE
	// Close dock widget if it's already showing and return
	for (auto& entry : sub_windows_) {
		QDockWidget* dock = entry.first;
		shared_ptr<subwindows::SubWindowBase> decoder_selector =
			dynamic_pointer_cast<subwindows::decoder_selector::SubWindow>(entry.second);

		if (decoder_selector && (&decoder_selector->session() == session)) {
			sub_windows_.erase(dock);
			dock->close();
			return;
		}
	}

	// We get a pointer and need a reference
	for (shared_ptr<Session>& s : sessions_)
		if (s.get() == session)
			add_subwindow(subwindows::SubWindowTypeDecoderSelector, *s);
#else
	(void)session;
#endif
}

void MainWindow::on_sub_window_close_clicked()
{
	// Find the dock widget that contains the close button that was clicked
	QObject *w = QObject::sender();
	QDockWidget *dock = nullptr;

	while (w) {
	    dock = qobject_cast<QDockWidget*>(w);
	    if (dock)
	        break;
	    w = w->parent();
	}
	sub_windows_.erase(dock);
	dock->close();

	// Restore focus to the last used main view
	if (last_focused_session_)
		last_focused_session_->main_view()->setFocus();
}

void MainWindow::on_view_colored_bg_shortcut()
{
	GlobalSettings settings;

	bool state = settings.value(GlobalSettings::Key_View_ColoredBG).toBool();
	settings.setValue(GlobalSettings::Key_View_ColoredBG, !state);

	show_session_info_with_auto_close(QString(QCoreApplication::translate("pv::dialogs::Settings","Use colored trace &background")) + " : " +  (!state ? "Enabled" : "Disabled"));
}

void MainWindow::on_view_sticky_scrolling_shortcut()
{
	GlobalSettings settings;

	bool state = settings.value(GlobalSettings::Key_View_StickyScrolling).toBool();
	settings.setValue(GlobalSettings::Key_View_StickyScrolling, !state);

	show_session_info_with_auto_close(QString(QCoreApplication::translate("pv::dialogs::Settings","Always keep &newest samples at the right edge during capture")) + " : " +  (!state ? "Enabled" : "Disabled"));
}

void MainWindow::on_view_show_sampling_points_shortcut()
{
	GlobalSettings settings;

	bool state = settings.value(GlobalSettings::Key_View_ShowSamplingPoints).toBool();
	settings.setValue(GlobalSettings::Key_View_ShowSamplingPoints, !state);

	show_session_info_with_auto_close(QString(QCoreApplication::translate("pv::dialogs::Settings","Show data &sampling points")) + " : " +  (!state ? "Enabled" : "Disabled"));
}

void MainWindow::on_view_show_analog_minor_grid_shortcut()
{
	GlobalSettings settings;

	bool state = settings.value(GlobalSettings::Key_View_ShowAnalogMinorGrid).toBool();
	settings.setValue(GlobalSettings::Key_View_ShowAnalogMinorGrid, !state);

	show_session_info_with_auto_close(QString(QCoreApplication::translate("pv::dialogs::Settings","Show analog minor grid in addition to div grid")) + " : " +  (!state ? "Enabled" : "Disabled"));
}

void MainWindow::on_close_current_tab()
{
	int tab = session_selector_.currentIndex();

	on_tab_close_requested(tab);
}

void MainWindow::on_close_decoder_dock(Session *session)
{
	on_show_decoder_selector(session);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        if (QWidget *w = qobject_cast<QWidget*>(obj))
        {
			#ifdef _WIN32
            // Skip child widgets and only handle standalone top-level windows
			GlobalSettings settings;
            if(w->isTopLevel() && settings.current_theme_is_dark())
            {
                HWND hwnd = reinterpret_cast<HWND>(w->winId());
                if(hwnd)
                {
                    BOOL useDark = TRUE;
                    HRESULT hr = DwmSetWindowAttribute(hwnd, 20, &useDark, sizeof(useDark));
					if(SUCCEEDED(hr))
					{
						MARGINS margins = {0,0,0,0};
						DwmExtendFrameIntoClientArea(hwnd, &margins);
					}
                }
            }
			#endif
        }
    }
    return QObject::eventFilter(obj, event);
}

} // namespace pv
