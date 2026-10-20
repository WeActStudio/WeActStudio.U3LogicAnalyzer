/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
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

#ifndef PULSEVIEW_PV_TOOLBARS_MAINBAR_HPP
#define PULSEVIEW_PV_TOOLBARS_MAINBAR_HPP

#include <cstdint>
#include <list>
#include <memory>
#include <QString>
#include <QDialog>
#include <glibmm/variant.h>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMenu>
#include <QPixmap>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <pv/popups/channels.hpp>
#include <pv/session.hpp>
#include <pv/views/trace/standardbar.hpp>
#include <pv/widgets/devicetoolbutton.hpp>
#include <pv/widgets/popuptoolbutton.hpp>
#include <pv/widgets/sweeptimingwidget.hpp>
#include <pv/widgets/hovertoolbutton.hpp>
#include <thread>
#include <mutex>
#include <QFile>
#include <libusb.h>
#include <hidapi/hidapi.h>

#include "../../../libsigrok/src/hardware/wch-ch32h417/ch375_wrapper.h"

Q_DECLARE_METATYPE(unsigned long)

using std::shared_ptr;

namespace sigrok {
class Device;
class InputFormat;
class OutputFormat;
}

Q_DECLARE_METATYPE(shared_ptr<sigrok::Device>)

class QAction;
class QEvent;

namespace pv {

class MainWindow;
class Session;

namespace popups {
class DeviceOptions;
}

namespace views {
namespace trace {
class View;
}
}

namespace toolbars {

class MainBar : public pv::views::trace::StandardBar
{
	Q_OBJECT

private:
	static const uint64_t MinSampleCount;
	static const uint64_t MaxSampleCount;
	static const uint64_t DefaultSampleCount;
	static const uint64_t MinSampleRate;
	uint64_t max_sample_rate_ = 50000000ULL;
	/**
	 * Name of the setting used to remember the directory
	 * containing the last file that was opened.
	 */
	static const char *SettingOpenDirectory;

	/**
	 * Name of the setting used to remember the directory
	 * containing the last file that was saved.
	 */
	static const char *SettingSaveDirectory;
	std::thread t_hot_plug;
	QString m_vendorName;
	QString m_device_driver;
	QString m_device_identity;
	QSet<QString> m_busy_identities;
	std::mutex vendorName_mutex;
	
public:
	MainBar(Session &session, QWidget *parent,
		pv::views::trace::View *view);
	~MainBar();
	void update_device_list();
	/*
	 * Refreshes what the selector offers without touching the device page
	 * or the view. For a session that kept its board: a rescan hands that
	 * board back as a new instance, and building the page again would drop
	 * its triggers and the labels its decoders put on the channels - all
	 * for a change that is only in the list.
	 */
	void refresh_device_selector();

	/* Takes a device into the session and brings the bar in line with it:
	 * vendor, selector label, pattern mode and the rollback of a board that
	 * will not open. A device picked in the list and the replacement a
	 * detach picks go through the same call, so the two cannot drift apart.
	 * ask is false when the caller has already settled the data question. */
	bool switch_to_device(std::shared_ptr<devices::Device> device, bool ask);
	void update_runstop_status(int state);

	void set_capture_state(pv::Session::capture_state state);

	void reset_device_selector();

	QAction* action_new_view() const;
	QAction* action_open() const;
	QAction* action_save() const;
	QAction* action_save_as() const;
	QAction* action_save_selection_as() const;
	QAction* action_restore_setup() const;
	QAction* action_save_setup() const;
	QAction* action_connect() const;
	
	uint64_t get_sample_count();
	uint64_t get_sample_rate();

	uint16_t get_channel_number() const;
	uint64_t get_trigger_offset();
	uint16_t get_sample_count_index();
	void set_device_selector_name(QString device_selector);
	/* Keeps the decoder button pressed while its dock is open. */
	void set_decoder_button_checked(bool on);
	void hot_plug_start();
	/* The page hosting the poll, if any: it goes with that page, and the window
	 * hands it to one of the pages that are left. */
	static MainBar *hot_plug_owner();
	void hot_plug_stop();
	void set_vendorName(QString vendorName);
	QString get_vendorName();
	/* What the device selector offers depends on the session it sits in:
	 * only the capture page that owns every board can open a new session,
	 * and a session opened for one board stays with that board. A session
	 * showing a file has nothing to select at all. */
	void set_device_selector_role(bool new_session_allowed,
		bool single_device_only);
	void set_busy_devices(const QSet<QString> &busy, const QString &own);
	/* What every page holds, for the poll: it has to tell whose board a
	 * chain belonged to. */
	QSet<QString> get_busy_identities();
	/* Covers the page while its board is away and a capture elsewhere keeps
	 * the pass waiting: nothing here can be done until that capture ends.
	 * The page is handed in: a session lives in a tab, so window() would be
	 * the whole program and not this page. */
	void set_device_offline(bool offline, QWidget *page = nullptr);
	bool is_device_offline() const;

