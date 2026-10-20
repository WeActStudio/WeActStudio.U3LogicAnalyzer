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
#include <cstdio>
#include <climits>
#include <cstdint>
#include <iterator>
#include <map>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QDockWidget>
#include <QFile>
#include <QFontMetrics>
#include <QPainter>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSet>
#include <QTabBar>
#include <QVBoxLayout>
#ifdef __linux__
#include <unistd.h>
#endif
#include <QInputDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QElapsedTimer>
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>

#include <thread>
#include "application.hpp"
#include "devicemanager.hpp"
#include "devices/hardwaredevice.hpp"
#include "dialogs/settings.hpp"
#include "globalsettings.hpp"
#include "fluentui3colors.h"
#include "toolbars/mainbar.hpp"
#include "widgets/devicetoolbutton.hpp"
#include "storesession.hpp"
#include "data/decodesignal.hpp"
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
		iconLabel->setPixmap(pv::util::message_box_icon(QMessageBox::Information));
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
	const pv::util::MissingUdevRules missing = pv::util::udev_rules_missing();
	if (missing.paths.isEmpty())
		return true;

	/* Already root: no password needed. */
	if (geteuid() == 0) {
		std::string err;
		if (pv::util::install_udev_rules(std::string(), err))
			return true;
	}

	const QString intro = MainWindow::tr(
		"No udev access rules were found for:\n%1\n\n"
		"Without them the device cannot be opened by an unprivileged user, "
		"so it will not be detected.\n\n"
		"Enter your administrator password to install:\n%2")
		.arg(missing.devices, missing.paths);

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
			"later, or copy the rule files from the repository to "
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

namespace {

/*
 * A dock that answers a double click on its title bar while it floats.
 *
 * The window system has a use of its own for that click on a tool window -
 * it resizes the window - and the geometry it leaves behind is the one the
 * view keeps when it is docked again, which can be outside the window
 * altogether, where nothing can reach it. The click is taken here instead,
 * and the toggle is put off a turn of the event loop so that it is not done
 * from inside the window procedure.
 */
class ViewDock : public QDockWidget
{
public:
	ViewDock(const QString &title, QWidget *parent) :
		QDockWidget(title, parent)
	{
	}

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	bool nativeEvent(const QByteArray &type, void *message, qintptr *result) override
#else
	bool nativeEvent(const QByteArray &type, void *message, long *result) override
#endif
	{
#ifdef Q_OS_WIN
		if ((type == QByteArrayLiteral("windows_generic_MSG")) && message) {
			const MSG *msg = static_cast<const MSG *>(message);

			if (msg->message == WM_NCLBUTTONDBLCLK) {
				QTimer::singleShot(0, this, [this]() {
					setFloating(!isFloating());
				});

				*result = 0;
				return true;
			}
		}
#endif
		return QDockWidget::nativeEvent(type, message, result);
	}
};

/*
 * Outlines the decoder selector panel: FluentUI3 gives it none, and Fusion loses
 * it at the bottom, so only the lines Fusion does not draw are painted here.
 * A style sheet is no use - a widget that carries one is left out of the palette
 * and style changes and keeps the last theme.
 */
class DockFrame : public QWidget
{
public:
	DockFrame(QWidget *parent) : QWidget(parent) {}

	/* The margins are the pixels the lines are drawn in, and which lines are
	 * drawn depends on the style, so they move with it. */
	void update_margins()
	{
		if (QLayout *l = layout()) {
			const int side = paints_sides() ? 1 : 0;
			l->setContentsMargins(side, 0, side, 1);
		}
	}

protected:
	void paintEvent(QPaintEvent *) override
	{
		/* FluentUI3's frame colour, or the tone of the palette under Fusion:
		 * its dark Mid sits just above Base, where the line would vanish. */
		const bool dark = GlobalSettings::current_theme_is_dark();
		const bool fluent = paints_sides();
		const QColor line = fluent ?
			(dark ? WINUI3ColorsDark : WINUI3ColorsLight)[WINUI3Color::frameColorLight] :
			palette().color(dark ? QPalette::Midlight : QPalette::Mid);

		QPainter p(this);
		p.setPen(line);
		if (fluent) {
			p.drawLine(0, 0, 0, height() - 1);
			p.drawLine(width() - 1, 0, width() - 1, height() - 1);
		}
		p.drawLine(0, height() - 1, width() - 1, height() - 1);
	}

	void changeEvent(QEvent *event) override
	{
		if (event->type() == QEvent::StyleChange)
			update_margins();
		QWidget::changeEvent(event);
	}

private:
	/* Fusion draws the sides of the panel itself, FluentUI3 does not. The name
	 * of the style class is the only thing that tells the two apart. */
	static bool paints_sides()
	{
		return qApp->style()->inherits("FluentUI3Style");
	}
};

} // namespace

/*
 * Lets a dock be taken out of the window and put back with a double click on
 * its title bar, as the views of PulseView can. The dock does the docked case
 * for itself; ViewDock looks after the floating one, where the decorations
 * belong to the window system.
 *
 * A view that comes back keeps the geometry of the floating window, which is
 * measured from the window - so one carried well away from the window lands
 * beyond its edge, where nothing can reach it. Only that case is put right:
 * anywhere inside the window is where the user put it. The window is made to
 * lay the whole thing out afresh by handing it back the layout it had when
 * the view was last known to be in a good place.
 */
