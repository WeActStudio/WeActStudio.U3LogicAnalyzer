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

#ifndef PULSEVIEW_PV_MAINWINDOW_HPP
#define PULSEVIEW_PV_MAINWINDOW_HPP

#include <list>
#include <map>
#include <memory>
#include <vector>

#include <QMainWindow>
#include <QShortcut>
#include <QTabWidget>
#include <QToolButton>
#include <QString>
#include <QHash>
#include <QStringList>
#include <QCheckBox>
#include <QDialog>
#include <QPointer>
#include <future>
#include "devicemanager.hpp"
#include "session.hpp"
#include "subwindows/subwindowbase.hpp"
#include "subwindows/decoder_selector/subwindow.hpp"
using std::list;
using std::map;
using std::shared_ptr;
using std::string;

struct srd_decoder;

class QEvent;
class QVBoxLayout;

namespace pv {

class DeviceManager;

namespace toolbars {
class ContextBar;
class MainBar;
}

namespace view {
class View;
class ViewBase;
}

namespace widgets {
#ifdef ENABLE_DECODE
class DecoderMenu;
#endif
}

using pv::views::ViewBase;
using pv::views::ViewType;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	static QString WindowTitle;

public:
	static bool checkSystemRequirements();

	/** Offer to install the udev access rules (Linux only, no-op elsewhere). */
	static bool checkUdevRules();
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

	static void show_session_error(const QString text, const QString info_text,
		QWidget *parent = nullptr);
 	
	shared_ptr<views::ViewBase> get_active_view() const;

	shared_ptr<views::ViewBase> add_view(ViewType type, Session &session);

	void remove_view(shared_ptr<ViewBase> view);

	shared_ptr<subwindows::SubWindowBase> add_subwindow(
		subwindows::SubWindowType type, Session &session);

	shared_ptr<Session> add_session();

	void remove_session(shared_ptr<Session> session);

	/**
	 * Point every session at the given device manager.
	 *
	 * A session only keeps a raw pointer to the manager, so replacing the
	 * manager has to be propagated to all of them: updating just the focused
	 * session leaves the rest holding a pointer to a deleted object.
	 */
	void update_sessions_device_manager(pv::DeviceManager *device_manager);

	/**
	 * Let every session take the instance the last scan made for the board
	 * it is on, keeping what the session holds, and say whose board is no
	 * longer on the list.
	 *
	 * A scan hands out new instances, so each board that stayed plugged in
	 * has to be taken back - by the session that is on it, and without the
	 * data going away with the instance. The session whose board is gone is
	 * returned for the detach question.
	 */
	shared_ptr<Session> adopt_device_in_sessions();

	/**
	 * Looks the device up by its identity, which is what tells two boards
	 * of the same model apart. Null when no device reports that identity.
	 */
	shared_ptr<devices::HardwareDevice> find_device_by_identity(
		const QString &identity);

	/**
	 * The same board under another port. A port is part of an identity, and
	 * a board that re-enumerates can come back on a different one - an FX2
	 * lands on another bus number, for one. The serial number survives that
	 * and is what is matched; it is not unique, so the answer is only taken
	 * when exactly one free board answers to it.
	 */
	shared_ptr<devices::HardwareDevice> find_moved_board(
		const shared_ptr<Session> &session);

	void add_session_with_file(string open_file_name, string open_file_format,
		string open_setup_file_name);

	void add_default_session();

	void save_sessions();
	void restore_sessions();
	void add_default_setting();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

	/* Called when the application language changed. */
	void changeEvent(QEvent *event) override;


private:
	void retranslate_ui();
	void setup_ui();
	void update_acq_button(Session *session);

	void save_ui_settings();
	void restore_ui_settings();

	shared_ptr<Session> get_tab_session(int index) const;

	void closeEvent(QCloseEvent *event);

	virtual QMenu* createPopupMenu();

	virtual bool restoreState(const QByteArray &state, int version = 0);

	void show_session_info_with_auto_close(const QString info_text);

Q_SIGNALS:
	void show_error();
	void save_session();

public Q_SLOTS:
	void on_run_stop_clicked();
	void on_show_error(QString err);
	void show_session_info(const QString info_text);
	void show_device_detach(const shared_ptr<Session> &session);
	void on_get_repeat_acquisition(bool& is_repeat_acquisition_state);
	void on_device_attached();
	void run_device_attached();

private:
	void do_device_detach_switch(const shared_ptr<Session> &session);

	/* Tells every bar what its device selector may offer: the capture page
	 * lists every board, a session opened for one board lists that one,
	 * and a session showing a file hides the selector outright. */
	void update_session_roles();

	/* Names the tabs, eliding what is too long for the strip and keeping
	 * the full name in its tool tip. The capture page keeps no close
	 * button: it is the window's way back to a device. */
	void update_session_tabs();

	void apply_session_role(const shared_ptr<Session> &session);

	/* Names a copied session after the board it came from, the decoder that
	 * was put on the trace, and the moment it was copied. */
	QString copy_session_name(const shared_ptr<Session> &session);

	/**
	 * Hands the capture of one page to a page of its own. True when there
	 * was something to hand over: the question asked when a board goes
	 * away keeps the data this way, since the page itself cannot keep it -
	 * the board it was capturing from is the one that left.
	 */
	bool copy_session_to_new_session(shared_ptr<Session> source);

	/**
	 * Writes the capture of a page into a session file and waits for it to
	 * be there. Returns false when nothing was written.
	 */
	bool store_capture_to(const shared_ptr<Session> &session,
		const QString &path);

	/* The answer to the detach question: read the device list again, move
	 * the session to what is there, and let device events through. */
	void apply_device_detach(const shared_ptr<Session> &session);

