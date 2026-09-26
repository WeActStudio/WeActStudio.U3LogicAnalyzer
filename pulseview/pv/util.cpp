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
#include <unistd.h>
#endif

#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>
#include <QDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

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
	show_message(parent, title, text, QMessageBox::standardIcon(icon),
		min_width, rich_text, open_links, modal);
}

#ifdef __linux__

namespace {

const char *kUdevRulesPath = "/etc/udev/rules.d/99-weact-ch32h417.rules";

/* Kept here so no extra file has to be shipped next to the binary. */
const char *kUdevRulesText =
	"# WeAct Studio CH32H417 Logic Analyzer - udev access rules\n"
	"# 5537: runtime interfaces (command / logic data / ADC data)\n"
	"# fe17: HID interface used for the IAP firmware upgrade\n"
	"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"5537\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n"
	"SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"fe17\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n"
	"SUBSYSTEM==\"hidraw\", ATTRS{idVendor}==\"1a86\", ATTRS{idProduct}==\"fe17\", MODE=\"0666\", GROUP=\"plugdev\", TAG+=\"uaccess\"\n";

} // namespace

bool udev_rules_installed()
{
	return access(kUdevRulesPath, F_OK) == 0;
}

bool install_udev_rules(const std::string &password, std::string &error_out)
{
	error_out.clear();

	/* Stage the file in /tmp first: sudo only copies it into place. */
	char tmp_path[] = "/tmp/ch32h417-udev-XXXXXX";
	int fd = mkstemp(tmp_path);
	if (fd < 0) {
		error_out = "Failed to create a temporary file";
		return false;
	}
	FILE *f = fdopen(fd, "w");
	if (!f) {
		close(fd);
		unlink(tmp_path);
		error_out = "Failed to write the temporary rules file";
		return false;
	}
	fputs(kUdevRulesText, f);
	fclose(f);

	const std::string err_path = std::string(tmp_path) + ".err";
	std::string cmd = "sudo -S -p '' sh -c '";
	cmd += "cp " + std::string(tmp_path) + " " + kUdevRulesPath;
	cmd += std::string(" && chmod 0644 ") + kUdevRulesPath;
	cmd += " && (udevadm control --reload-rules || true)";
	cmd += " && (udevadm trigger --subsystem-match=usb || true)";
	cmd += "' 2>" + err_path;

	FILE *p = popen(cmd.c_str(), "w");
	if (!p) {
		unlink(tmp_path);
		error_out = "Failed to run sudo";
		return false;
	}
	/* The password travels on stdin only, it never hits a command line. */
	if (!password.empty())
		fwrite(password.data(), 1, password.size(), p);
	fputc('\n', p);
	const int rc = pclose(p);
	unlink(tmp_path);

	if (rc != 0) {
		if (FILE *ef = fopen(err_path.c_str(), "r")) {
			char line[512];
			bool any = false;
			while (fgets(line, sizeof(line), ef)) {
				error_out += line;
				any = true;
			}
			fclose(ef);
			if (!any)
				error_out = "sudo rejected the password, or sudo is not available";
		} else {
			error_out = "sudo rejected the password, or sudo is not available";
		}
		unlink(err_path.c_str());
		return false;
	}
	unlink(err_path.c_str());
	return true;
}

#else

bool udev_rules_installed()
{
	return true;
}

bool install_udev_rules(const std::string &password, std::string &error_out)
{
	(void)password;
	error_out = "udev rules only apply to Linux";
	return false;
}

#endif

} // namespace util
} // namespace pv
