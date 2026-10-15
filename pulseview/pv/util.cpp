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

#include "util.hpp"

#include <extdef.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#ifdef __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_host.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#endif

#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QTimer>
#include <QWidget>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

#ifdef PV_HAVE_QTDBUS
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusVariant>
#endif

using std::fixed;
using std::max;
using std::ostringstream;
using std::setfill;
using std::setprecision;
using std::showpos;
using std::string;

using namespace Qt;

namespace pv {
namespace util {

WchDeviceType get_wch_device_type(const QString& vendorName) {
	if (vendorName == "CH569")
		return WchDeviceType::CH569;
	if (vendorName == "CH32H417")
		return WchDeviceType::CH32H417;
	return WchDeviceType::None;
}

WchDeviceType get_wch_device_type_by_driver(const string& driverName) {
	if (driverName == "wch-ch569")
		return WchDeviceType::CH569;
	/* The CH32H417 has two backends: the vendor CH375DLL and libusb. */
	if (driverName == "wch-ch32h417" ||
		driverName == "wch-ch32h417-libusb")
		return WchDeviceType::CH32H417;
	return WchDeviceType::None;
}

const vector<string>& get_ch32h417_driver_names() {
	static const vector<string> names = {
		"wch-ch32h417",
		"wch-ch32h417-libusb",
	};
	return names;
}

bool is_wch_device(const QString& vendorName) {
	return get_wch_device_type(vendorName) != WchDeviceType::None;
}

static QTextStream& operator<<(QTextStream& stream, SIPrefix prefix)
{
	switch (prefix) {
	case SIPrefix::yocto: return stream << 'y';
	case SIPrefix::zepto: return stream << 'z';
	case SIPrefix::atto:  return stream << 'a';
	case SIPrefix::femto: return stream << 'f';
	case SIPrefix::pico:  return stream << 'p';
	case SIPrefix::nano:  return stream << 'n';
	case SIPrefix::micro: return stream << QChar(0x03BC);
	case SIPrefix::milli: return stream << 'm';
	case SIPrefix::kilo:  return stream << 'k';
	case SIPrefix::mega:  return stream << 'M';
	case SIPrefix::giga:  return stream << 'G';
	case SIPrefix::tera:  return stream << 'T';
	case SIPrefix::peta:  return stream << 'P';
	case SIPrefix::exa:   return stream << 'E';
	case SIPrefix::zetta: return stream << 'Z';
	case SIPrefix::yotta: return stream << 'Y';

	default: return stream;
	}
}

int exponent(SIPrefix prefix)
{
	return 3 * (static_cast<int>(prefix) - static_cast<int>(SIPrefix::none));
}

static SIPrefix successor(SIPrefix prefix)
{
	assert(prefix != SIPrefix::yotta);
	return static_cast<SIPrefix>(static_cast<int>(prefix) + 1);
}

// Insert the timestamp value into the stream in fixed-point notation
// (and honor the precision)
static QTextStream& operator<<(QTextStream& stream, const Timestamp& t)
{
	// The multiprecision types already have a function and a stream insertion
	// operator to convert them to a string, however these functions abuse a
	// precision value of zero to print all available decimal places instead of
	// none, and the boost authors refuse to fix this because they don't want
	// to break buggy code that relies on this bug.
	// (https://svn.boost.org/trac/boost/ticket/10103)
	// Therefore we have to work around the case where precision is zero.

	int precision = stream.realNumberPrecision();

	ostringstream ss;
	ss << fixed;

	if (stream.numberFlags() & QTextStream::ForceSign)
		ss << showpos;

	if (0 == precision)
		ss << setprecision(1) << round(t);
	else
		ss << setprecision(precision) << t;

	string str(ss.str());
	if (0 == precision) {
		// remove the separator and the unwanted decimal place
		str.resize(str.size() - 2);
	}

	return stream << QString::fromStdString(str);
}

SIPrefix determine_value_prefix(double v)
{
	SIPrefix prefix;

	if (v == 0) {
		prefix = SIPrefix::none;
	} else {
		int exp = exponent(SIPrefix::yotta);
		prefix = SIPrefix::yocto;
		while ((fabs(v) * pow(10, exp)) > 999 &&
				prefix < SIPrefix::yotta) {
			prefix = successor(prefix);
			exp -= 3;
		}
	}

	return prefix;
}

QString format_time_si(const Timestamp& v, SIPrefix prefix,
	unsigned int precision, QString unit, bool sign)
{
	if (prefix == SIPrefix::unspecified)
		prefix = determine_value_prefix(v.convert_to<double>());

	assert(prefix >= SIPrefix::yocto);
	assert(prefix <= SIPrefix::yotta);

	const Timestamp multiplier = pow(Timestamp(10), -exponent(prefix));

	QString s;
	QTextStream ts(&s);
	if (sign && !v.is_zero())
		ts.setNumberFlags(ts.numberFlags() | QTextStream::ForceSign);
	ts << qSetRealNumberPrecision(precision) << (v * multiplier);
	ts << ' ' << prefix << unit;

	return s;
}

QString format_value_si(double v, SIPrefix prefix, unsigned precision,
	QString unit, bool sign)
{
	if (prefix == SIPrefix::unspecified) {
		prefix = determine_value_prefix(v);

		const int prefix_order = -exponent(prefix);
		precision = (prefix >= SIPrefix::none) ? max((int)(precision + prefix_order), 0) :
			max((int)(precision - prefix_order), 0);
	}

	assert(prefix >= SIPrefix::yocto);
	assert(prefix <= SIPrefix::yotta);

	const double multiplier = pow(10.0, -exponent(prefix));

	QString s;
	QTextStream ts(&s);
	if (sign && (v != 0))
		ts.setNumberFlags(ts.numberFlags() | QTextStream::ForceSign);
	ts.setRealNumberNotation(QTextStream::FixedNotation);
	ts.setRealNumberPrecision(precision);
	ts << (v * multiplier) << ' ' << prefix << unit;

	return s;
}

QString format_time_si_adjusted(const Timestamp& t, SIPrefix prefix,
	unsigned precision, QString unit, bool sign)
{
	// The precision is always given without taking the prefix into account
	// so we need to deduct the number of decimals the prefix might imply
	const int prefix_order = -exponent(prefix);

	const unsigned int relative_prec =
		(prefix >= SIPrefix::none) ? precision :
		max((int)(precision - prefix_order), 0);

	return format_time_si(t, prefix, relative_prec, unit, sign);
}

// Helper for 'format_time_minutes()'.
static QString pad_number(unsigned int number, int length)
{
	return QString("%1").arg(number, length, 10, QChar('0'));
}

QString format_time_minutes(const Timestamp& t, signed precision, bool sign)
{
	const Timestamp whole_seconds = floor(abs(t));
	const Timestamp days = floor(whole_seconds / (60 * 60 * 24));
	const unsigned int hours = fmod(whole_seconds / (60 * 60), 24).convert_to<uint>();
	const unsigned int minutes = fmod(whole_seconds / 60, 60).convert_to<uint>();
	const unsigned int seconds = fmod(whole_seconds, 60).convert_to<uint>();

	QString s;
	QTextStream ts(&s);

	if (t < 0)
		ts << "-";
	else if (sign)
		ts << "+";

	bool use_padding = false;

	// DD
	if (days) {
		ts << days.str().c_str() << ":";
		use_padding = true;
	}

	// HH
	if (hours || days) {
		ts << pad_number(hours, use_padding ? 2 : 0) << ":";
		use_padding = true;
	}

	// MM
	ts << pad_number(minutes, use_padding ? 2 : 0);

	ts << ":";

	// SS
	ts << pad_number(seconds, 2);

	if (precision) {
		ts << ".";

		const Timestamp fraction = fabs(t) - whole_seconds;

		ostringstream ss;
		ss << fixed << setprecision(precision) << setfill('0') << fraction;
		string fs = ss.str();

		// Copy all digits, inserting spaces as unit separators
		for (int i = 1; i <= precision; i++) {
			// Start at index 2 to skip the "0." at the beginning
			ts << fs.at(1 + i);

			if ((i > 0) && (i % 3 == 0) && (i != precision))
				ts << " ";
		}
	}

	return s;
}

/**
 * Split a string into tokens at occurences of the separator.
 *
 * @param[in] text The input string to split.
 * @param[in] separator The delimiter between tokens.
 *
 * @return A vector of broken down tokens.
 */
vector<string> split_string(string text, string separator)
{
	vector<string> result;
	size_t pos;

	while ((pos = text.find(separator)) != std::string::npos) {
		result.push_back(text.substr(0, pos));
		text = text.substr(pos + separator.length());
	}
	result.push_back(text);

	return result;
}

/**
 * Return the width of a string in a given font.
 *
 * @param[in] metric metrics of the font
 * @param[in] string the string whose width should be determined
 *
 * @return width of the string in pixels
 */
std::streamsize text_width(const QFontMetrics &metric, const QString &string)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	return metric.horizontalAdvance(string);
#else
	return metric.width(string);
#endif
}


QPoint mouse_global_position(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	return event->globalPosition().toPoint();
#else
	return event->globalPos();
#endif
}


QPoint mouse_position(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	return event->position().toPoint();
#else
	return event->pos();
#endif
}


QPixmap message_box_icon(QMessageBox::Icon icon)
{
	QStyle::StandardPixmap standard_pixmap;

	switch (icon) {
	case QMessageBox::Information:
		standard_pixmap = QStyle::SP_MessageBoxInformation;
		break;
	case QMessageBox::Warning:
		standard_pixmap = QStyle::SP_MessageBoxWarning;
		break;
	case QMessageBox::Critical:
		standard_pixmap = QStyle::SP_MessageBoxCritical;
		break;
	case QMessageBox::Question:
		standard_pixmap = QStyle::SP_MessageBoxQuestion;
		break;
	default:
		return QPixmap();
	}

	QStyle *style = QApplication::style();

	int size = style->pixelMetric(QStyle::PM_MessageBoxIconSize);
	if (size <= 0)
		size = 32;

	return style->standardIcon(standard_pixmap).pixmap(QSize(size, size));
}


void show_message(QWidget *parent, const QString &title, const QString &text,
	const QPixmap &icon, int min_width, bool rich_text, bool open_links,
	bool modal)
{
	QDialog *dialog = new QDialog(parent);
	dialog->setWindowTitle(title);
	// These dialogs never need a context help button.
	dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	if (!modal) {
		dialog->setWindowModality(Qt::NonModal);
		dialog->setAttribute(Qt::WA_DeleteOnClose);
	}

	// Use a slightly larger font than the parent.
	QFont font = dialog->font();
	font.setPointSize(font.pointSize() + 2);
	dialog->setFont(font);

	// The dialog height is computed automatically.
	if (min_width > 0)
		dialog->setMinimumWidth(min_width);

	QVBoxLayout *layout = new QVBoxLayout(dialog);

	// Icon on the left, message text on the right.
	QHBoxLayout *content_layout = new QHBoxLayout();

	QLabel *icon_label = new QLabel(dialog);
	icon_label->setPixmap(icon);
	icon_label->setAlignment(Qt::AlignTop);
	content_layout->addWidget(icon_label);

	QLabel *label = new QLabel(dialog);
	label->setText(text);
	label->setStyleSheet(QString("font-size: %1pt;").arg(font.pointSize()));
	label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	if (rich_text)
		label->setTextFormat(Qt::RichText);
	if (open_links)
		label->setOpenExternalLinks(true);
	content_layout->addWidget(label, 1);

	layout->addLayout(content_layout);

	// A single confirmation button, right aligned. The translation context
	// matches the one the callers used before this helper existed.
	QHBoxLayout *button_layout = new QHBoxLayout();
	button_layout->addStretch();
	QPushButton *ok_button = new QPushButton(
		QCoreApplication::translate("pv::toolbars::MainBar", "Confirm"), dialog);
	button_layout->addWidget(ok_button);
	layout->addLayout(button_layout);

	QObject::connect(ok_button, &QPushButton::clicked, dialog, &QDialog::accept);

	if (modal) {
		dialog->exec();
		delete dialog;
	} else {
		dialog->show();
	}
}

void show_message(QWidget *parent, const QString &title, const QString &text,
	QMessageBox::Icon icon, int min_width, bool rich_text, bool open_links,
	bool modal)
{
	show_message(parent, title, text, message_box_icon(icon),
		min_width, rich_text, open_links, modal);
}

#ifdef __linux__

namespace {

/* Where udev reads its rules from; ours goes into the first one. */
const char *kUdevRulesDirs[] = {
	"/etc/udev/rules.d",
	"/run/udev/rules.d",
	"/usr/lib/udev/rules.d",
	"/lib/udev/rules.d",
};

struct UdevRuleId {
	const char *vid;
	const char *pid;
};

struct UdevRuleSet {
	const char *label;      /* what it is for, named in the install dialog */
	const char *path;       /* where our copy of the file belongs */
	const char *text;       /* the file itself, so nothing has to be shipped */
	const UdevRuleId *ids;  /* the devices it opens up */
	int id_count;
};

const UdevRuleId kCh32h417Ids[] = {
	{ "1a86", "5537" },
	{ "1a86", "fe17" },
};

const UdevRuleId kFx2lafwIds[] = {
	{ "1d50", "608c" },
	{ "1d50", "608d" },
};

/* One file per analyzer family: the CH32H417 and the FX2 based boards. */
const UdevRuleSet kUdevRuleSets[] = {
	{
		"WeAct Studio CH32H417 logic analyzer (VID 1a86 / PID 5537)",
		"/etc/udev/rules.d/99-weact-ch32h417.rules",
		"# WeAct Studio CH32H417 Logic Analyzer - udev access rules\n"
		"# 5537: runtime interfaces (command / logic data / ADC data)\n"
		"# fe17: HID interface used for the IAP firmware upgrade\n"
		"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"5537\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n"
		"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"fe17\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n"
		"SUBSYSTEM==\"hidraw\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"fe17\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n",
		kCh32h417Ids, 2,
	},
	{
		"FX2 based logic analyzer (VID 1d50 / PID 608c, 608d)",
		"/etc/udev/rules.d/99-fx2lafw.rules",
		"# FX2 based logic analyzer (sigrok fx2lafw) - udev access rules\n"
		"# 608c: 8 channel, 608d: 16 channel\n"
		"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1d50\", ATTRS{idProduct}==\"608c\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n"
		"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1d50\", ATTRS{idProduct}==\"608d\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n",
		kFx2lafwIds, 2,
	},
};

/*
 * Whether a rule for this USB ID is installed somewhere already. Any file in
 * the rules directories counts, whatever it is called: somebody else's file,
 * or an older copy of ours under another name, opens the device just as
 * well, and installing ours next to it would only duplicate it. Quotes and
 * spacing are stripped, so only the two IDs matter.
 */
bool udev_id_is_covered(const UdevRuleId &id)
{
	const QString want_vendor = QString("idVendor}==") + id.vid;
	const QString want_product = QString("idProduct}==") + id.pid;

	for (const char *dir : kUdevRulesDirs) {
		const QDir rules_dir(dir);
		const QStringList names =
			rules_dir.entryList(QStringList("*.rules"), QDir::Files);

		for (const QString &name : names) {
			QFile file(rules_dir.filePath(name));
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				continue;

			QString text = QString::fromUtf8(file.readAll());
			text.remove('"');
			text.remove(' ');
			text.remove('\t');

			for (const QString &line : text.split('\n')) {
				if (line.contains(want_vendor) &&
				    line.contains(want_product))
					return true;
			}
		}
	}

	return false;
}

/*
 * One device of the set being covered is enough: that is what the application
 * needs to find the analyzer. The other entries are for things a user may not
 * have - the IAP interface, the 16 channel board.
 */
bool udev_rule_set_is_installed(const UdevRuleSet &set)
{
	if (access(set.path, F_OK) == 0)
		return true;

	for (int i = 0; i < set.id_count; ++i)
		if (udev_id_is_covered(set.ids[i]))
			return true;

	return false;
}

} // namespace

MissingUdevRules udev_rules_missing()
{
	MissingUdevRules missing;

	for (const UdevRuleSet &set : kUdevRuleSets) {
		if (udev_rule_set_is_installed(set))
			continue;
		if (!missing.devices.isEmpty())
			missing.devices += "\n";
		missing.devices += QString::fromUtf8(set.label);
		if (!missing.paths.isEmpty())
			missing.paths += "\n";
		missing.paths += QString::fromUtf8(set.path);
	}

	return missing;
}

bool install_udev_rules(const std::string &password, std::string &error_out)
{
	error_out.clear();

	/* Every missing file is staged in /tmp first: sudo only copies them into
	 * place, and the password travels on stdin, never on a command line. */
	std::vector<std::string> staged;
	std::string cmd = "sudo -S -p '' sh -c '";

	for (const UdevRuleSet &set : kUdevRuleSets) {
		if (udev_rule_set_is_installed(set))
			continue;

		char tmp_path[] = "/tmp/udev-rules-XXXXXX";
		const int fd = mkstemp(tmp_path);
		if (fd < 0) {
			error_out = "Failed to create a temporary file";
			break;
		}
		FILE *f = fdopen(fd, "w");
		if (!f) {
			close(fd);
			unlink(tmp_path);
			error_out = "Failed to write the temporary rules file";
			break;
		}
		fputs(set.text, f);
		fclose(f);

		staged.push_back(tmp_path);
		cmd += "cp " + staged.back() + " " + set.path;
		cmd += std::string(" && chmod 0644 ") + set.path + " && ";
	}

	if (!error_out.empty()) {
		for (const std::string &path : staged)
			unlink(path.c_str());
		return false;
	}
	if (staged.empty())
		return true;  /* everything is in place already */

	char err_path[] = "/tmp/udev-rules-err-XXXXXX";
	const int err_fd = mkstemp(err_path);
	if (err_fd >= 0)
		close(err_fd);
	else
		err_path[0] = '\0';

	cmd += "(udevadm control --reload-rules || true)";
	cmd += " && (udevadm trigger --subsystem-match=usb || true)";
	cmd += "'";
	if (err_path[0])
		cmd += std::string(" 2>") + err_path;

	FILE *p = popen(cmd.c_str(), "w");
	if (!p) {
		for (const std::string &path : staged)
			unlink(path.c_str());
		if (err_path[0])
			unlink(err_path);
		error_out = "Failed to run sudo";
		return false;
	}
	/* The password travels on stdin only, it never hits a command line. */
	if (!password.empty())
		fwrite(password.data(), 1, password.size(), p);
	fputc('\n', p);
	const int rc = pclose(p);
	for (const std::string &path : staged)
		unlink(path.c_str());

	if (rc != 0) {
		error_out = "sudo rejected the password, or sudo is not available";
		if (err_path[0]) {
			if (FILE *ef = fopen(err_path, "r")) {
				char line[512];
				std::string captured;
				while (fgets(line, sizeof(line), ef))
					captured += line;
				fclose(ef);
				if (!captured.empty())
					error_out = captured;
			}
			unlink(err_path);
		}
		return false;
	}
	if (err_path[0])
		unlink(err_path);
	return true;
}

#else

MissingUdevRules udev_rules_missing()
{
	return MissingUdevRules();
}

bool install_udev_rules(const std::string &password, std::string &error_out)
{
	(void)password;
	error_out = "udev rules only apply to Linux";
	return false;
}

#endif

uint64_t available_memory()
{
#ifdef _WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (!GlobalMemoryStatusEx(&statex))
		return 0;

	return (uint64_t)statex.ullAvailPhys;
#elif defined(__APPLE__)
	vm_size_t page_size = 0;
	if (host_page_size(mach_host_self(), &page_size) != KERN_SUCCESS)
		return 0;

	vm_statistics64_data_t vmstat;
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
	    (host_info64_t)&vmstat, &count) != KERN_SUCCESS)
		return 0;

	return (uint64_t)(vmstat.free_count + vmstat.inactive_count) *
		(uint64_t)page_size;
