/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2026 Q2H2 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef PULSEVIEW_PV_DIALOGS_IAPDIALOG_HPP
#define PULSEVIEW_PV_DIALOGS_IAPDIALOG_HPP

#include <QDialog>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <memory>
#include <cstdint>

/* IAP command definitions */
#define CMD_IAP_PROM    0x80    /* Program command */
#define CMD_IAP_ERASE   0x81    /* Erase command */
#define CMD_IAP_VERIFY  0x82    /* Verify command */
#define CMD_IAP_END     0x83    /* End marker */
#define CMD_IAP_SUCC    0x84    /* Success marker */

/* IAP error codes */
#define IAP_ERR_UNKNOWN        0   /* Unknown error */
#define IAP_ERR_OVERTIME       1   /* Timeout */
#define IAP_ERR_CHECK          2   /* Checksum mismatch */
#define IAP_ERR_ADDR           3   /* Invalid address */
#define IAP_ERR_ERASE_FAIL     4   /* Erase failed */
#define IAP_ERR_PROG_NO_ERASE  5   /* Write without erasing */
#define IAP_ERR_WRITE_FAIL     6   /* Write failed */
#define IAP_ERR_VERIFY         7   /* Verification failed */

#define IAP_DATA_LEN           64  /* IAP packet length */
#define HID_PACKET_LEN         65  /* HID packet length (including the report ID) */
#define HID_REPORT_ID          0   /* HID Report ID */
#define HID_TIMEOUT_MS         500 /* HID timeout in ms */

namespace pv {

class Session;

namespace dialogs {

class IAPWorker;

/**
 * IAP firmware upgrade
 *
 */
class IAPDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent widget
     * @param device_index CH32H417 device index (used for ch375 communication)
     */
    IAPDialog(QWidget *parent, int device_index);
    ~IAPDialog();

    /**
     * Set the VID and PID used in IAP mode
     * @param vid
     * @param pid
     */
    void set_iap_vid_pid(uint16_t vid, uint16_t pid);

    /**
     * Set the firmware file path
     * @param path Firmware file path
     */
    void set_firmware_path(const QString &path);

Q_SIGNALS:
    /**
     * Upgrade finished signal
     * @param success Whether the upgrade succeeded
     */
    void upgrade_finished(bool success);

private Q_SLOTS:
    void on_browse_clicked();
    void on_start_clicked();
    void on_cancel_clicked();

    void on_progress_updated(int percent, const QString &status);
    void on_log_message(const QString &message);
    void on_upgrade_finished(bool success);

private:
    void setup_ui();
    void log(const QString &message);
    void update_progress(int percent, const QString &status);

private:
    int device_index_;              /* CH32H417 device index */
    uint16_t iap_vid_;              /* IAP mode VID */
    uint16_t iap_pid_;              /* IAP mode PID */
    QString firmware_path_;         /* Firmware file path */

    QLineEdit *file_edit_;          /* File path line edit */
    QPushButton *browse_btn_;       /* Browse button */
    QPushButton *start_btn_;        /* Start button */
    QPushButton *cancel_btn_;       /* Cancel button */
    QProgressBar *progress_bar_;    /* Progress bar */
    QLabel *status_label_;          /* Status label */
    QTextEdit *log_text_;           /* Log text box */

    IAPWorker *worker_;             /* Upgrade worker object */
    QThread *worker_thread_;        /* Upgrade worker thread */
    bool is_upgrading_;             /* Whether an upgrade is running */
};

/**
 * IAP upgrade worker
 *
 * Runs the IAP upgrade on a background thread so the UI stays responsive
 */
class IAPWorker : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param device_index CH32H417 device index
     * @param firmware_path Firmware file path
     * @param iap_vid IAP mode VID
     * @param iap_pid IAP mode PID
     */
    IAPWorker(int device_index, const QString &firmware_path,
              uint16_t iap_vid, uint16_t iap_pid, bool already_in_hid_mode = false);
    ~IAPWorker();

    void start();

Q_SIGNALS:
    void progress_updated(int percent, const QString &status);
    void log_message(const QString &message);
    void finished(bool success);

public Q_SLOTS:
    void do_work();
    void cancel();

private:
    /* HID device operations */
    bool init_hid();
    void close_hid();
    bool find_hid_device();
    bool hid_write(const uint8_t *data, size_t len);
    bool hid_read(uint8_t *data, size_t len, int timeout_ms);

    /* IAP operations */
    bool enter_iap_mode();
    bool erase_flash(uint32_t start_addr);
    bool program_flash(const uint8_t *data, size_t len);
    bool verify_flash(uint32_t start_addr, const uint8_t *data, size_t len);
    bool end_upgrade();

    /* File handling */
    bool read_firmware_file(QByteArray &out_data, uint32_t &start_addr);
    bool hex_to_bin(const QByteArray &hex_data, QByteArray &bin_data, uint32_t &start_addr);

    /* Logging helpers */
    void log(const QString &message);
    void update_progress(int percent, const QString &status);

private:
    int device_index_;
    QString firmware_path_;
    uint16_t iap_vid_;
    uint16_t iap_pid_;

    void *hid_handle_;              /* hidapi device handle */
    bool already_in_hid_mode_;      /* Whether the device is already in HID mode */
    bool cancelled_;                /* Cancellation flag */
    QMutex mutex_;                  /* Mutex */
};

} // namespace dialogs
} // namespace pv

#endif // PULSEVIEW_PV_DIALOGS_IAPDIALOG_HPP