	/* Tells the user about the drivers whose scan had to be given up on,
	 * so a device that is missing from the list has a reason on screen. */
	void report_stalled_devices();

	/* Lets device events through again. A change that arrived meanwhile is
	 * run again only when replay_pending is set: the answer to the detach
	 * question has just read the whole device list, and running the pass
	 * again would put the same question to the user twice. */
	void resume_device_events(bool replay_pending = true);
	
private Q_SLOTS:
	void on_add_view(ViewType type, Session *session);

	/**
	 * Sets a dock up the way the views are: it is stacked with the ones
	 * already in the window, and can be taken out and put back with a
	 * double click on its title bar. Shared by the views and the sub
	 * windows.
	 */
	void make_dock_floatable(QDockWidget *dock);

	void on_focus_changed();
	void on_focused_session_changed(shared_ptr<Session> session);

	void on_new_session_clicked();
	void on_settings_clicked();

	void on_session_name_changed();
	void on_session_device_changed();
	void on_session_capture_state_changed(int state);

	void on_sample_rate_too_low(qulonglong configured, qulonglong reason);

	void on_new_view(Session *session, int view_type);
	void on_view_close_clicked();

	void on_tab_changed(int index);
	void on_tab_close_requested(int index);

	/* A file was chosen in a session's menu: it always gets a session of
	 * its own, the capture page never shows a file. */
	void on_open_file_requested(QString file_name);

	/* A board was asked to open in a session of its own, and the save menu
	 * asked to copy the captured trace into one. */
	void on_device_new_session_requested(QString identity);
	void on_copy_session_requested();

	void on_show_decoder_selector(Session *session);
	void on_sub_window_close_clicked();

	void on_view_colored_bg_shortcut();
	void on_view_sticky_scrolling_shortcut();
	void on_view_show_sampling_points_shortcut();
	void on_view_show_analog_minor_grid_shortcut();

	void on_close_current_tab();

	void on_repeat_acquisition_clicked();

	void on_close_decoder_dock(Session *session);

private:
	atomic<DeviceManager *> device_manager_;
	// DeviceManager *device_manager_;

	/*
	 * The one and only libsigrok Context of this process. Every
	 * sigrok::Context destructor calls sr_exit(), which tears down the
	 * global backend and frees the device instances of *all* drivers,
	 * including those created by a newer context. Rescans must therefore
	 * always reuse this context instead of creating a second one.
	 */
	shared_ptr<sigrok::Context> sigrok_context_;

	list< shared_ptr<Session> > sessions_;
	shared_ptr<Session> last_focused_session_;

	/* The capture page the window starts with. It is the one session that
	 * cannot be closed and never shows a file. */
	shared_ptr<Session> default_session_;

	/* Session files written to copy a capture into another session. They
	 * are deleted on exit, once nothing holds them open. */
	QStringList copy_temp_files_;

	/* The pages whose capture is being written out for a copy, with how
	 * far the write has got: the tab shows it, and a second click is
	 * ignored. */
	QHash<const Session*, int> copy_percent_;

	map< QDockWidget*, shared_ptr<views::ViewBase> > view_docks_;
	map< QDockWidget*, shared_ptr<subwindows::SubWindowBase> > sub_windows_;
	/* Views and sub windows whose title came from a translated string,
	 * remembered as the type it was derived from. */
	map< QDockWidget*, int > view_dock_types_;
	map< QDockWidget*, int > sub_window_types_;
	/* The layout of each session window as it was last known to be good,
	 * kept so a view that comes back from a float - and takes the float's
	 * geometry with it, outside the window - can be put right by handing
	 * the whole layout back to the window. */
	map< QMainWindow*, QByteArray > window_layouts_;
	map< shared_ptr<Session>, QMainWindow*> session_windows_;

	QWidget *static_tab_widget_;
	// QToolButton *new_session_button_;
	QToolButton *run_stop_button_, *settings_button_;
	QCheckBox* repeat_acquisition_button_;
	QTabWidget session_selector_;

	QIcon icon_red_;
	QIcon icon_green_;
	QIcon icon_grey_;

	QShortcut *view_sticky_scrolling_shortcut_;
	QShortcut *view_show_sampling_points_shortcut_;
	QShortcut *view_show_analog_minor_grid_shortcut_;
	QShortcut *view_colored_bg_shortcut_;
	QShortcut *run_stop_shortcut_;
	QShortcut *close_application_shortcut_;
	QShortcut *close_current_tab_shortcut_;
	QString using_device_name;
	bool is_repeat_acq_ = false;
	/* True while the link stop warning is on screen. */
	bool link_stop_message_visible_ = false;
	bool is_wch_device_ = false;
	bool attach_process_done = true;
	/* Set when a device change arrived while the previous one was still being
	 * handled; it is handled as soon as that pass is done. */
	bool device_event_pending_ = false;
	/*
	 * One board being plugged is reported more than once, so the pass that
	 * answers it is held back a little and the notices that arrive in the
	 * meantime join it. Without this, one plug means several rescans.
	 */
	bool device_event_queued_ = false;
	static const int DeviceEventDebounceMs = 150;
	/* The detach question while it is on screen. Only one is ever asked: a
	 * second copy would sit exactly on top of the first, and answering one
	 * would only reveal the other. */
	QPointer<QDialog> detach_dialog_;

	/* The board the capture page is waiting to get back, by identity. */
	QString waiting_for_board_;

	/* A pass that found a page without its board gets one more look before
	 * the board is taken for gone: a scan can miss one while the bus is
	 * being re-enumerated. */
	bool attach_retry_done_ = false;
};

} // namespace pv

#endif // PULSEVIEW_PV_MAINWINDOW_HPP