#else
	/*
	 * MemAvailable is the estimate that counts what the page cache can give
	 * back; sysinfo().freeram leaves that out and under-reports on almost
	 * every system.
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
		return (uint64_t)avail_kb * 1024;

	struct sysinfo si;
	if (sysinfo(&si) == 0)
		return (uint64_t)si.freeram * si.mem_unit;

	return 0;
#endif
}

void apply_window_theme(QWidget *window, bool dark)
{
	if (!window)
		return;

#ifdef _WIN32
	/* 20 is DWMWA_USE_IMMERSIVE_DARK_MODE, which older SDKs do not name. */
	HWND hwnd = reinterpret_cast<HWND>(window->winId());
	if (!hwnd)
		return;

	BOOL use_dark = dark ? TRUE : FALSE;
	if (FAILED(DwmSetWindowAttribute(hwnd, 20, &use_dark, sizeof(use_dark))))
		return;

	/*
	 * 35 is DWMWA_CAPTION_COLOR, which the systems before Windows 11 do
	 * not have: the call fails there and is left alone, so that the dark
	 * mode that was just set stands as it always did. Where it is taken,
	 * the bar is given one colour for both states, which is what it is
	 * after - the system draws the bar of the window that has the focus in
	 * one shade and the bar of a window that does not in another.
	 *
	 * The colour the system draws is 0xFFFFFFFF, DWMWA_COLOR_DEFAULT, which
	 * puts the bar back to it when the bright theme comes on again.
	 */
	const COLORREF caption = dark ? RGB(0x2a, 0x2a, 0x2a) :
		static_cast<COLORREF>(0xFFFFFFFF);
	(void)DwmSetWindowAttribute(hwnd, 35, &caption, sizeof(caption));

	MARGINS margins = {0, 0, 0, dark ? 1 : 0};
	DwmExtendFrameIntoClientArea(hwnd, &margins);
