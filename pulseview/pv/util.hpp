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

#ifndef PULSEVIEW_PV_UTIL_HPP
#define PULSEVIEW_PV_UTIL_HPP

#include <cmath>
#include <functional>
#include <string>
#include <vector>

#ifndef Q_MOC_RUN
// Workaround for https://github.com/boostorg/serialization/issues/186
#include <boost/serialization/nvp.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>
#endif

#include <QMetaType>
#include <QString>
#include <QFontMetrics>
#include <QKeySequence>
#include <QMessageBox>

using std::string;
using std::vector;

class QMouseEvent;

namespace pv {
namespace util {

enum class WchDeviceType {
	None,
	CH569,
	CH32H417
};

static const char* const DeviceModelNames[] =
{
    "Unknown",
    "CH569",
    "CH32H417",
};

WchDeviceType get_wch_device_type(const QString& vendorName);
WchDeviceType get_wch_device_type_by_driver(const string& driverName);
bool is_wch_device(const QString& vendorName);

/**
 * Driver names that can reach a CH32H417, in preference order.
 *
 * The device is reachable either through the vendor CH375DLL or through
 * libusb, depending on which driver the OS bound it to; the two can never
 * claim the same device instance at the same time.
 */
const vector<string>& get_ch32h417_driver_names();

enum class TimeUnit {
	None = 0,
	Time = 1,
	Samples = 2
};

enum class SIPrefix {
	unspecified = -1,
	yocto, zepto,
	atto, femto, pico,
	nano, micro, milli,
	none,
	kilo, mega, giga,
	tera, peta, exa,
	zetta, yotta
};

/// Returns the exponent that corresponds to a given prefix.
int exponent(SIPrefix prefix);

/// Timestamp type providing yoctosecond resolution.
typedef boost::multiprecision::number<
	boost::multiprecision::cpp_dec_float<24>,
	boost::multiprecision::et_off> Timestamp;

/**
 * Chooses a prefix so that the value in front of the decimal point is between 1 and 999.
 */
SIPrefix determine_value_prefix(double v);

/**
 * Formats a given timestamp with the specified SI prefix.
 *
 * If 'prefix' is left 'unspecified', the function chooses a prefix so that
 * the value in front of the decimal point is between 1 and 999.
 *
 * The default value "s" for the unit argument makes the most sense when
 * formatting time values, but a different value can be given if the function
 * is reused to format a value of another quantity.
 *
 * @param t The value to format.
 * @param prefix The SI prefix to use.
 * @param precision The number of digits after the decimal separator.
 * @param unit The unit of quantity.
 * @param sign Whether or not to add a sign also for positive numbers.
 *
 * @return The formatted value.
 */
QString format_time_si(const Timestamp& v,
	SIPrefix prefix = SIPrefix::unspecified, unsigned precision = 0,
	QString unit = "s", bool sign = true);

/**
 * Formats a given value into a representation using SI units.
 *
 * If 'prefix' is left 'unspecified', the function chooses a prefix so that
 * the value in front of the decimal point is between 1 and 999.
 *
 * @param value The value to format.
 * @param prefix The SI prefix to use.
 * @param precision The number of digits after the decimal separator.
 * @param unit The unit of quantity.
 * @param sign Whether or not to add a sign also for positive numbers.
 *
 * @return The formatted value.
 */
QString format_value_si(double v,
	SIPrefix prefix = SIPrefix::unspecified, unsigned precision = 0,
	QString unit = "", bool sign = true);

/**
 * Wrapper around 'format_time_si()' that interprets the given 'precision'
 * value as the number of decimal places if the timestamp would be formatted
 * without a SI prefix (using 'SIPrefix::none') and adjusts the precision to
 * match the given 'prefix'
 *
 * @param t The value to format.
 * @param prefix The SI prefix to use.
 * @param precision The number of digits after the decimal separator if the
 *        'prefix' would be 'SIPrefix::none', see above for more information.
 * @param unit The unit of quantity.
 * @param sign Whether or not to add a sign also for positive numbers.
 *
 * @return The formatted value.
 */
QString format_time_si_adjusted(const Timestamp& t, SIPrefix prefix,
	unsigned precision = 0, QString unit = "s", bool sign = true);

/**
 * Formats the given timestamp using "[+-]DD:HH:MM:SS.mmm uuu nnn ppp..." format.
 *
 * "DD" and "HH" are left out if they would be "00" (but if "DD" is generated,
 * "HH" is also always generated. The "MM:SS" part is always produced, the
 * number of subsecond digits can be influenced using the 'precision' parameter.
 *
 * @param t The value to format.
 * @param precision The number of digits after the decimal separator.
 * @param sign Whether or not to add a sign also for positive numbers.
 *
 * @return The formatted value.
 */
QString format_time_minutes(const Timestamp& t, signed precision = 0,
	bool sign = true);

vector<string> split_string(string text, string separator);

/**
 * Return the width of a string in a given font.
 * @param[in] metric metrics of the font
 * @param[in] string the string whose width should be determined
 *
 * @return width of the string in pixels
 */
std::streamsize text_width(const QFontMetrics &metric, const QString &string);


/**
 * Where a mouse event happened on the screen, as opposed to
 * mouse_position(), which is relative to the widget.
 *
 * @param[in] event The mouse event to look at.
 */
QPoint mouse_global_position(const QMouseEvent *event);


/**
 * Where a mouse event happened, as a point of the widget it went to.
 *
 * QMouseEvent::x() and y() are on their way out in Qt 6, and the
 * position() that replaces them does not exist in Qt 5, hence the two
 * ways.
 *
 * @param[in] event The mouse event to look at.
 */
QPoint mouse_position(const QMouseEvent *event);


/**
 * A key sequence of one modifier and one key, such as Ctrl+O.
 *
 * Qt 6 wants the two put together with | and has Qt::operator+ on its way
 * out, while Qt 5 has nothing to combine them into, hence the two ways.
 *
 * @param[in] modifier The modifier, such as Qt::CTRL.
 * @param[in] key The key, such as Qt::Key_O.
 */
/* A template, as the type of Qt::CTRL is not the same everywhere: Qt 6
 * and Qt 5.15 have Qt::Modifier, older Qt 5 versions only have
 * Qt::KeyboardModifier. */
template <typename Modifier>
QKeySequence key_sequence(Modifier modifier, Qt::Key key)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	return QKeySequence(modifier | key);
#else
	return QKeySequence(modifier + key);
#endif
}


/**
 * The pixmap of one of the standard message box icons, in the size the
 * style says a message box uses.
 *
 * QMessageBox::standardIcon() is on its way out, and the icons it hands
 * out come from the style anyway, so ask the style directly. Works the
 * same in Qt 5 and Qt 6.
 *
 * @param[in] icon Which of the standard icons is wanted.
 * @return The icon as a pixmap, or a null pixmap for an icon that has no
 *  standard counterpart.
 */
QPixmap message_box_icon(QMessageBox::Icon icon);


/**
 * Show the custom styled message dialog used throughout the application:
 * a slightly larger font, an icon on the left, the message text and a
 * single "Confirm" button.
 *
 * @param[in] parent Parent widget, also used to place the dialog.
 * @param[in] title Window title.
 * @param[in] text Message body.
 * @param[in] icon Icon shown on the left hand side.
 * @param[in] min_width Minimum dialog width in pixels; 0 to leave it
 *  to the layout.
 * @param[in] rich_text Whether @a text has to be rendered as rich text.
 * @param[in] open_links Whether hyperlinks in @a text open in a browser.
 * @param[in] modal Whether to block until the user confirms.
 */
void show_message(QWidget *parent, const QString &title, const QString &text,
	const QPixmap &icon, int min_width = 400, bool rich_text = false,
	bool open_links = false, bool modal = true);

/**
 * How much physical memory is left for allocation right now, in bytes.
 *
 * Windows reports the physical availability, Linux reads MemAvailable from
 * /proc/meminfo, which is the figure that accounts for what the page cache
 * can hand back, and macOS the free plus inactive pages. Returns 0 when the
 * figure cannot be had.
 */
uint64_t available_memory();

/**
 * The udev rule files the application ships that are not installed yet.
 *
 * Without them the /dev/bus/usb nodes stay owned by root and libusb_open()
 * fails with LIBUSB_ERROR_ACCESS: the device shows up in lsusb but is
 * invisible to the application. Both fields are empty where udev does not
 * exist, or when every rule file is already in place.
 */
struct MissingUdevRules {
	QString devices;  //!< one "what it is for" line per missing file
	QString paths;    //!< one path per missing file, for the dialog
};

/**
 * Which of those rule files are missing.
 */
MissingUdevRules udev_rules_missing();

/**
 * Install every missing rule file as root, authenticating with @a password,
 * which is fed to "sudo -S" on stdin. An empty password works when the
 * process already runs as root or when sudo does not ask for one.
 *
 * @param[in] password The user's password, never passed on a command line.
 * @param[out] error_out Human readable reason on failure.
 * @retval true The rules are installed and udev has been reloaded.
 */
bool install_udev_rules(const std::string &password, std::string &error_out);

/** Overload taking one of the standard message box icons. */
void show_message(QWidget *parent, const QString &title, const QString &text,
	QMessageBox::Icon icon, int min_width = 400, bool rich_text = false,
	bool open_links = false, bool modal = true);

/**
 * Apply the current theme to the decorations the window manager draws.
 *
 * Those are not part of the Qt style sheet, so a title bar that was
 * made dark has to be told about a switch back to a bright theme as
 * well. Does nothing where the platform has no such attribute.
 */
void apply_window_theme(QWidget *window, bool dark);

/**
 * The color scheme the desktop asks for.
 *
 * Every current Linux desktop announces it through the freedesktop
 * portal, which is where this reads it. Qt 6 has an answer of its own,
 * but only where its platform theme follows the desktop, and it answers
 * "unknown" everywhere else, so the portal is asked too.
 */
enum class DesktopColorScheme {
	Unknown,  ///< The desktop does not say, or it was not asked.
	Light,
	Dark
};

DesktopColorScheme desktop_color_scheme();

/**
 * Calls @c callback whenever the color scheme of the desktop changes.
 *
 * The change is announced in a way of its own on every platform: a
 * broadcast message on Windows, and the freedesktop portal on Linux,
 * which Qt does not follow on its own. Does nothing where neither is
 * there.
 */
void watch_color_scheme(const std::function<void()> &callback);

} // namespace util
} // namespace pv

Q_DECLARE_METATYPE(pv::util::Timestamp)

#endif // PULSEVIEW_PV_UTIL_HPP