	/**
	 * Hands the capture values this page shows to the device. Needed
	 * before a capture starts: they are otherwise only pushed when one of
	 * the selectors is moved, so a page started on the values it was set
	 * up with would leave the driver holding whatever it had - and with
	 * the sample limit at zero a capture never reaches its end.
	 */
	void apply_capture_limits();

private:
	/** Sets the rate the page shows on the device; false if it refused. */
	bool push_sample_rate(const shared_ptr<sigrok::Device> &sr_dev);

public:
	void set_device_selector_visible(bool visible);

	/**
	 * The mode button and the run button both act on a device: what to do
	 * when it is started, and starting it at all. A page that shows a file
	 * has nothing to start, and a snapshot has nothing to ask for either.
	 */
	void set_capture_buttons_visible(bool mode, bool run);

	/** The directory the last session file was saved in, for the next one. */
	static QString last_save_directory();
	static void remember_save_directory(const QString &path);

	/* A device scan replaces the manager; the selector must not keep
	 * pointing at the one that was just freed. */
	void update_device_manager(DeviceManager *manager);

	/* Rebuilds the device page after the device went away without another
	 * one taking its place. */
	void refresh_device_widgets();

private:
	void run_stop();

	void select_init_device();

	void save_selection_to_file();

	void update_sample_rate_selector();
	void update_sample_rate_selector_value();
	void update_sample_count_selector();
	void update_device_config_widgets();
	void commit_sample_rate();
	void commit_sample_count(bool remember = true);
	void commit_channel_number(uint64_t channelNum);
	void commit_threshold_value(int index, float thresholdLevel);
	void hot_plug_listen();
	/* The driver backing the device in use; a refresh rescans only the
	 * drivers that are not in use, so it is cached here. */
	void set_device_driver(QString driver);
	QString get_device_driver();
	/* Which board this page holds, as the poll must see it: whether the one
	 * in use is still there decides if a pass can wait for a capture. */
	void set_device_identity(QString identity);
	QString get_device_identity();

	QString offline_mask_text() const;
	/* QPointer: a page can be closed before its bar, and the mask is a child
	 * of that page. */
	QPointer<QWidget> offline_mask_;
	QPointer<QWidget> offline_mask_page_;
	QPointer<QLabel> offline_mask_label_;
	bool device_offline_ = false;
	void renew_samplecount_samplerate(uint64_t samplerate,uint64_t samplerate_max);
	void get_decoder_list_default(QString decoder_name);
	QString setting_group() const;
	/* Whether a saved group was written by the board this page is on. */
	bool setting_group_is_ours(QSettings &settings, const QString &group);

	static void ch32h417_notify_callback(unsigned long eventStatus);
	static MainBar* s_mainbar_instance;

private Q_SLOTS:
	void show_session_error(const QString text, const QString info_text);
	void on_refresh_device_list();

	void export_file(shared_ptr<sigrok::OutputFormat> format,
		bool selection_only = false, QString file_name = "");
	void import_file(shared_ptr<sigrok::InputFormat> format);

	void on_device_selected();
	void on_device_changed();
	void on_capture_state_changed(int state);
	void on_sample_count_changed();
	void on_sample_rate_changed();
	void channels_button_ex_clicked();
	void on_config_changed();

	void on_actionSingle_triggered();
	void on_actionRepeat_triggered();
	
	void on_action_version_info_triggered();
    void on_action_feedback_triggered();
    void on_action_update_triggered();
	void on_action_use_guide_triggered();
    void on_action_about_triggered();

	void on_actionNewView_triggered(QAction* action = nullptr);

	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionSaveAs_triggered();
	void on_actionSaveSelectionAs_triggered();