#else
	(void)dark;
#endif
}

namespace {

#ifdef PV_HAVE_QTDBUS
const char *const PortalService = "org.freedesktop.portal.Desktop";
const char *const PortalPath = "/org/freedesktop/portal/desktop";
const char *const PortalSettings = "org.freedesktop.portal.Settings";
const char *const PortalAppearance = "org.freedesktop.appearance";
#endif

/*
 * Asks the freedesktop portal which color scheme the desktop is set to.
 *
 * 0 is "the desktop has no preference", 1 is dark, 2 is light, and -1 is
 * "the desktop did not answer", which is what a build without Qt DBus
 * gets as well.
 */
int portal_color_scheme()
{
#ifdef PV_HAVE_QTDBUS
	QDBusInterface portal(QLatin1String(PortalService),
		QLatin1String(PortalPath), QLatin1String(PortalSettings),
		QDBusConnection::sessionBus());
	if (!portal.isValid())
		return -1;

	const QDBusMessage reply = portal.call(QLatin1String("Read"),
		QLatin1String(PortalAppearance), QLatin1String("color-scheme"));
	if (reply.type() == QDBusMessage::ErrorMessage ||
		reply.arguments().isEmpty())
		return -1;

	/*
	 * The answer is a variant, and so is the value the portal wraps the
	 * setting in, so two layers have to be taken off before the number
	 * is in reach.
	 */
	QVariant value = reply.arguments().at(0);
	for (int i = 0; i < 2 && value.canConvert<QDBusVariant>(); ++i)
		value = value.value<QDBusVariant>().variant();

	bool ok = false;
	const uint scheme = value.toUInt(&ok);

	return ok ? static_cast<int>(scheme) : -1;
#else
	return -1;
#endif
}

} // namespace