void MainWindow::make_dock_floatable(QDockWidget *dock)
{
	/*
	 * The dock joins the ones already stacked in the window, as the views
	 * of PulseView do: the window gives them a tab bar and they take turns,
	 * which leaves the room to the waveforms. The first one to arrive has
	 * nobody to join, so it simply stands beside the main view.
	 */
	QMainWindow *window = qobject_cast<QMainWindow *>(dock->parentWidget());

	if (window) {
		QDockWidget *sibling = nullptr;

		/* Views first, then the sub windows: whichever came in first is
		 * the one anything new is stacked on. */
		for (auto &entry : view_docks_)
			if ((entry.first != dock) &&
				(view_dock_types_.count(entry.first) > 0) &&
				!entry.first->isFloating() &&
				(entry.first->parentWidget() == window)) {
				sibling = entry.first;
				break;
			}

		if (!sibling)
			for (auto &entry : sub_windows_)
				if (entry.first != dock &&
					!entry.first->isFloating() &&
					(entry.first->parentWidget() == window)) {
					sibling = entry.first;
					break;
				}

		if (sibling) {
			window->tabifyDockWidget(sibling, dock);

			/*
			 * The one just opened is the one to look at. Raised a turn
			 * later: at this point the dock has no contents yet, and
			 * the window has not laid the tab bar out.
			 */
			QTimer::singleShot(0, dock, [dock]() {
				dock->raise();
			});
		}
	}

	/*
	 * Every view can be taken out of the window and put back again, as the
	 * views of PulseView can.
	 */
	dock->setFeatures(QDockWidget::DockWidgetMovable |
		QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

	{
		/* Enough to read in, and no more: the divider stays free. */
		dock->setMinimumWidth(200);

		/*
		 * A view that comes back from a float keeps the geometry of the
		 * floating window as its own, even though Qt reports it as docked
		 * - it can end up outside the window, where nothing can reach it.
		 * Taking it out of the layout and putting it back is what makes
		 * the window lay it out afresh; the width is then given to it in
		 * a turn of the event loop, once the layout has settled.
		 */
		connect(dock, &QDockWidget::topLevelChanged, dock,
			[this, dock](bool floating) {
				if (floating)
					return;

				QMainWindow *window =
					qobject_cast<QMainWindow *>(dock->parentWidget());

				if (!window)
					return;

				/*
				 * A view that comes back from a float keeps the geometry
				 * of the floating window, even though Qt reports it as
				 * docked - it can sit outside the window, where nothing
				 * can reach it. Adding it to the edge it is already on
				 * changes nothing at all; splitting it off the main view
				 * is what makes the window lay it out afresh.
				 */
				QDockWidget *main_dock = nullptr;

				for (auto &entry : view_docks_)
					if (view_dock_types_.count(entry.first) == 0) {
						main_dock = entry.first;
						break;
					}

				/*
				 * A view that sits inside the window stays where it is:
				 * that is where the user put it. One that only overlaps
				 * the window - its top left corner inside, the rest of it
				 * beyond the edge - does not count, which is what an
				 * over-wide view gives.
				 */
				const QRect window_rect(window->mapToGlobal(QPoint(0, 0)),
					window->size());

				if (window_rect.contains(QRect(
						dock->mapToGlobal(QPoint(0, 0)), dock->size())))
					return;

				/*
				 * Only one that came back somewhere outside the window is
				 * put right. Qt keeps the geometry of the floating window
				 * as the place of the dock - a place measured from the
				 * window, so a window at 531,445 on the screen puts a
				 * view dropped at 1962,604 at 2493,1049, well outside
				 * it. Neither addDockWidget nor splitDockWidget shifts
				 * it once that has happened; handing the whole layout
				 * back to the window does.
				 */
				auto known = window_layouts_.find(window);

				if (known != window_layouts_.end() &&
					!known->second.isEmpty()) {
					window->restoreState(known->second);
					window->addDockWidget(Qt::RightDockWidgetArea, dock);
				} else if (main_dock &&
					(main_dock->parentWidget() == window)) {
					window->removeDockWidget(dock);
					window->splitDockWidget(main_dock, dock,
						Qt::Horizontal);
				} else {
					window->removeDockWidget(dock);
					window->addDockWidget(Qt::RightDockWidgetArea, dock);
				}

				dock->show();

				QTimer::singleShot(0, dock, [dock]() {
					QMainWindow *w =
						qobject_cast<QMainWindow *>(dock->parentWidget());

					if (!w)
						return;

					const int room = qMax(240, qMin(w->width() / 3, 480));

					if (dock->width() < room)
						w->resizeDocks({dock}, {room}, Qt::Horizontal);
				});
			});
	}

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

	/*
	 * Every view is named after its view type, the main one included: what
	 * it shows is the waveforms, not the session it belongs to.
	 */
	QString title = QString("%1").arg(QCoreApplication::translate(
		"pv::views", views::ViewTypeNames[type]));

	QDockWidget* dock = new ViewDock(title, main_window);
	// dock->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);  //todo ???
	dock->setObjectName(title);

	if (type == views::ViewTypeTrace && !main_bar)
		main_window->addDockWidget(Qt::LeftDockWidgetArea, dock);
	else
		main_window->addDockWidget(Qt::RightDockWidgetArea, dock);

	make_dock_floatable(dock);

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

	/* The main view carries the session name, and has no title bar of its
	 * own: it takes the whole window. The waveforms are what the window is
	 * for, so it keeps the larger part of it even when other views are
	 * docked beside it or under it. */
	if (type == views::ViewTypeTrace && !main_bar) {
		dock->setTitleBarWidget(new QWidget());
		dock->setMinimumWidth(240);
	}

	view_docks_[dock] = v;
	/* The main view is not one of these: the others are laid beside it,
	 * never over it, and it is not renamed when the session is. */
	if (main_bar)
		view_dock_types_[dock] = (int)type;
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
			connect(main_bar.get(), SIGNAL(device_attached()), this, SLOT(on_device_attached()));
			connect(main_bar.get(), SIGNAL(devices_went_offline(QStringList)),
				this, SLOT(on_devices_went_offline(QStringList)), Qt::QueuedConnection);
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
	/* Remember the layout now that it holds this view as well. */
	QTimer::singleShot(0, main_window, [this, main_window]() {
		window_layouts_[main_window] = main_window->saveState();
	});

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
				view_dock_types_.erase(entry.first);
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

	QDockWidget* dock = new ViewDock(title, main_window);
	dock->setObjectName(title);

	if (title == "") {
		dock->setTitleBarWidget(new QWidget());
		dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	}

	// main_window->addDockWidget(Qt::TopDockWidgetArea, dock);
	main_window->addDockWidget(Qt::RightDockWidgetArea, dock);
	dock->setAllowedAreas(Qt::RightDockWidgetArea);
	make_dock_floatable(dock);

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
	sub_window_types_[dock] = (int)type;
	dock_main->setCentralWidget(w.get());

	/*
	 * The dock sizes its widget from this one now, and a plain widget asks for
	 * its hint rather than for the room it is given.
	 */
	DockFrame *dock_frame = new DockFrame(dock);
	dock_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QVBoxLayout *dock_frame_layout = new QVBoxLayout(dock_frame);
	dock_frame_layout->setSpacing(0);
	dock_frame_layout->addWidget(dock_main);
	dock_frame->update_margins();
	dock->setWidget(dock_frame);

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

	if (w->minimum_width() > 0)
		/* Room enough for what it holds, and no ceiling: it is dragged
		 * to the width that suits like any other view. */
		dock->setMinimumSize(w->minimum_width(), 0);
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

	window->setDockNestingEnabled(true);

	add_view(views::ViewTypeTrace, *session);

	/*
	 * The bar of this session can ask for a file, for a board to be opened
	 * in a session of its own, or for the captured trace to be copied into
	 * one. All three are the window's to carry out.
	 */
	if (session->main_bar()) {
		connect(session->main_bar().get(), &toolbars::MainBar::open_file_requested,
			this, &MainWindow::on_open_file_requested);
		connect(session->main_bar().get(), &toolbars::MainBar::device_new_session_requested,
			this, &MainWindow::on_device_new_session_requested);
		connect(session->main_bar().get(), &toolbars::MainBar::copy_session_requested,
			this, &MainWindow::on_copy_session_requested);
	}

	update_session_roles();
	update_session_tabs();
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

/*
 * Whether another session than the one asking is already on this board:
 * the identity is the port the board sits in, so it is the same board even
 * though a scan handed each session its own instance of it.
 */
static bool board_taken(const list< shared_ptr<Session> > &sessions,
	const shared_ptr<Session> &self, const shared_ptr<devices::Device> &dev)
{
	const QString identity = widgets::DeviceToolButton::device_identity(dev);
	if (identity.isEmpty())
		return false;

	for (const shared_ptr<Session>& s : sessions)
		if (s && s != self &&
			widgets::DeviceToolButton::device_identity(s->device()) == identity)
			return true;

	return false;
}

void MainWindow::do_device_detach_switch(const shared_ptr<Session> &session)
{
	/*
	 * The session whose board went away, which is not always the one in
	 * front: a board can be opened in a session of its own and be the one
	 * that is pulled while the user is looking at another.
	 */
	if (!session || !session->main_bar()) {
		return;
	}

	/* What the page is about to lose, remembered in case it has to fall
	 * back to the demo device: the board can come back to it later. */
	const QString lost_identity =
		widgets::DeviceToolButton::device_identity(session->device());

	/*
	 * A session that was opened for a board of its own has nothing left to
	 * do once that board is pulled, so it is closed - that is what pulling
	 * it asks for, whether the capture on it was saved or not. Keeping it
	 * would only leave a page holding another board's settings.
	 *
	 * The capture page is the one that cannot be closed, so it is the one
	 * that moves to whatever is still there.
	 */
	if (!sessions_.empty() && session != sessions_.front()) {
		remove_session(session);

		/*
		 * The board is gone from the list the rescan made, but every page
		 * that is left was built before that and would still offer it -
		 * the capture page above all, since it is the one the user is
		 * looking at.
		 */
		for (const shared_ptr<Session>& s : sessions_)
			if (s && s->main_bar())
				s->main_bar()->refresh_device_selector();

		return;
	}

	session->device_detached();

	/* Reuse the global context: a second Context would sr_exit() the
	 * shared backend when destroyed, freeing every device instance. */

	shared_ptr<sigrok::Context> context = sigrok_context_;
	string driver = ch32h417_driver_name(context);
	/*
	 * Let go of the old instances before scanning, not after: a scan opens
	 * every board it finds, and deleting the old manager afterwards closes
	 * what that manager holds - by an index that a board coming or going
	 * has already moved. Every page has to let go of its board, not only
	 * the one that lost it, since a page's own reference keeps the board
	 * open and the scan would leave it out as if it had been unplugged.
	 */
	for (const shared_ptr<Session> &s : sessions_) {
		if (!s || !s->device() || s == session)
			continue;
		try {
			s->device()->close();
		} catch (const sigrok::Error &e) {
			qWarning() << "Error closing a device before the scan:" << e.what();
		} catch (const QString &msg) {
			qWarning() << "Error closing a device before the scan:" << msg;
		}
	}

	/*
	 * The fresh manager takes over before the old one goes: releasing it
	 * lets signals through, and a page being built at that moment asks the
	 * window for a manager - one that had been set aside would hand it a
	 * null one, and the page would be built around nothing.
	 */
	pv::DeviceManager *const old_device_manager = device_manager_.load();
	auto new_device_manager = new pv::DeviceManager(context, driver, true);
	device_manager_.store(new_device_manager);
	update_sessions_device_manager(new_device_manager);
	if (old_device_manager)
		delete old_device_manager;

	/*
	 * The old manager took every device of it with it when it went, the
	 * boards of the other sessions included - they would go on holding
	 * instances that are shut, and every call they make into the driver
	 * would fail. Each of them takes the fresh instance of its own board.
	 */
	adopt_device_in_sessions();
	update_session_roles();

	list< shared_ptr<devices::HardwareDevice> > devices = new_device_manager->devices();

	if (!devices.empty()) {
		shared_ptr<devices::Device> default_device;
		WchDeviceType found_device_type = WchDeviceType::None;

		/*
		 * Any board that is free will do, whatever make it is: there is no
		 * order between them, and a board of another make takes a capture
		 * just as well as a WCH one. What a board must not be is one that
		 * another session is already on - two pages on one board would both
		 * try to run it, which is what put the board of the second session
		 * on the capture page as well.
		 */
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			if (board_taken(sessions_, session, dev))
				continue;
			if (dev->hardware_device()->driver()->name() == "demo")
				continue;

			found_device_type = get_wch_device_type_by_driver(
				dev->hardware_device()->driver()->name());
			default_device = dev;
			break;
		}

		/* Every board is in use elsewhere or gone: the demo device. */
		if (!default_device)
			for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
				shared_ptr<devices::HardwareDevice> hw_dev =
					std::dynamic_pointer_cast<devices::HardwareDevice>(dev);
				if (hw_dev &&
					hw_dev->hardware_device()->driver()->name() == "demo") {
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

		/*
		 * A board of another make is named by its own model: the bar keys
		 * the settings logic off the vendor, and it must not go on reading
		 * the WCH name of the board that went away.
		 */
		if ((found_device_type == WchDeviceType::None) && !is_demo_device &&
			default_device && default_device->device())
			session->main_bar()->set_vendorName(QString::fromStdString(
				default_device->device()->model()));

		/*
		 * The same call the device list uses, so the replacement gets the
		 * pattern mode, the rate selector and the rollback of a board that
		 * will not open as well. Ask is false: the question this flow came
		 * from has already been answered.
		 */
		if (session->main_bar()->switch_to_device(default_device, false)) {
			/* The strip names each page after the board on it, and it is
			 * this call that does the naming. */
			update_session_roles();

			if (found_device_type != WchDeviceType::None) {
				/*
				 * The saved setup belongs to this page - it is the one that
				 * was just given another board - and is applied to it, not
				 * to whichever page happens to be in front: restoring it
				 * there would put the settings of this board on a page that
				 * is on another one.
				 */
				QSettings settings;
				settings.beginGroup("Session0");
				session->restore_settings(settings);
				settings.endGroup();
				if (session->main_bar())
					session->main_bar()->renew_setting_default();
			} else if (is_demo_device) {
				/* The vendor name is what the rest of the bar keys off, and
				 * the demo driver reports none. */
				session->main_bar()->set_vendorName("Demo device");
				session->create_demo_uart_decoder();

				/* Nothing could take the place of the board: the page
				 * stands in with the demo device and takes the board back
				 * when it is plugged in again. */
				waiting_for_board_ = lost_identity;
			}
		}
	} else {
		session->main_bar()->set_vendorName("No Device");
		/* Nothing took the place of the board that went away, so the page
		 * that described it is cleared as well. */
		session->main_bar()->refresh_device_widgets();
	}
}

/*
 * Carries out what the answer to the detach question asks for: the device that
 * was in use is gone, so the list is read again and the session moves to what
 * is there. Device events are let through again, but the one that was held
 * back is not replayed - the rescan just done covers it, and a second pass
 * would only ask about the session again while the board is still coming up.
 */
void MainWindow::apply_device_detach(const shared_ptr<Session> &session)
{
	detach_dialog_ = nullptr;
	do_device_detach_switch(session);
	resume_device_events(false);
	report_stalled_devices();
}

void MainWindow::show_device_detach(const shared_ptr<Session> &session)
{
	if (!session || !session->main_bar()) {
		/*
		 * Nobody to ask. Device events were held back for this question,
		 * so they have to be let through again - otherwise every arrival
		 * from here on stays pending and the program stops noticing boards
		 * coming and going.
		 */
		resume_device_events(false);
		return;
	}

	/* Already asking. Raising the question is all that is left to do; a
	 * second copy would have to be answered for the same thing. */
	if (detach_dialog_) {
		detach_dialog_->raise();
		detach_dialog_->activateWindow();
		return;
	}

	attach_process_done = false;
	QDialog* dialog = new QDialog(this);
	dialog->setWindowTitle(tr("Warning"));

	/*
	 * No close button: the answer finishes the detach, and a dialog that
	 * can be dismissed without answering used to leave the device events
	 * held back for a question that never got one. The hint is dropped as
	 * well as the flags customised - a title bar keeps its close box on
	 * some window managers otherwise.
	 */
	dialog->setWindowFlags((dialog->windowFlags()
			& ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint)
		| Qt::CustomizeWindowHint | Qt::WindowTitleHint);

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
	iconLabel->setPixmap(pv::util::message_box_icon(QMessageBox::Warning));
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
	/*
	 * Three answers. The page is the only place the capture is, and the
	 * board it came from is the one that left, so the data only survives
	 * if it is handed to a page of its own before the page moves on:
	 * the first button does just that, the second saves the session, and
	 * the third lets the page move on without it.
	 */
	/* Nothing to hand over when the page holds no capture, so the option is
	 * not offered: it would be an empty page in a new tab. */
	const bool has_data = session && session->data_held();

	QPushButton *exportBtn = new QPushButton(
		tr("Export Only the Capture to a New Session"), dialog);
	exportBtn->setVisible(has_data);
	QPushButton *saveBtn = new QPushButton(tr("Save"), dialog);
	QPushButton *discardBtn = new QPushButton(tr("Cancel"), dialog);
	exportBtn->setObjectName("detach_export");
	saveBtn->setObjectName("detach_save");
	discardBtn->setObjectName("detach_discard");
	for (QPushButton *button : {exportBtn, saveBtn, discardBtn})
		button->setMinimumWidth(90);
	buttonLayout->addWidget(exportBtn);
	buttonLayout->addWidget(saveBtn);
	buttonLayout->addWidget(discardBtn);
	layout->addLayout(buttonLayout);

	// Size to contents and prevent the window from being resized
	dialog->setFixedSize(dialog->sizeHint());

	/* Modal, but shown rather than exec()'d: the question belongs to this
	 * change of device, and while it is up the user must not be able to
	 * pick another board - the answer replaces the device afterwards, and
	 * a pick made in between would simply be overwritten. */
	dialog->setModal(true);

	/*
	 * Every page is written down, whatever board it holds: the pages are
	 * restored from this, and a page on another make of board is a page
	 * like any other. Only the settings of a board are WCH boards' own.
	 */
	save_sessions();
	if (session->main_bar()->channels_ &&
		is_wch_device(session->main_bar()->channels_->device_name_))
		session->main_bar()->set_setting_default();

	connect(exportBtn, &QPushButton::clicked, [=, session](){
		/* The capture goes to a page of its own first, while it is still
		 * here to be handed over. */
		copy_session_to_new_session(session);
		apply_device_detach(session);
		dialog->deleteLater();
	});

	connect(saveBtn, &QPushButton::clicked, [=, session](){
		/*
		 * Written here rather than through the save action: that one hands
		 * the work to a progress dialog and returns, and the capture would
		 * be moved to another device while it was still being written. A
		 * name the user backs out of leaves everything as it was.
		 */
		QString path = QFileDialog::getSaveFileName(this,
			tr("Save File"), toolbars::MainBar::last_save_directory(),
			tr("Session files (*.sr);;All Files (*)"));
		if (path.isEmpty())
			return;

		toolbars::MainBar::remember_save_directory(path);

		if (!store_capture_to(session, path))
			return;

		apply_device_detach(session);
		dialog->deleteLater();
	});

	connect(discardBtn, &QPushButton::clicked, [=, session](){
		apply_device_detach(session);
		dialog->deleteLater();
	});

	/*
	 * A dismissal that never reached a button - the cancel key, or a
	 * manager that puts the close box back regardless. It keeps nothing,
	 * like the third button; what it must not do is leave the device
	 * events held back, or the list would never be read again.
	 */
	connect(dialog, &QDialog::rejected, [=, session](){
		apply_device_detach(session);
		dialog->deleteLater();
	});

	dialog->show();
	detach_dialog_ = dialog;
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
	/*
	 * One board being plugged reaches us several times: the vendor driver
	 * reports the arrival while it walks its indices, and the rescan it
	 * starts ends by replaying what came in meanwhile. Let them pile up
	 * for a moment and answer them all with a single pass - a scan per
	 * notice is a scan too many, and every one of them walks the USB bus
	 * again and flashes the scan dialog.
	 */
	if (!attach_process_done || device_event_queued_) {
		device_event_pending_ = true;
		return;
	}

	device_event_queued_ = true;
	device_event_pending_ = false;

	QTimer::singleShot(DeviceEventDebounceMs, this, [this]() {
		device_event_queued_ = false;
		run_device_attached();
	});
}

void MainWindow::on_devices_went_offline(const QStringList &identities)
{
	/* Only the pages that lost a board are marked: a mask is taken away by
	 * the pass that settles the list, never here. */
	for (const shared_ptr<Session> &session : sessions_)
	{
		if (!session || !session->main_bar())
			continue;

		const QString identity =
			widgets::DeviceToolButton::device_identity(session->device());
		if (identity.isEmpty() || !identities.contains(identity))
			continue;

		/*
		 * The page is the tab this session sits in, not the whole program.
		 * A session whose window is not up yet is left alone: passing none
		 * would put the mask on the window this bar happens to belong to,
		 * which is the whole program.
		 */
		const auto window = session_windows_.find(session);
		if (window == session_windows_.end())
			continue;

		session->main_bar()->set_device_offline(true, window->second);
	}
}

void MainWindow::run_device_attached()
{
	/*
	 * A board arrived or went away. A pass has to finish before the next one
	 * starts, but a change that comes in meanwhile must not be dropped: a
	 * board that was plugged and unplugged quickly would otherwise be missed
	 * for good. Remember it and handle it in one more pass instead.
	 */

	if (!attach_process_done) {
		device_event_pending_ = true;
		return;
	}
	attach_process_done = false;

	const shared_ptr<devices::Device> previous = last_focused_session_ ?
		last_focused_session_->device() : nullptr;
	const shared_ptr<devices::HardwareDevice> previous_hw =
		dynamic_pointer_cast<devices::HardwareDevice>(previous);

	/*
	 * What is in use has to be identified before the scan, while the driver
	 * still knows the device: the scan frees the instances, so afterwards it
	 * can only be looked up again by the identity it reports.
	 */
	std::map<string, string> previous_info;
	const QString previous_identity =
		widgets::DeviceToolButton::device_identity(previous);
	if (previous_hw) {
		if (device_manager_)
			previous_info = device_manager_.load()->get_device_info(previous);
	}

	if (previous) {
		last_focused_session_->stop_capture();
		/*
		 * The previous device may already be gone, and libsigrok reports that
		 * by throwing. Letting it escape from a Qt slot would terminate the
		 * application, so swallow it: we are about to rescan anyway.
		 */
		try {
			previous->close();
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

	/*
	 * Let go of the old instances before scanning, not after: a scan opens
	 * every board it finds, and deleting the old manager afterwards closes
	 * what that manager holds - by an index that a board coming or going
	 * has already moved. Every page has to let go of its board, not only
	 * the one in front, since a page's own reference keeps the board open
	 * and the scan would leave it out as if it had been unplugged.
	 */
	for (const shared_ptr<Session> &s : sessions_) {
		if (!s || !s->device() || s->device() == previous)
			continue;
		try {
			s->device()->close();
		} catch (const sigrok::Error &e) {
			qWarning() << "Error closing a device before the scan:" << e.what();
		} catch (const QString &msg) {
			qWarning() << "Error closing a device before the scan:" << msg;
		}
	}

	if (device_manager_) {
		delete device_manager_.load();
		device_manager_.store(nullptr);
	}
	auto device_manager = new pv::DeviceManager(context, driver, true);
	device_manager_.store(device_manager);
	update_sessions_device_manager(device_manager);

	/*
	 * Look for what is in use again. It comes back as a new instance, so it
	 * is the identity that has to match, not the pointer. Finding it means
	 * nothing was unplugged: the selection stays where the user put it, an
	 * arrival never moves it.
	 *
	 * The identity is asked first: it is the port the board sits in, and it
	 * is what tells two boards of one model apart. Matching on the vendor,
	 * model and serial number instead would let firmware that writes the
	 * same serial into every board hand back the wrong one - which is a
	 * device switch.
	 */
	shared_ptr<devices::HardwareDevice> same_device;
	if (!previous_identity.isEmpty())
		same_device = find_device_by_identity(previous_identity);
	else if (!previous_info.empty())
		same_device = device_manager->find_device_from_info(previous_info);

	/*
	 * A board that comes back is the one that is in use, so it has to be
	 * taken over: the scan freed its instance. Any other arrival - a second
	 * board, or the first one plugged in while the demo device stands in -
	 * only joins the list. Picking a device is the user's to do; a board
	 * appearing under the desk must not move the selection out of the
	 * capture he is looking at.
	 *
	 * This runs before the question about a board being pulled, and has to:
	 * a page still holding the instance the scan freed would list its board
	 * a second time once the list is read again.
	 */
	/*
	 * Every session with a board takes its new instance - including the
	 * ones that are not in front: a session left holding the instance a
	 * scan freed has an object no list has, and listing it beside the
	 * board is what showed the same board twice.
	 *
	 * Taking a board back has to keep what the session holds: the arrival
	 * of another board is no reason to throw a capture away. Nothing is
	 * restored from the saved settings here on purpose: applying them
	 * again would put back values the user has changed since - the channel
	 * count he just lowered to eight, say, or the pre-trigger - which is
	 * what made a board coming back look like a reset of everything.
	 */
	std::vector<shared_ptr<Session>> lost;
	const shared_ptr<Session> unplugged = adopt_device_in_sessions(&lost);

	/*
	 * A page that has its board back is usable again, so its mask goes. Every
	 * page that lost one keeps its own: only the first is asked about below,
	 * and until that is answered its mask is the truth - the other is no less
	 * broken for not being asked. Clearing the masks before the scan would
	 * leave those pages looking alive while their devices are the ones the
	 * scan freed.
	 */
	for (const shared_ptr<Session> &masked : sessions_)
		if (masked && masked->main_bar() &&
			std::find(lost.begin(), lost.end(), masked) == lost.end())
			masked->main_bar()->set_device_offline(false);
	if (last_focused_session_ && last_focused_session_->main_bar() &&
		std::find(lost.begin(), lost.end(), last_focused_session_) == lost.end())
		last_focused_session_->main_bar()->set_device_offline(false);

	/* A board came or went beside it, which changes what the selector of
	 * each session may offer. */
	update_session_roles();

	/*
	 * The board this page is waiting for is back: it returns to it, unless
	 * another page took it in the meantime. A board that arrives is not
	 * supposed to move a page, but this one was only ever on the demo
	 * device because its own board was missing.
	 */
	if (!waiting_for_board_.isEmpty()) {
		const shared_ptr<Session> page =
			sessions_.empty() ? nullptr : sessions_.front();
		shared_ptr<devices::HardwareDevice> back =
			find_device_by_identity(waiting_for_board_);

		if (back && page && page->main_bar() &&
			!board_taken(sessions_, page, back)) {
			waiting_for_board_.clear();
			page->main_bar()->switch_to_device(back, false);
			update_session_roles();
		}
	}

	/*
	 * A page other than the capture page lost its board in this scan. A
	 * scan that ran while the bus was being re-enumerated can miss a board
	 * that is still there, and closing a page over that is worse than one
	 * more look: the board is asked for again before it is taken for gone.
	 */
	if (unplugged && !attach_retry_done_ && unplugged != sessions_.front()) {
		attach_retry_done_ = true;
		resume_device_events();
		QTimer::singleShot(700, this, [this]() { on_device_attached(); });
		return;
	}
	attach_retry_done_ = false;

	if (unplugged) {
		/*
		 * The board in that session is gone: ask about saving it the way
		 * the detach flow does. That is also what picks the replacement,
		 * and it lets the device events through again once the question
		 * is answered.
		 */
		show_device_detach(unplugged);
		return;
	}

	/* The list is already up to date: rebuilding the manager above put it
	 * into the selector, and taking the board back rebuilds it again. */
	resume_device_events();
	report_stalled_devices();
}

void MainWindow::report_stalled_devices()
{
	const vector<string> names = DeviceManager::take_stalled_drivers();
	if (names.empty())
		return;

	/*
	 * Name the board where the driver is one of ours, so the message talks
	 * about the analyzer the user is holding rather than about a driver.
	 */
	QStringList boards;
	for (const string &name : names) {
		const WchDeviceType type = get_wch_device_type_by_driver(name);
		boards << ((type != WchDeviceType::None) ?
			DeviceModelNames[static_cast<int>(type)] :
			QString::fromStdString(name));
	}

	pv::util::show_message(this, tr("Device scan timed out"),
		tr("Scanning these drivers took too long, so the software started "
			"without waiting for them:") + "\n\n" + boards.join("\n") +
			"\n\n" + tr("Their devices are not in the list. Unplug and replug "
			"the analyzer - or close whatever else is using it - and scan "
			"again."),
		pv::util::message_box_icon(QMessageBox::Warning),
		460, false, false, false);
}

void MainWindow::resume_device_events(bool replay_pending)
{
	attach_process_done = true;

	if (!device_event_pending_)
		return;

	device_event_pending_ = false;

	/* The pass that just ran read the whole device list, so a change it
	 * swallowed is already accounted for and must not be replayed. */
	if (!replay_pending)
		return;

	/* A change arrived while the previous pass was running: handle it now. */
	QTimer::singleShot(0, this, &MainWindow::on_device_attached);
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

shared_ptr<devices::HardwareDevice> MainWindow::find_device_by_identity(
	const QString &identity)
{
	if (identity.isEmpty() || !device_manager_)
		return nullptr;

	for (const shared_ptr<devices::HardwareDevice>& dev :
			device_manager_.load()->devices())
		if (widgets::DeviceToolButton::device_identity(dev) == identity)
			return dev;

	return nullptr;
}

shared_ptr<devices::HardwareDevice> MainWindow::find_moved_board(
	const shared_ptr<Session> &session)
{
	if (!session || !session->device() || !device_manager_)
		return nullptr;

	const shared_ptr<sigrok::Device> was = session->device()->device();
	if (!was)
		return nullptr;

	/*
	 * Only an identity the host bus assigns is worth chasing: both WCH
	 * drivers name a board by the port path, which a replug does not move,
	 * and matching those by serial number would let one page take the board
	 * of another - the serial of a board that has gone is still readable
	 * through the handle it left behind.
	 */
	const QString was_id = QString::fromStdString(was->connection_id());
	if (!was_id.startsWith("usb/") && !was_id.startsWith("ch375/"))
		return nullptr;

	const string serial = was->serial_number();
	const string model = was->model();
	if (serial.empty())
		return nullptr;

	shared_ptr<devices::HardwareDevice> found;
	for (const shared_ptr<devices::HardwareDevice>& dev :
			device_manager_.load()->devices()) {
		const shared_ptr<sigrok::Device> sr_dev = dev->device();
		if (!sr_dev || sr_dev->serial_number() != serial ||
			sr_dev->model() != model)
			continue;

		/* A board another page is on is not this page's to take. */
		const QString id = widgets::DeviceToolButton::device_identity(dev);
		bool held = false;
		for (const shared_ptr<Session> &other : sessions_)
			if (other && other != session && other->device() &&
				widgets::DeviceToolButton::device_identity(other->device()) == id) {
				held = true;
				break;
			}
		if (held)
			continue;

		/* Two of them answer: which one is not for a guess to decide. */
		if (found)
			return nullptr;

		found = dev;
	}

	return found;
}

shared_ptr<Session> MainWindow::adopt_device_in_sessions(
	std::vector<shared_ptr<Session>> *lost)
{
	/*
	 * Every session with a board on it is looked at, not only the one in
	 * front: the scan handed out new instances for all of them, and a
	 * session left holding the old one keeps an object no list has - which
	 * is what put the same board twice into the selector.
	 */
	shared_ptr<Session> unplugged;

	/* The focused session may already have been taken out of the list. */
	list< shared_ptr<Session> > sessions = sessions_;
	if (last_focused_session_ && std::find(sessions.begin(), sessions.end(),
			last_focused_session_) == sessions.end())
		sessions.push_back(last_focused_session_);

	for (const shared_ptr<Session>& session : sessions) {
		if (!session)
			continue;

		/*
		 * A board on a bus has an identity; a page that shows a file has
		 * none to look for, and must not be taken for one whose board went
		 * away - it never had a board to lose.
		 */
		if (session->using_file_device())
			continue;

		const QString identity =
			widgets::DeviceToolButton::device_identity(session->device());
		if (identity.isEmpty())
			continue;

		shared_ptr<devices::HardwareDevice> again =
			find_device_by_identity(identity);

		/*
		 * The board may have come back in another port: what the scan
		 * reports is where it sits now, not where it used to sit.
		 */
		if (!again)
			again = find_moved_board(session);

		if (!again) {
			if (!unplugged)
				unplugged = session;
			if (lost)
				lost->push_back(session);
			continue;
		}

		if (!session->adopt_device_instance(again)) {
			/* A real switch: the page and the view are rebuilt by it. */
			session->select_device(again);
			continue;
		}

		/*
		 * The board it is on came back as a new instance. The list is the
		 * only thing that moved - a board was plugged or pulled beside it -
		 * so the selector is refreshed and everything the session holds is
		 * left standing.
		 */
		if (session->main_bar())
			session->main_bar()->refresh_device_selector();
	}

	/* Whose board the list no longer has, if any. */
	return unplugged;
}

void MainWindow::remove_session(shared_ptr<Session> session)
{
	/*
	 * The page cannot be closed while its tab is being renamed: the editor
	 * would be left over the strip with nothing under it, and would keep
	 * the page alive through the handle it holds.
	 */
	if (tab_rename_session_ == session)
		finish_tab_rename(false);

	/*
	 * Asked while the page is whole: the poll belongs to one page, and this
	 * is the one if its bar is the host. Its bar lives as long as the
	 * session, so at the end of this function there is no way to tell.
	 */
	const bool hosted = session && session->main_bar().get() ==
		toolbars::MainBar::hot_plug_owner();
	const bool was_file = session && session->using_file_device();

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

	/*
	 * A page holding a file can be the last one, and then there is nothing
	 * left to capture with: the board page the program opens with takes its
	 * place, and the poll with it. A page that held a board is another matter
	 * - that is the last board the user had, and closing it is what he meant.
	 *
	 * Put off for a turn: the page that just went is still alive in this
	 * function, and the board page may rebuild the device manager and delete
	 * the old one under it.
	 */
	if (sessions_.empty() && was_file) {
		QTimer::singleShot(0, this, [this]() {
			if (!sessions_.empty())
				return;
			add_default_session();
			start_hot_plug();
		});
	}

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

	/* The board this session held is free again, so every selector must
	 * offer it once more, and the strip loses one tab. */
	update_session_roles();
	update_session_tabs();

	/* The page is gone, and so is what it had set up: an id is handed out
	 * again only after a restart, and the page that gets it then would
	 * otherwise come up with the settings of a page that was closed. */
	QSettings().remove("Settings/" + QString::number(session->id()));

	/* A copy of it that is still being written keeps the session itself
	 * alive; the page no longer carries the button that said so. */
	copy_percent_.remove(session.get());

	/*
	 * The poll goes with the page that hosted it, and that page is still alive
	 * here - this function holds the session, and the bar with it. So it is
	 * stopped where it is before another page takes it over: two polls must
	 * never run, and the one on the dying page would go on feeding a window
	 * that is being torn down.
	 */
	if (hosted && !sessions_.empty()) {
		if (toolbars::MainBar *const owner = toolbars::MainBar::hot_plug_owner())
			owner->hot_plug_stop();

		const shared_ptr<Session> next = sessions_.front();
		if (next && next->main_bar())
			next->main_bar()->hot_plug_start();
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
	start_hot_plug();
}

void MainWindow::start_hot_plug()
{
	if (!last_focused_session_ || !last_focused_session_->main_bar())
		return;

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
		/* Same order as above: the pages must never catch the window
		 * without a manager to give them. */
		pv::DeviceManager *const old_device_manager = device_manager_.load();
		pv::DeviceManager *const new_device_manager =
			new pv::DeviceManager(context, driver, true);
		device_manager_.store(new_device_manager);
		update_sessions_device_manager(new_device_manager);
		if (old_device_manager)
			delete old_device_manager;
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
	shared_ptr<devices::HardwareDevice> saved_device = nullptr;
	devices = device_manager_.load()->devices();
	/*
	 * The board this page was on last time comes first. Its saved setup
	 * belongs to it, and restore_settings() asks for that same board a
	 * moment later: picking another one here meant opening it, closing it
	 * again and building the whole page twice - and the board that stayed
	 * began at the default channel count, so the rates the page offered
	 * were the ones of the narrower table.
	 */
	QSettings saved;
	saved.beginGroup("Session0");
	const bool saved_is_hardware =
		saved.value("device_type").toString() == "hardware";
	const string saved_port =
		saved.value("device/connection_id").toString().toStdString();
	const string saved_serial =
		saved.value("device/serial_num").toString().toStdString();
	const string saved_model =
		saved.value("device/model").toString().toStdString();
	saved.endGroup();

	if (!devices.empty()) {
		/*
		 * The board this page was on last time comes first: a WeAct board
		 * by the serial number that is its own, so that it is still the
		 * board after it moved to another port.
		 */
		map<string, string> saved_info;
		if (!saved_model.empty())
			saved_info.insert(make_pair("model", saved_model));
		if (!saved_serial.empty())
			saved_info.insert(make_pair("serial_num", saved_serial));
		if (!saved_port.empty())
			saved_info.insert(make_pair("connection_id", saved_port));

		if (saved_is_hardware && !saved_info.empty())
			saved_device = device_manager_.load()->find_saved_device(saved_info);

		/* Nothing remembered of it: the port is all there is to go by. */
		if (!saved_device && saved_is_hardware && !saved_port.empty())
			for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
				const shared_ptr<sigrok::Device> sr_dev = dev->device();
				/* A real board is worth more than the demo device. */
				if (string(dev->hardware_device()->driver()->name()) == "demo")
					continue;
				if (sr_dev && sr_dev->connection_id() == saved_port) {
					saved_device = dev;
					break;
				}
			}

		/* The demo device keeps the session usable when nothing else is there. */
		for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
			if (string(dev->hardware_device()->driver()->name()) == "demo") {
				demo_device = dev;
				break;
			}
		}

		if (saved_device) {
			default_device = saved_device;
			found_device_type = get_wch_device_type_by_driver(
				saved_device->hardware_device()->driver()->name());
			if (found_device_type != WchDeviceType::None)
				wch_device = saved_device;
		} else {
			/* Nothing remembered: the first board of ours will do. */
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
			/*
			 * Anything that is not the demo device is real hardware, so it
			 * is the better choice: fx2lafw and drivers added later have
			 * to reach a session the same way the WCH ones do.
			 */
			bool found_real_device = false;
			if (!wch_device) {
				for (const shared_ptr<devices::HardwareDevice>& dev : devices) {
					if (string(dev->hardware_device()->driver()->name()) != "demo") {
						default_device = dev;
						found_real_device = true;
						break;
					}
				}
			}
			if (!wch_device && !found_real_device && demo_device)
				default_device = demo_device;
		}
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
	} else if (session->device()) {
		/*
		 * Any other hardware device: name it after the model it reports,
		 * so the tool bar shows what is actually being sampled.
		 */
		const shared_ptr<sigrok::Device> sr_dev = session->device()->device();
		const string model = sr_dev ? sr_dev->model() : string();
		if (session->main_bar() && !model.empty()) {
			session->main_bar()->set_vendorName(QString::fromStdString(model));
			session->main_bar()->set_device_selector_name(
				QString::fromStdString(
					session->device()->display_name(*device_manager_.load())));
		}
	}

	/* The page this call set up is the capture page: it is never closed and
	 * never shows a file, and it is the only one a new session can be
	 * started from. */
	default_session_ = session;
	update_session_roles();
	update_session_tabs();

	report_stalled_devices();
}
void MainWindow::save_sessions()
{
	QSettings settings;
	int id = 0;

	/*
	 * The pages are written in the order the strip shows them, not in the
	 * order they were opened: a page is restored from the group its number
	 * gives it, so the numbers are what brings the arrangement back. The
	 * first tab is always the capture page, the rest follow as they were
	 * dragged into place.
	 */
	for (int i = 0; i < session_selector_.count(); i++) {
		const shared_ptr<Session> session = get_tab_session(i);

		// Ignore sessions using the demo device or no device at all
		if (!session || !session->device())
			continue;

		shared_ptr<devices::HardwareDevice> device =
			dynamic_pointer_cast< devices::HardwareDevice >
			(session->device());

		/*
		 * A file is not restored at start-up - it always gets a
		 * session of its own - and the demo device stands in for a
		 * board that is not there, so neither is worth saving.
		 */
		if (!device)
			continue;

		if (string(device->hardware_device()->driver()->name()) == "demo")
			continue;

		settings.beginGroup("Session" + QString::number(id++));
		settings.remove("");  // Remove all keys in this group
		session->save_settings(settings);
		settings.endGroup();
	}

	/*
	 * A page that is gone leaves its group behind. The count below is what
	 * a start reads, so a group beyond it is never looked at - but a later
	 * run that has that page again would write over a group the count had
	 * already left out, and the two would disagree about what was saved.
	 * The groups of pages that are no longer there go with them.
	 */
	const QStringList groups = settings.childGroups();
	for (const QString &group : groups) {
		if (!group.startsWith("Session"))
			continue;

		bool is_number = false;
		const int index = group.mid(7).toInt(&is_number);
		if (is_number && index >= id)
			settings.remove(group);
	}

	settings.setValue("sessions", id);
}

void MainWindow::restore_sessions()
{
	QSettings settings;
	const int session_count = settings.value("sessions", 0).toInt();

	/*
	 * One page per saved session, in the order they were saved. The first
	 * one is the capture page that is already there; the rest are opened
	 * beside it, and each takes the board it was saved with - a page whose
	 * board is not plugged in has nothing to show and is dropped.
	 */
	const shared_ptr<Session> capture_page = last_focused_session_;
	QStringList lost;

	for (int i = 0; i < session_count; i++) {
		settings.beginGroup("Session" + QString::number(i));

		if (i == 0) {
			if (capture_page)
				capture_page->restore_settings(settings);
		} else {
			shared_ptr<Session> page = add_session();
			if (page) {
				page->restore_settings(settings);
				DeviceManager *const manager = device_manager_.load();

				/*
				 * A page whose board is not plugged in has nothing to
				 * show; neither has one whose board another page already
				 * holds - two pages cannot capture from one board.
				 */
				if (!page->device() ||
					board_taken(sessions_, page, page->device())) {
					/*
					 * Say which board it was, so the page that went missing
					 * can be matched to the board that is not plugged in -
					 * or to the one another program is holding.
					 */
					settings.beginGroup("device");
					lost << QString("%1 %2")
						.arg(settings.value("model").toString())
						.arg(settings.value("connection_id").toString());
					settings.endGroup();
					remove_session(page);
				} else if (manager) {
					const QString name = QString::fromStdString(
						page->device()->display_name(*manager));
					if (!name.isEmpty())
						page->set_name(name);
				}
			}
		}

		settings.endGroup();
	}

	if (capture_page && capture_page->main_bar())
		capture_page->main_bar()->renew_setting_default();

	/* Opening the pages left the last one in front; the capture page is the
	 * one that was being looked at when the program was closed. */
	if (capture_page)
		session_selector_.setCurrentIndex(0);

	if (!lost.isEmpty())
		pv::util::show_message(this, tr("Sessions not restored"),
			tr("These boards are not connected, so the pages they held are "
				"not here:") + QString("\n\n") + lost.join("\n") +
				QString("\n\n") +
				tr("Plug the board in, or close the program that is using it, "
					"then open the page again."),
			pv::util::message_box_icon(QMessageBox::Warning),
			520, false, false, false);

	update_session_roles();
	update_session_tabs();
}

void MainWindow::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange)
		retranslate_ui();

	/* The marks on the tabs are drawn in the colour of the text, so a
	 * change of theme has to draw them again. */
	if (event->type() == QEvent::ApplicationPaletteChange ||
		event->type() == QEvent::PaletteChange) {
		for (int i = 0; i < session_selector_.count(); i++)
			session_selector_.tabBar()->setTabButton(
				i, QTabBar::RightSide, nullptr);
		update_session_tabs();
	}

	QMainWindow::changeEvent(event);
}

void MainWindow::retranslate_ui()
{
	run_stop_button_->setToolTip(tr("Start/Stop Acquisition"));
	repeat_acquisition_button_->setToolTip(tr("Repeat Acquisition"));
	repeat_acquisition_button_->setText(tr("Repeat Acquisition"));
	settings_button_->setToolTip(tr("Settings"));

	/* Dock titles are chosen when the dock is created and are never
	 * looked at again, so give them the text of the new language. */
	for (auto &entry : view_dock_types_)
		entry.first->setWindowTitle(QCoreApplication::translate(
			"pv::views", views::ViewTypeNames[entry.second]));

	for (auto &entry : sub_window_types_) {
		QString title;

		switch (entry.second) {
#ifdef ENABLE_DECODE
		case subwindows::SubWindowTypeDecoderSelector:
			title = tr("Decoder Selector");
			break;
#endif
		default:
			break;
		}

		if (!title.isEmpty())
			entry.first->setWindowTitle(title);
	}

	/* The run/stop button text carries the capture state. */
	if (last_focused_session_)
		update_acq_button(last_focused_session_.get());

	/* The tab tool tips and the header of the strips are built from
	 * translated pieces as well. */
	update_session_tabs();
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
	session_selector_.tabBar()->setVisible(true);
	/* The tabs sit at the bottom: the strip is where the eye goes to change
	 * traces, not something worth the top edge of the waveform area. */
	session_selector_.setTabPosition(QTabWidget::South);

	/*
	 * The buttons of a tab are ours (see update_session_tabs): Qt's own
	 * close button would take the side the copy button needs, and its
	 * "Close Tab" tool tip is not translated by this build.
	 */
	session_selector_.setTabsClosable(false);
	close_application_shortcut_ = new QShortcut(pv::util::key_sequence(Qt::CTRL, Qt::Key_Q), this, SLOT(close()));
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

	/*
	 * The pages are dragged into the order the user wants. The first one
	 * is the capture page and stays at the left end, which is settled when
	 * a drag ends. A double click on a page that was exported into gives
	 * it a name of its own.
	 */
	session_selector_.tabBar()->setMovable(true);
	session_selector_.tabBar()->installEventFilter(this);
	connect(session_selector_.tabBar(), &QTabBar::tabBarDoubleClicked,
		this, &MainWindow::on_tab_double_clicked);

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

	for (auto& entry : session_windows_)
		if (!entry.first->data_saved())
			data_saved = false;

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
	/*
	 * Closing saves the pages, not only the settings of a WCH board: a page
	 * on another make of board would otherwise never be written, and the
	 * next start would restore a state from some earlier run instead.
	 */
	if (last_focused_session_ && last_focused_session_->main_bar()) {
		save_sessions();
		if (last_focused_session_->main_bar()->channels_ &&
			is_wch_device(
				last_focused_session_->main_bar()->channels_->device_name_))
			last_focused_session_->main_bar()->set_setting_default();
	}
	save_ui_settings();

	/*
	 * The files a copied session was read from stay locked for as long as
	 * the session holds them, so they are removed here instead of at the
	 * moment of the copy. The session file is written with its setup
	 * beside it, so that one goes too.
	 */
	for (const QString &path : copy_temp_files_) {
		QFile::remove(path);

		QString setup = path;
		const int dot = setup.lastIndexOf('.');
		if (dot > 0)
			QFile::remove(setup.left(dot) + ".pvs");
	}
	copy_temp_files_.clear();
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
	for (const shared_ptr<Session>& s : sessions_)
		if (s)
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

/*
 * Starts a capture on one page. The mode is the one the bar shows, and the
 * values that bar displays are handed to the device first - they are only
 * pushed when a selector is moved, so a capture started on the values a
 * page was set up with would otherwise run with whatever the driver held.
 */
static void start_capture_on(const shared_ptr<Session> &session, int mode)
{
	if (!session || !session->main_bar())
		return;

	/* A page whose board is away cannot start; its mask goes away with the
	 * pass, and the run shortcut must not get round it. */
	if (session->main_bar()->is_device_offline())
		return;

	session->is_repeat_acquisition_ = (mode == Session::Repeat);
	session->main_bar()->apply_capture_limits();

	session->start_capture([](QString message) {
		Q_UNUSED(message);
		qDebug() << "Capture failed";
	});
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

		const int mode = last_focused_session_->main_bar()->capture_mode_;
		for (shared_ptr<Session> s : hw_sessions){
			if (any_running)
				s->stop_capture();
			else
				start_capture_on(s, mode);
		}
	} else {
		shared_ptr<Session> session = last_focused_session_;
		if (!session)
			return;
		switch (session->get_capture_state()) {
		case Session::Stopped:
			/*
			 * A repeat that has run its last frame is stopped already; the
			 * flag is all that is left over, and the next press starts a
			 * fresh run rather than pretending to stop one.
			 */
			session->is_repeat_acquisition_ = false;
			start_capture_on(session,
				last_focused_session_->main_bar()->capture_mode_);
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
	/*
	 * The dialog is given this window as its parent so that it is a window
	 * of this application rather than a window of its own: it then stays
	 * above the main window, is placed over it, and does not take a place
	 * of its own in the task bar.
	 */
	dialogs::Settings dlg(*device_manager_, this);
	dlg.exec();
}

void MainWindow::on_session_name_changed()
{
	// Update the corresponding dock widget's name(s)
	Session *session = qobject_cast<Session*>(QObject::sender());
	assert(session);
	if (session == nullptr)
		return;

	/*
	 * Every view is named after its view type - the main one shows the
	 * waveforms, not the session - so a session given another name has
	 * nothing to change here.
	 */

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

	/* The tab is named after the session, so it has to be relabelled. */
	update_session_tabs();
}

void MainWindow::on_session_device_changed()
{
	Session *session = qobject_cast<Session*>(QObject::sender());
	assert(session);

	/*
	 * Each page of the strip is named after the board on it, and the
	 * selector offers what is left. Both are read again whichever page
	 * changed: the one in front may well be another, and what happened is
	 * not about it.
	 */
	update_session_roles();

	// Ignore if caller is not the currently focused session
	// unless there is only one session
	if ((sessions_.size() > 1) && (session != last_focused_session_.get()))
		return;

	update_acq_button(session);
}

void MainWindow::on_session_capture_state_changed(int state)
{
	(void)state;

	/* Every page tells on its tab what it holds, not only the one in
	 * front: a capture that just ended appears as a copy button there. */
	update_session_tabs();

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

void MainWindow::update_session_roles()
{
	/* A board a session already holds cannot be captured by a second one,
	 * so its "open in a new session" entry is greyed out everywhere. */
	QSet<QString> busy;
	for (const shared_ptr<Session> &session : sessions_) {
		if (!session || !session->device())
			continue;
		const QString id =
			widgets::DeviceToolButton::device_identity(session->device());
		if (!id.isEmpty())
			busy.insert(id);
	}

	for (const shared_ptr<Session> &session : sessions_) {
		apply_session_role(session);
		if (!session)
			continue;

		if (session->main_bar())
			session->main_bar()->set_busy_devices(busy,
				widgets::DeviceToolButton::device_identity(session->device()));

		/*
		 * A session that captures is named after the board it is on, so the
		 * tab follows when the board changes. One showing a file keeps the
		 * file name instead. Done here rather than in the bar: the name
		 * needs the device manager, and this is the window that holds it.
		 */
		DeviceManager *const manager = device_manager_.load();
		const shared_ptr<devices::Device> device = session->device();
		if (manager && device && !session->using_file_device()) {
			const QString name = QString::fromStdString(
				device->display_name(*manager));
			if (!name.isEmpty() && (name != session->name()))
				session->set_name(name);
		}
	}
}

void MainWindow::apply_session_role(const shared_ptr<Session> &session)
{
	if (!session || !session->main_bar())
		return;

	/* The capture page owns the whole device list and is the only place a
	 * new session can be started from. A session opened for one board
	 * stays with that board. One showing a file has nothing to select. */
	const bool is_default = (session == default_session_);
	session->main_bar()->set_device_selector_role(is_default, !is_default);
	session->main_bar()->set_device_selector_visible(!session->using_file_device());

	/*
	 * A page that shows a file has no device mode to pick, but it keeps the
	 * start button, and so does the page a capture was exported to: both
	 * read the capture again and put the waveform back on screen.
	 */
	const bool from_device = !session->using_file_device();
	session->main_bar()->set_capture_buttons_visible(from_device, true);
}

/*
 * The two marks a tab carries are drawn here, in the colour of the text:
 * the icon files this build ships read as a blob at this size, and the
 * style's close mark is a heavier weight than the one beside it.
 */
static QIcon tab_mark(const QPalette &palette, bool copy, bool dim = false,
	int percent = -1)
{
	const int size = 14;
	const qreal ratio = qApp->devicePixelRatio();
	QPixmap pm(int(size * ratio), int(size * ratio));
	pm.setDevicePixelRatio(ratio);
	pm.fill(Qt::transparent);

	QPainter p(&pm);
	p.setRenderHint(QPainter::Antialiasing, true);

	/*
	 * While the write it asks for is under way the mark gives way to a
	 * ring that fills with it: the tab keeps its size, and how far the
	 * file has got is visible without a dialog in the way.
	 */
	if (percent >= 0) {
		QColor track = palette.color(QPalette::WindowText);
		track.setAlpha(70);

		QPen ring(track);
		ring.setWidthF(1.5);
		p.setPen(ring);
		p.setBrush(Qt::NoBrush);
		p.drawEllipse(QRectF(2.0, 2.0, 10.0, 10.0));

		QPen arc(palette.color(QPalette::WindowText));
		arc.setWidthF(1.5);
		arc.setCapStyle(Qt::RoundCap);
		p.setPen(arc);
		p.drawArc(QRectF(2.0, 2.0, 10.0, 10.0), 90 * 16,
			-qRound(percent * 3.6 * 16.0));
		p.end();
		return QIcon(pm);
	}

	/* Dimmed while the write it asks for is under way: the mark stays
	 * where it was, so the tab does not jump - it only fades. */
	QColor ink = palette.color(QPalette::WindowText);
	if (dim)
		ink.setAlpha(90);

	QPen pen(ink);
	pen.setWidthF(1.1);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	p.setPen(pen);
	p.setBrush(Qt::NoBrush);

	if (!copy) {
		p.drawLine(QPointF(3.2, 3.2), QPointF(10.8, 10.8));
		p.drawLine(QPointF(10.8, 3.2), QPointF(3.2, 10.8));
		p.end();
		return QIcon(pm);
	}

	/* A sheet, and the opened corner of the sheet behind it. */
	p.drawRoundedRect(QRectF(5.0, 5.0, 7.9, 7.9), 1.3, 1.3);

	QPainterPath back;
	back.moveTo(8.9, 2.9);
	back.lineTo(8.9, 2.5);
	back.quadTo(8.9, 1.1, 7.5, 1.1);
	back.lineTo(2.5, 1.1);
	back.quadTo(1.1, 1.1, 1.1, 2.5);
	back.lineTo(1.1, 7.5);
	back.quadTo(1.1, 8.9, 2.5, 8.9);
	back.lineTo(2.9, 8.9);
	p.drawPath(back);
	p.end();

	return QIcon(pm);
}

/*
 * The buttons a page's tab carries on its right: the copy button while the
 * page holds a capture, and the close button after it. A tab takes a single
 * widget per side, so the two share one.
 */
class TabSideWidget : public QWidget
{
public:
	TabSideWidget(QToolButton *copy, QToolButton *close, QWidget *parent) :
		QWidget(parent),
		copy_(copy),
		close_(close)
	{
		QHBoxLayout *const layout = new QHBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(2);
		if (copy_)
			layout->addWidget(copy_);
		if (close_)
			layout->addWidget(close_);
	}

	QToolButton *copy() { return copy_; }
	QToolButton *close() { return close_; }

private:
	QToolButton *copy_;
	QToolButton *close_;
};

void MainWindow::update_session_tabs()
{
	const QFontMetrics fm(session_selector_.tabBar()->font());
	/*
	 * What a tab may take in all, buttons included: enough for the longest
	 * name a board gives a page - "WeAct Studio Logic Analyzer V2
	 * ch375/6-3" - to be shown whole. A name cut in the middle no longer
	 * says which board the page is on. The two 14 px buttons and a little
	 * room for the frame come on top.
	 */
	const int max_width = int(util::text_width(fm,
		QString::fromLatin1(util::WeActV2ModelName) + " ch375/6-3")) + 34;

	for (int i = 0; i < session_selector_.count(); i++) {
		const shared_ptr<Session> session = get_tab_session(i);
		const QString full =
			session ? session->name() : session_selector_.tabText(i);

		/* What the tab carries on its right; the name gets the rest. */
		const bool want_copy = session && session->data_held();
		const bool want_close = session && session != default_session_;
		const int name_width =
			max_width - (want_copy ? 14 : 0) - (want_close ? 14 : 0);

		/* Long names - device, decoder and time - would push the strip
		 * across the window; the whole of it stays in the tool tip, with
		 * what the page holds underneath: a name of a board and a time
		 * says little about which of two such pages is which. */
		session_selector_.setTabText(i,
			fm.elidedText(full, Qt::ElideMiddle, name_width));

		QString tip = full;
		if (session) {
			const shared_ptr<devices::Device> device = session->device();
			const shared_ptr<sigrok::Device> sr_dev =
				device ? device->device() : nullptr;

			if (session->using_file_device()) {
				tip += QString("\n") + tr("Shows a session file");
			} else if (sr_dev) {
				/* Which board, and where it sits: two boards of one model
				 * differ in nothing else, and the port is what a replug
				 * leaves alone. */
				const string port = sr_dev->connection_id();
				if (!port.empty())
					tip += QString("\n") + tr("Port: %1").arg(
						QString::fromStdString(port));

				const string board = sr_dev->model();
				if (!board.empty())
					tip += QString("\n") + tr("Board: %1").arg(
						QString::fromStdString(board));

				const string serial = sr_dev->serial_number();
				if (!serial.empty())
					tip += QString("\n") + tr("SN: %1").arg(
						QString::fromStdString(serial));
			}

			int decoders = 0;
			for (const shared_ptr<data::SignalBase> &sig : session->signalbases())
				if (dynamic_pointer_cast<data::DecodeSignal>(sig))
					decoders++;

			if (decoders > 0)
				tip += QString("\n") + tr("Decoders: %1").arg(decoders);
		}
		session_selector_.setTabToolTip(i, tip);

		/*
		 * One widget on the right of the tab: the copy button while the
		 * page holds a capture, and the close button after it. It is built
		 * for what the tab carries rather than hidden and shown, because a
		 * tab keeps the width it was laid out with - one with a hidden
		 * button in it would keep a blank of that width for ever.
		 */
		TabSideWidget *buttons = dynamic_cast<TabSideWidget *>(
			session_selector_.tabBar()->tabButton(i, QTabBar::RightSide));
		const bool have_copy = buttons && buttons->copy();
		const bool have_close = buttons && buttons->close();

		if (!buttons || have_copy != want_copy || have_close != want_close) {
			if (buttons)
				session_selector_.tabBar()->setTabButton(
					i, QTabBar::RightSide, nullptr);

			QToolButton *copy = nullptr;
			QToolButton *close = nullptr;

			if (want_copy) {
				copy = new QToolButton(session_selector_.tabBar());
				copy->setAutoRaise(true);
				copy->setIcon(tab_mark(palette(), true));
				copy->setIconSize(QSize(14, 14));
				copy->setFixedSize(14, 14);
				connect(copy, &QToolButton::clicked, this, [this, session]() {
					/* The page may be gone by the time the click lands. */
					if (std::find(sessions_.begin(), sessions_.end(),
							session) == sessions_.end())
						return;
					copy_session_to_new_session(session);
				});
			}

			if (want_close) {
				close = new QToolButton(session_selector_.tabBar());
				close->setAutoRaise(true);
				close->setIcon(tab_mark(palette(), false));
				close->setIconSize(QSize(14, 14));
				close->setFixedSize(14, 14);
				connect(close, &QToolButton::clicked, this, [this, session]() {
					for (int k = 0; k < session_selector_.count(); k++)
						if (get_tab_session(k) == session) {
							on_tab_close_requested(k);
							return;
						}
				});
			}

			if (copy || close) {
				buttons = new TabSideWidget(copy, close,
					session_selector_.tabBar());
				session_selector_.tabBar()->setTabButton(
					i, QTabBar::RightSide, buttons);
			} else {
				buttons = nullptr;
			}
		}

		/* Set again on every pass: the texts follow the language, and the
		 * copy button turns into the progress of the write it asked for. */
		if (buttons) {
			if (buttons->copy()) {
				QToolButton *const copy = buttons->copy();
				const bool busy = copy_percent_.contains(session.get());
				const int percent = busy ?
					copy_percent_.value(session.get()) : -1;
				if (copy->property("percent").toInt() != percent) {
					copy->setProperty("percent", percent);
					copy->setIcon(tab_mark(palette(), true, busy, percent));
				}
				copy->setToolTip(busy
					? tr("Exporting the capture... %1%").arg(percent)
					: tr("Export the capture to a new session"));
				/* Grey with the page capturing or with a copy already on its
				 * way: a copy would stop the capture it is taken from. */
				copy->setEnabled(session && !busy &&
					session->can_export_capture());
			}
			if (buttons->close())
				buttons->close()->setToolTip(tr("Close Session"));
		}
	}
}

void MainWindow::on_open_file_requested(QString file_name)
{
	if (file_name.isEmpty())
		return;

	/* The capture page never shows a file, it is where a board is
	 * captured: a file always gets a session of its own. */
	const shared_ptr<Session> session = add_session();
	session->load_file(file_name);
	update_session_roles();
	update_session_tabs();
}

void MainWindow::on_device_new_session_requested(QString identity)
{
	if (identity.isEmpty())
		return;

	DeviceManager *const manager = device_manager_.load();
	if (!manager)
		return;

	shared_ptr<devices::HardwareDevice> pick;
	for (const shared_ptr<devices::HardwareDevice> &dev : manager->devices())
		if (widgets::DeviceToolButton::device_identity(dev) == identity) {
			pick = dev;
			break;
		}

	if (!pick)
		return;

	/* A board another page is on is not a second page's to open: the two
	 * would both run it and each would see the other's samples. The list
	 * greys such a row out, but the request is answered from here as well. */
	if (board_taken(sessions_, nullptr, pick))
		return;

	const shared_ptr<Session> session = add_session();
	session->select_device(pick);

	/*
	 * A board that refused to open would leave an empty tab behind, and a
	 * tab that holds nothing is worse than the request having done nothing.
	 */
	if (session->device() != pick) {
		remove_session(session);
		return;
	}

	/* A session that holds one board is named after it, so the strip says
	 * which board each trace came from. */
	const QString name = QString::fromStdString(pick->display_name(*manager));
	if (!name.isEmpty())
		session->set_name(name);

	update_session_roles();
	update_session_tabs();
}

QString MainWindow::copy_session_name(const shared_ptr<Session> &source)
{
	const shared_ptr<sigrok::Device> sr_dev =
		source->device() ? source->device()->device() : nullptr;

	/*
	 * The model, not what the driver calls its vendor: a WCH board says
	 * "CH32H417" there, which is the chip rather than the board the
	 * capture came from.
	 */
	QString device;
	if (sr_dev)
		device = QString::fromStdString(sr_dev->model());
	if (device.isEmpty() && source->main_bar())
		device = source->main_bar()->get_vendorName();
	if (device.isEmpty() && source->device())
		device = QString::fromStdString(
			source->device()->display_name(*device_manager_.load()));

	/*
	 * The port goes after the model: two boards of one model are told
	 * apart by it, and a page named after the model alone would leave
	 * the user guessing which of them the capture came from.
	 */
	if (sr_dev) {
		const QString port = QString::fromStdString(sr_dev->connection_id());
		if (!port.isEmpty() && !device.contains(port))
			device = device.isEmpty() ? port : device + " " + port;
	}

	QString decoder;
	for (const shared_ptr<data::SignalBase> &sig : source->signalbases()) {
		const shared_ptr<data::DecodeSignal> decode =
			dynamic_pointer_cast<data::DecodeSignal>(sig);
		if (decode) {
			decoder = decode->name();
			break;
		}
	}

	QStringList parts;
	if (!device.isEmpty())
		parts << device;
	if (!decoder.isEmpty())
		parts << decoder;
	parts << QTime::currentTime().toString("HH:mm:ss");
	return parts.join(' ');
}

void MainWindow::on_copy_session_requested()
{
	copy_session_to_new_session(last_focused_session_);
}

/*
 * The page is taken by value, not by reference: the callers hand over the
 * page in front, and adding a page moves that very pointer to the new one -
 * a reference would quietly become the copy, and the copy would be named
 * after itself instead of after the page it came from.
 */
bool MainWindow::copy_session_to_new_session(shared_ptr<Session> source)
{
	if (!source || source->using_file_device())
		return false;

	/* A copy takes the data the page holds, and a capture that is still running
	 * has not written all of it - the write below would stop it halfway. */
	if (!source->can_export_capture())
		return false;

	/* One export per page is enough: the tab says it is busy, and a
	 * second request would write the same capture again. */
	if (copy_percent_.contains(source.get()))
		return false;

	const shared_ptr<sigrok::Context> context = sigrok_context_;
	const shared_ptr<sigrok::OutputFormat> format =
		context ? context->output_formats()["srzip"] : nullptr;
	if (!format)
		return false;

	/*
	 * The data has to leave the session that captured it to be handed to
	 * another one, and a session file is the form both sides already
	 * understand. It is written in the temporary directory and read back
	 * at once, so nothing is left behind.
	 *
	 * The write is compressed on the way out, which takes seconds for a
	 * capture of a few hundred megabytes - long enough for the window to
	 * stop drawing if it were waited for here. StoreSession writes on a
	 * thread of its own; the page is marked busy until it is done.
	 */
	const QString path = QDir::temp().filePath(
		QString("weact-copy-%1.sr").arg(QDateTime::currentMSecsSinceEpoch()));

	source->stop_capture();

	StoreSession *const store = new StoreSession(path.toStdString(), format,
		map<string, Glib::VariantBase>(),
		std::make_pair<uint64_t, uint64_t>(0, 0), *source);
	store->setParent(this);

	/*
	 * A dialog of its own rather than the tab alone: a write of a few
	 * hundred megabytes takes long enough to want a number and a way out.
	 * A small capture is written before the dialog would appear, so it
	 * only shows up when there is something to wait for.
	 */
	auto canceled = std::make_shared<bool>(false);
	QProgressDialog *const progress = new QProgressDialog(
		tr("Exporting the capture... %1%").arg(0), tr("Cancel"), 0, 100,
		this);
	progress->setWindowTitle(tr("Export to a New Session"));
	progress->setWindowModality(Qt::WindowModal);
	progress->setMinimumDuration(400);
	progress->setAutoClose(false);
	progress->setAutoReset(false);

	/* The bar puts a percentage of its own in the middle, and the label
	 * already says one: keep the label, which says what is going on. */
	if (QProgressBar *bar = progress->findChild<QProgressBar *>())
		bar->setTextVisible(false);

	connect(progress, &QProgressDialog::canceled, this,
		[store, canceled]() {
			*canceled = true;
			store->cancel();
		});

	connect(store, &StoreSession::progress_updated, this,
		[this, store, source, progress]() {
			/* After the write is over the dialog is on its way out. */
			if (!copy_percent_.contains(source.get()))
				return;

			const auto got = store->progress();
			const int percent = got.second > 0 ?
				int(100.0 * got.first / got.second) : 0;

			progress->setLabelText(
				tr("Exporting the capture... %1%").arg(percent));
			if (progress->value() != percent)
				progress->setValue(percent);

			if (copy_percent_.value(source.get(), -1) == percent)
				return;
			copy_percent_[source.get()] = percent;
			update_session_tabs();
		});

	connect(store, &StoreSession::store_successful, this,
		[this, store, source, path, progress, canceled]() {
			const QString error = store->error();
			store->wait();
			store->deleteLater();

			progress->reset();
			progress->deleteLater();

			copy_percent_.remove(source.get());
			update_session_tabs();

			if (*canceled) {
				/* Half a capture is of no use to anyone. */
				QFile::remove(path);
				return;
			}

			if (!error.isEmpty()) {
				show_session_info(
					tr("Could not copy the capture: %1").arg(error));
				return;
			}

			const shared_ptr<Session> copy = add_session();
			copy->load_file(path);
			copy->set_name(copy_session_name(source));
			copy->set_snapshot(true);
			/*
			 * The file this page was made from is a copy in the temp
			 * directory, swept away on the way out. Saving the page has to
			 * ask for a place of its own instead of writing there.
			 */
			copy->set_save_path(QString());
			/* Still open in the session that now holds it; the window
			 * sweeps it away on the way out. */
			copy_temp_files_ << path;

			update_session_roles();
			update_session_tabs();
		});

	copy_percent_.insert(source.get(), 0);
	update_session_tabs();

	if (!store->start()) {
		copy_percent_.remove(source.get());
		update_session_tabs();
		show_session_info(
			tr("Could not copy the capture: %1").arg(store->error()));
		store->deleteLater();
		return false;
	}

	return true;
}

bool MainWindow::store_capture_to(const shared_ptr<Session> &session,
	const QString &path)
{
	const shared_ptr<sigrok::Context> context = sigrok_context_;
	if (!context || !session || path.isEmpty())
		return false;

	const shared_ptr<sigrok::OutputFormat> format =
		context->output_formats()["srzip"];
	if (!format)
		return false;

	session->stop_capture();

	/*
	 * Waited for, not left running: the callers here move the page on to
	 * another device as soon as they return, and the capture would be
	 * taken away underneath a write that had only just begun.
	 */
	pv::StoreSession store(path.toStdString(), format,
		map<string, Glib::VariantBase>(),
		std::make_pair<uint64_t, uint64_t>(0, 0), *session);

	if (!store.start()) {
		show_session_info(tr("Could not save the session: %1").arg(store.error()));
		return false;
	}
	store.wait();

	/*
	 * The write can still have gone wrong - a file that could not be put
	 * in place, a disk that filled up - and the caller has to know: it
	 * moves the page on to another board next, and the capture would go
	 * with it.
	 */
	if (!store.error().isEmpty()) {
		show_session_info(tr("Could not save the session: %1").arg(store.error()));
		return false;
	}

	return true;
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

/*
 * The capture page is the first tab and stays there. Its place is put back
 * when the drag is over - doing it while the drag is still running would
 * have the tab bar and this window moving the same tab turn after turn.
 */
void MainWindow::keep_capture_tab_first()
{
	QMainWindow *window = nullptr;
	const auto entry = session_windows_.find(default_session_);
	if (entry != session_windows_.end())
		window = entry->second;
	if (!window)
		return;

	const int index = session_selector_.indexOf(window);
	if (index > 0)
		session_selector_.tabBar()->moveTab(index, 0);
}

void MainWindow::on_tab_double_clicked(int index)
{
	const shared_ptr<Session> session = get_tab_session(index);

	/*
	 * Only a page that was exported into is renamed: the others are named
	 * after the board they hold, and a name of their own would hide which
	 * board that is.
	 */
	if (!session || !session->is_snapshot())
		return;

	finish_tab_rename(false);

	QTabBar *const bar = session_selector_.tabBar();
	QLineEdit *const edit = new QLineEdit(bar);
	edit->setText(session->name());
	edit->selectAll();
	/* The name is edited where it is read, so the editor takes the place of
	 * the tab instead of covering the strip with a dialog. */
	edit->setGeometry(bar->tabRect(index).adjusted(2, 3, -2, -3));
	edit->installEventFilter(this);

	tab_rename_edit_ = edit;
	tab_rename_session_ = session;

	connect(edit, &QLineEdit::editingFinished,
		this, &MainWindow::on_tab_rename_finished);

	edit->show();
	edit->setFocus(Qt::MouseFocusReason);
}

void MainWindow::on_tab_rename_finished()
{
	finish_tab_rename(true);
}

void MainWindow::finish_tab_rename(bool apply)
{
	QLineEdit *const edit = tab_rename_edit_;
	const shared_ptr<Session> session = tab_rename_session_;

	if (!edit)
		return;

	/* Cleared first: taking the editor away can report the edit finished
	 * once more, and there is nothing left to finish by then. */
	tab_rename_edit_ = nullptr;
	tab_rename_session_.reset();

	edit->removeEventFilter(this);
	const QString name = edit->text().trimmed();
	/* Hidden here as well: a delete can be held back for a turn of the
	 * event loop, and the editor would sit over the tab until then. */
	edit->hide();
	edit->deleteLater();

	if (!apply || !session || name.isEmpty() || name == session->name())
		return;

	session->set_name(name);
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
			sub_window_types_.erase(dock);
			dock->close();
			/* The button is pressed exactly while this dock is open. */
			if (session->main_bar())
				session->main_bar()->set_decoder_button_checked(false);
			return;
		}
	}

	// We get a pointer and need a reference
	for (shared_ptr<Session>& s : sessions_)
		if (s.get() == session) {
			add_subwindow(subwindows::SubWindowTypeDecoderSelector, *s);
			if (s->main_bar())
				s->main_bar()->set_decoder_button_checked(true);
		}
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
	/* A decoder dock that is closed from its own button - or with the
	 * escape key, which presses that button - releases the one in the bar
	 * that opened it. */
	if (dock) {
		const auto entry = sub_windows_.find(dock);
		if (entry != sub_windows_.end()) {
			shared_ptr<subwindows::decoder_selector::SubWindow> decoder =
				dynamic_pointer_cast<subwindows::decoder_selector::SubWindow>(
					entry->second);
			if (decoder) {
				Session &session = decoder->session();
				if (session.main_bar())
					session.main_bar()->set_decoder_button_checked(false);
			}
		}
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
    if (obj == tab_rename_edit_ && event->type() == QEvent::KeyPress)
    {
        /* Taken here rather than left to the line edit: the two keys have
         * to end the rename the same way whichever widget has the focus. */
        const int key = static_cast<QKeyEvent *>(event)->key();
        if (key == Qt::Key_Escape) {
            /* Escape drops what was typed and leaves the name as it was. */
            finish_tab_rename(false);
            return true;
        }
        if (key == Qt::Key_Return || key == Qt::Key_Enter) {
            finish_tab_rename(true);
            return true;
        }
    }

    if (obj == session_selector_.tabBar() &&
            event->type() == QEvent::MouseButtonRelease)
    {
        /* After the tab bar has finished the drag, not before it. */
        QTimer::singleShot(0, this, [this]() { keep_capture_tab_first(); });
    }

    if (event->type() == QEvent::Show)
    {
        if (QWidget *w = qobject_cast<QWidget*>(obj))
        {
			/* Only standalone top-level windows have decorations of their own. */
			if (w->isWindow()) {
				GlobalSettings settings;
				util::apply_window_theme(w, settings.current_theme_is_dark());
			}
        }
    }
    return QObject::eventFilter(obj, event);
}

} // namespace pv