	void on_actionSaveSetup_triggered();
	void on_actionRestoreSetup_triggered();

	void on_actionConnect_triggered();

	
	void on_add_math_signal_clicked();

	void on_update_max_sample_rate(uint64_t maxSampleRate);
	void on_update_channel_count(uint16_t channelCount);  // Handle a CH32H417 channel count change
	void on_update_threshold_value_1(float thresholdValue);
	void on_update_threshold_value_2(float thresholdValue);
	void on_update_threshold_value_ch32h417(float thresholdValue);  // CH32H417-specific threshold handling
	void on_update_pre_trigger(uint8_t value);
	void on_mode_changed(int mode);
	void on_adc_channel_changed(int channel);  // Handle an ADC channel switch
	void on_adc_precision_changed(int precision);  // Handle an ADC resolution switch
	void on_threshold_value_raw_changed(double value);  // Set the threshold voltage value
	void on_ch32h417_device_event(unsigned long eventStatus);
	void on_channel_selection_changed();  // Handle a channel selection change, used to update the channel height
	void update_device_status_icon();  // Update the device status icon
public Q_SLOTS:
	void on_add_decoder_clicked();
	
protected:
	void add_toolbar_widgets();

	bool eventFilter(QObject *watched, QEvent *event);

	/* Called when the application language changed. */
	void changeEvent(QEvent *event) override;
	void retranslate_ui();

Q_SIGNALS:
	void new_view(Session *session, int type);
	void show_decoder_selector(Session *session);
	void device_attached();
	/* The boards these identities stood for are gone; the pages that hold
	 * them can say so while a pass waits for a capture to end. */
	void devices_went_offline(QStringList identities);

	/* A board came or went in a driver the session is not using, so only the
	 * device list has to catch up. */
	void refresh_device_list();
	void run_stop_button_clicked();
	void settings_triggered();
	void ch32h417_device_event_signal(unsigned long eventStatus);

	/* A file was picked in this bar's menu. Whether it lands in this session
	 * or in a new one is the window's decision, not the bar's. */
	void open_file_requested(QString file_name);

	/* Forwarded from the device selector: open this board in a session of
	 * its own. */
	void device_new_session_requested(QString identity);

	/* The save menu was asked to put what has been captured into a session
	 * of its own. */
	void copy_session_requested();

private:
	QAction *const action_new_view_;
	// QAction *const action_run_stop_;
	QAction *action_new_binary_;
	QAction *action_new_tabular_;
	QAction *const action_open_;
	QAction *const action_save_;
	QAction *const action_save_as_;
	QAction *const action_save_selection_as_;
	QAction *const action_restore_setup_;
	QAction *const action_save_setup_;
	QAction *const action_connect_;

	/* The tool bar entry that holds the device selector, and the save menu
	 * entry that copies the trace into a session of its own. */
	QAction *device_selector_action_ = nullptr;
	QAction *model_change_action_ = nullptr;
	QAction *run_stop_action_ = nullptr;
	QAction *action_copy_to_new_session_ = nullptr;

	/* Menus and entries whose text is a translation and which the
	 * constructor used to own as local variables. */
	QMenu *menu_file_export_ = nullptr;
	QMenu *menu_file_import_ = nullptr;
	QMenu *menu_new_view_ = nullptr;
	QMenu *menu_model_view_ = nullptr;
	QMenu *menu_help_view_ = nullptr;
	QAction *action_single_ = nullptr;
	QAction *action_repeat_ = nullptr;
	QAction *action_settings_ = nullptr;
	QAction *action_version_info_ = nullptr;
	QAction *action_update_ = nullptr;
	QAction *action_about_ = nullptr;

	QToolButton *run_stop_button_;
	pv::widgets::HoverToolButton *model_change_button_;
	pv::widgets::HoverToolButton *new_view_button_;
	QToolButton *open_button_, *save_button_;
	pv::widgets::HoverToolButton *help_button_;
	QToolButton *device_status_icon_;

	pv::widgets::DeviceToolButton device_selector_;

	// pv::widgets::PopupToolButton configure_button_;
	// QAction *configure_button_action_;

	pv::widgets::PopupToolButton channels_button_;
	QToolButton * channels_button_ex_;
	QAction *channels_button_action_;