DesktopColorScheme desktop_color_scheme()
{
	switch (portal_color_scheme()) {
	case 1:
		return DesktopColorScheme::Dark;
	case 2:
		return DesktopColorScheme::Light;
	default:
		return DesktopColorScheme::Unknown;
	}
}

#ifdef _WIN32
namespace {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
typedef qintptr native_event_result;
#else
typedef long native_event_result;
#endif

/*
 * Watches the color scheme of the desktop.
 *
 * Changing it makes Windows broadcast a WM_SETTINGCHANGE message that
 * carries the string "ImmersiveColorSet". Qt 6 turns that into
 * QStyleHints::colorSchemeChanged, Qt 5 has no such signal, so the
 * message itself is caught here. That is what lets a theme which
 * follows the system follow a change that is made while the program is
 * running.
 */
class ColorSchemeWatcher : public QAbstractNativeEventFilter
{
public:
	explicit ColorSchemeWatcher(std::function<void()> callback) :
		callback_(std::move(callback))
	{
	}

	bool nativeEventFilter(const QByteArray &event_type, void *message,
		native_event_result *result) override
	{
		(void)result;

		if (event_type != "windows_generic_MSG" &&
			event_type != "windows_dispatcher_MSG")
			return false;

		const MSG *msg = static_cast<const MSG *>(message);
		if (msg->message != WM_SETTINGCHANGE || !msg->lParam)
			return false;

		if (QString::fromWCharArray(
			reinterpret_cast<const wchar_t *>(msg->lParam)) !=
			QLatin1String("ImmersiveColorSet"))
			return false;

		callback_();

		return false;
	}

private:
	std::function<void()> callback_;
};

} // namespace

void watch_color_scheme(const std::function<void()> &callback)
{
	/*
	 * The event dispatcher is handed the watcher and does not take
	 * ownership of it, so it is left alive on purpose: it has to outlive
	 * the application.
	 */
	QCoreApplication::instance()->installNativeEventFilter(
		new ColorSchemeWatcher(callback));
}

#else

void watch_color_scheme(const std::function<void()> &callback)
{
#ifndef Q_OS_LINUX
	(void)callback;
#else
	/*
	 * Qt has no signal to offer here: Qt 5 has none at all, and the one
	 * Qt 6 has only fires where its platform theme follows the desktop,
	 * which is not everywhere. So the portal is asked again every couple
	 * of seconds and the callback runs when the answer changes. Asking a
	 * desktop for a single number is small enough to do that often.
	 */
	QTimer *timer = new QTimer(QCoreApplication::instance());
	timer->setInterval(2000);

	DesktopColorScheme last = desktop_color_scheme();
	QObject::connect(timer, &QTimer::timeout, [callback, last]() mutable {
		const DesktopColorScheme now = desktop_color_scheme();
		if (now == DesktopColorScheme::Unknown || now == last)
			return;

		last = now;
		callback();
	});

	timer->start();
#endif
}

#endif

} // namespace util
} // namespace pv