	pv::widgets::SweepTimingWidget sample_count_;
	pv::widgets::SweepTimingWidget sample_rate_;
	bool updating_sample_rate_;
	bool updating_sample_count_;

	bool sample_count_supported_;

	atomic<bool> hot_plug_listen_;
	/* The poll is the hot-plug source once it runs; the CH375 callback is
	 * a fallback and the upgrade report only. */
	atomic<bool> poll_running_{ false };
	/* A capture on this page; the poll leaves a pass to the end of it. */
	atomic<bool> capture_running_{ false };
	/* Captures running over all pages, and a pass put off until the last of
	 * them ends. Static: the poll of one page sees the captures of the others. */
	static atomic<int> s_captures_running;
	static atomic<bool> s_attach_deferred;
	/* Which boards those captures are on, read by the poll under the same lock
	 * as the count: what matters is whether a captured board was the one that
	 * went, not whether this page's own board did. */
	static std::mutex s_capture_mutex;
	static QSet<QString> s_capturing_identities;
	/* Read from the hot-plug polling thread as well as the main thread. */
	atomic<bool> iap_upgrading_;
	/*
	 * Monotonic deadline after which the upgrade flag is cleared even if the
	 * device never came back, so a lost device cannot silence hot-plug for
	 * good. Written on the main thread, read from the polling thread.
	 */
	atomic<int64_t> iap_upgrade_deadline_;

	bool is_count_setting_default_;
	bool is_rate_setting_default_;
	/* Nothing is put back into a page whose board has no settings saved. */
	bool has_setting_default_ = false;
	/*
	 * Set while renew_setting_default() walks the settings back into the
	 * widgets: the commits it triggers would otherwise write the very
	 * values they are in the middle of reading.
	 */
	bool restoring_defaults_ = false;

	/*
	 * Dragging the pre-trigger slider calls the save on every step, and
	 * each of those writes the whole settings file. The write is held back
	 * until the dragging stops instead.
	 */
	QTimer *setting_save_timer_ = nullptr;
	static const int SettingSaveDelayMs = 300;

	uint64_t sample_count_value_ = 12500;
	uint64_t sample_rate_value_ = SR_KHZ(500);
	uint16_t channel_number_;
	/*
	 * Which of the channel modes the user picked - the 2/4/8/16 choice in
	 * the device page, not the channel count the board is told to sample.
	 * The two differ: a board that only samples 8 or 16 still lets two or
	 * four of them be shown, and that choice has to come back as it was.
	 */
	int channel_mode_index_ = 3;
	float thresholdValue_1_ = 1.0f;
	float thresholdValue_2_ = 1.0f;
	float thresholdValue_ch32h417_ = 2.3f;
	uint8_t pre_trigger_ = 0;
	uint64_t hardware_version_ = 0;

public:
	pv::popups::Channels*  channels_;
	/* Holds the device bindings; the config_changed signal is reported
	 * through it, so it lives as long as the device it describes. */
	pv::popups::DeviceOptions *device_options_ = nullptr;
	int is_repeat_acq_default_ = 0;
	float threshold_bank_1_default_ = 1.0;
	float threshold_bank_2_default_ = 1.0;
	float threshold_ch32h417_default_ = 2.3f;
	uint8_t channel_number_default_ = 16;
	uint8_t pre_trigger_default_ = 0;
	uint64_t sample_count_default_index_ = 0;
	uint64_t sample_rate_default_index_ = 0;
	QStringList decoder_list_default_;
	int capture_mode_ = Session::Single;
	void get_setting_default();
	void set_setting_default();
	/* The write set_setting_default() holds back; see SettingSaveDelayMs. */
	void write_setting_default();
	void renew_setting_default();
	void save_setup(QSettings &settings) const;
	void restore_setup(QSettings &settings);
	void renew_default_bank_threshold();
	void show_hardware_version();
	QDialog channel_dlg;
#ifdef ENABLE_DECODE
	QToolButton *add_decoder_button_;
#endif

	QToolButton *add_math_signal_button_;

	/**
	 * Set while the device change the user has just agreed to is under
	 * way: the switch itself walks the same path again (the device
	 * list is rebuilt as part of it), and asking a second time would
	 * be asking about the very change that was just allowed.
	 */
	bool device_switch_allowed_ = false;
};

} // namespace toolbars
} // namespace pv

#endif // PULSEVIEW_PV_TOOLBARS_MAINBAR_HPP
