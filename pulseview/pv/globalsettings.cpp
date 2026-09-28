/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2017 Soeren Apel <soeren@apelpie.net>
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

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QEvent>
#include <QFontMetrics>
#include <QPixmapCache>
#include <QString>
#include <QStyle>
#include <QStyleFactory>
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QStyleHints>
#endif
#include <QWidget>
#include <QtGlobal>

#include "globalsettings.hpp"
#include "application.hpp"
#include "fluentui3style.h"

using std::map;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;

/*
 * The FluentUI3 style keeps the icon font it draws a ticked check box
 * with, and two images, in a Qt resource. A resource that sits in a
 * static library does not register itself: the object file that would do
 * it is only linked in when something asks for it, and nothing does as
 * long as the library is only linked against. Asking for it here pulls
 * that object in and registers the resource (see Q_INIT_RESOURCE).
 *
 * It has to be done outside the namespace: Q_INIT_RESOURCE declares the
 * function it calls, and a declaration inside the namespace would ask
 * for one that no object file carries.
 */
static void register_fluentui_resources()
{
	static bool registered = false;

	if (!registered) {
		Q_INIT_RESOURCE(resource);
		registered = true;
	}
}

namespace pv {

const vector<QString> Themes {
	"Follow system", "Light", "Dark"
};

const QString Style_Fusion = "fusion";

/*
 * Not a name the style factory knows either: it stands for the
 * FluentUI3 style that is built into the application (see
 * 3rdparty/fluentui3style). It is what the "FluentUI3" entry of the
 * setting asks for, so that the entry is there -- and looks the same
 * -- on every Qt version and not only on those that carry the style
 * of that platform themselves. The entry is named after the style
 * rather than after the desktop it resembles, which MIT does not ask
 * anyone to give up but which a third-party style may not claim.
 */
const QString Style_Fluent = "fluentui3";

namespace {

/*
 * Tells from the palette Qt ended up with whether the UI reads as dark.
 * Only the "follow system" theme needs this: there the platform decides,
 * and Qt reports its choice through the application palette (Qt 6 on
 * Windows follows the system color scheme, Qt 5 does not).
 */
bool palette_is_dark(const QPalette &palette)
{
	return palette.color(QPalette::Window).lightness() < 128;
}

/*
 * Whether the desktop asks for a dark color scheme.
 *
 * The answer has to be read from the desktop itself: the palette Qt
 * hands out is not a reliable stand-in for it, because some builds of
 * Qt (the static Qt 5 this is built against, for one) return a light
 * palette however the desktop is set up, and Qt 6 only follows the
 * desktop where its platform theme has such a thing, which is not the
 * case on Linux. That made "follow the system" come out light on a dark
 * desktop. Only if the desktop cannot be asked, as on an old Windows,
 * the palette is used as a fallback.
 */
bool system_prefers_dark(const QPalette &startup_palette)
{
#ifdef _WIN32
	QSettings personalize(
		QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows"
			"\\CurrentVersion\\Themes\\Personalize"),
		QSettings::NativeFormat);
	const QVariant apps_use_light = personalize.value("AppsUseLightTheme");
	if (apps_use_light.isValid())
		return apps_use_light.toInt() == 0;
#else
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
	/*
	 * Qt 6 answers "unknown" where its platform theme does not follow
	 * the desktop, and that is not the same as a light desktop, so only
	 * an answer either way is taken.
	 */
	const Qt::ColorScheme scheme =
		QGuiApplication::styleHints()->colorScheme();
	if (scheme == Qt::ColorScheme::Dark)
		return true;
	if (scheme == Qt::ColorScheme::Light)
		return false;
#endif
	/* Every current Linux desktop says through the freedesktop portal. */
	switch (util::desktop_color_scheme()) {
	case util::DesktopColorScheme::Dark:
		return true;
	case util::DesktopColorScheme::Light:
		return false;
	case util::DesktopColorScheme::Unknown:
		break;
	}
#endif

	return palette_is_dark(startup_palette);
}

/*
 * The palette of the light theme. It is built here instead of being taken
 * from the style because the standard palette of a style follows the
 * system color scheme on Windows: on a machine that is set to the dark
 * scheme it comes out dark, which is what used to make the light theme
 * look dark. Every role is filled in so that nothing is left over from
 * the palette that was in use before.
 */
QPalette make_light_palette()
{
	QPalette p;

	p.setColor(QPalette::Window, QColor(0xf0, 0xf0, 0xf0));
	p.setColor(QPalette::WindowText, Qt::black);
	p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x7f, 0x7f, 0x7f));
	p.setColor(QPalette::Base, Qt::white);
	p.setColor(QPalette::AlternateBase, QColor(0xf7, 0xf7, 0xf7));
	p.setColor(QPalette::ToolTipBase, QColor(0xff, 0xff, 0xdc));
	p.setColor(QPalette::ToolTipText, Qt::black);
	p.setColor(QPalette::Text, Qt::black);
	p.setColor(QPalette::Disabled, QPalette::Text, QColor(0x7f, 0x7f, 0x7f));
	p.setColor(QPalette::Button, QColor(0xf0, 0xf0, 0xf0));
	p.setColor(QPalette::ButtonText, Qt::black);
	p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x7f, 0x7f, 0x7f));
	p.setColor(QPalette::BrightText, Qt::red);
	p.setColor(QPalette::Light, Qt::white);
	p.setColor(QPalette::Midlight, QColor(0xe3, 0xe3, 0xe3));
	p.setColor(QPalette::Mid, QColor(0xc0, 0xc0, 0xc0));
	p.setColor(QPalette::Dark, QColor(0x9f, 0x9f, 0x9f));
	p.setColor(QPalette::Shadow, QColor(0x69, 0x69, 0x69));
	p.setColor(QPalette::Link, QColor(0x00, 0x00, 0xee));
	p.setColor(QPalette::Highlight, QColor(0x30, 0x82, 0xda));
	p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0xc0, 0xc0, 0xc0));
	p.setColor(QPalette::HighlightedText, Qt::white);
	p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(0x7f, 0x7f, 0x7f));

	return p;
}

/*
 * The palette of the dark theme. Every role is filled in because a
 * QPalette that is only partly set keeps the light defaults for the rest,
 * which then shows up as light gray shading on a dark UI.
 */
QPalette make_dark_palette()
{
	QPalette p;

	p.setColor(QPalette::Window, QColor(0x26, 0x26, 0x26));
	p.setColor(QPalette::WindowText, Qt::white);
	p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
	p.setColor(QPalette::Base, QColor(0x2a, 0x2a, 0x2a));
	p.setColor(QPalette::AlternateBase, QColor(0x42, 0x42, 0x42));
	p.setColor(QPalette::ToolTipBase, QColor(0x35, 0x35, 0x35));
	p.setColor(QPalette::ToolTipText, Qt::white);
	p.setColor(QPalette::Text, Qt::white);
	p.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
	p.setColor(QPalette::Button, QColor(0x35, 0x35, 0x35));
	p.setColor(QPalette::ButtonText, Qt::white);
	p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
	p.setColor(QPalette::BrightText, Qt::red);
	p.setColor(QPalette::Light, QColor(0x4a, 0x4a, 0x4a));
	p.setColor(QPalette::Midlight, QColor(0x3a, 0x3a, 0x3a));
	p.setColor(QPalette::Mid, QColor(0x2b, 0x2b, 0x2b));
	p.setColor(QPalette::Dark, QColor(0x1e, 0x1e, 0x1e));
	p.setColor(QPalette::Shadow, QColor(0x14, 0x14, 0x14));
	p.setColor(QPalette::Link, QColor(42, 130, 218));
	p.setColor(QPalette::Highlight, QColor(42, 130, 218));
	p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
	p.setColor(QPalette::HighlightedText, Qt::white);
	p.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

	return p;
}

} // namespace

const QString GlobalSettings::Key_General_Language = "General_Language";
const QString GlobalSettings::Key_General_Theme = "General_Theme";
/* Set once the theme value has been moved over to the UiTheme scheme. */
const QString GlobalSettings::Key_General_ThemeScheme = "General_ThemeScheme";
const QString GlobalSettings::Key_General_Style = "General_Style";
const QString GlobalSettings::Key_General_SaveWithSetup = "General_SaveWithSetup";
const QString GlobalSettings::Key_General_StartAllSessions = "General_StartAllSessions";
const QString GlobalSettings::Key_View_ZoomToFitDuringAcq = "View_ZoomToFitDuringAcq";
const QString GlobalSettings::Key_View_ZoomToFitAfterAcq = "View_ZoomToFitAfterAcq";
const QString GlobalSettings::Key_View_TriggerIsZeroTime = "View_TriggerIsZeroTime";
const QString GlobalSettings::Key_View_ColoredBG = "View_ColoredBG";
const QString GlobalSettings::Key_View_StickyScrolling = "View_StickyScrolling";
const QString GlobalSettings::Key_View_AllowVerticalDragging = "View_AllowVerticalDragging";
const QString GlobalSettings::Key_View_ShowSamplingPoints = "View_ShowSamplingPoints";
const QString GlobalSettings::Key_View_FillSignalHighAreas = "View_FillSignalHighAreas";
const QString GlobalSettings::Key_View_FillSignalHighAreaColor = "View_FillSignalHighAreaColor";
const QString GlobalSettings::Key_View_ShowAnalogMinorGrid = "View_ShowAnalogMinorGrid";
const QString GlobalSettings::Key_View_ConversionThresholdDispMode = "View_ConversionThresholdDispMode";
const QString GlobalSettings::Key_View_DefaultDivHeight = "View_DefaultDivHeight";
const QString GlobalSettings::Key_View_DefaultLogicHeight = "View_DefaultLogicHeight";
const QString GlobalSettings::Key_View_ShowHoverMarker = "View_ShowHoverMarker";
const QString GlobalSettings::Key_View_SnapDistance = "View_SnapDistance";
const QString GlobalSettings::Key_View_CursorFillColor = "View_CursorFillColor";
const QString GlobalSettings::Key_View_CursorShowFrequency = "View_CursorShowFrequency";
const QString GlobalSettings::Key_View_CursorShowInterval = "View_CursorShowInterval";
const QString GlobalSettings::Key_View_CursorShowSamples = "View_CursorShowSamples";
const QString GlobalSettings::Key_Dec_InitialStateConfigurable = "Dec_InitialStateConfigurable";
const QString GlobalSettings::Key_Dec_ExportFormat = "Dec_ExportFormat";
const QString GlobalSettings::Key_Dec_AlwaysShowAllRows = "Dec_AlwaysShowAllRows";
const QString GlobalSettings::Key_Log_BufferSize = "Log_BufferSize";
const QString GlobalSettings::Key_Log_NotifyOfStacktrace = "Log_NotifyOfStacktrace";
const QString GlobalSettings::Key_ThresholdLevel = "ThresholdLevel";
vector<GlobalSettingsInterface*> GlobalSettings::callbacks_;
bool GlobalSettings::tracking_ = false;
bool GlobalSettings::is_dark_theme_ = false;
map<QString, QVariant> GlobalSettings::tracked_changes_;
QPalette GlobalSettings::default_palette_;

GlobalSettings::GlobalSettings() :
	QSettings()
{
	beginGroup("Settings");
}

void GlobalSettings::save_internal_defaults()
{
	/*
	 * The palette Qt reports at startup is the one the platform asked
	 * for - on Qt 5 it is the only hint at the color scheme of the
	 * desktop there is.
	 */
	default_palette_ = QApplication::palette();
}

void GlobalSettings::set_defaults_where_needed()
{
	if (!contains(Key_General_Language)) {
		QString langTag = QLocale().uiLanguages().first(); // "es-MX", "zh-CN", "de-DE"
		QStringList parts = langTag.split('-');
		QString lang;
		if (parts.size() >=2 && parts[1].length() ==2) {
			// language-region, drop the script subtag
			lang = parts[0] + "_" + parts[1];
		} else {
			lang = parts[0];
		}
		setValue(Key_General_Language, lang);
		apply_language();
	}

	// Use no theme by default
	/*
	 * The theme setting used to be an index into a list of style sheets
	 * (0 = bright, 1 = qdarkstyle) that no longer exists. Move the value
	 * over to UiTheme so that a user who picked "bright" keeps a light UI
	 * and one who picked "qdarkstyle" keeps a dark one; a fresh install
	 * starts out following the system.
	 */
	if (!contains(Key_General_ThemeScheme)) {
		if (!contains(Key_General_Theme))
			setValue(Key_General_Theme, Theme_System);
		else
			setValue(Key_General_Theme,
				(value(Key_General_Theme).toInt() == 0) ? Theme_Light : Theme_Dark);

		setValue(Key_General_ThemeScheme, 1);
	}

	/*
	 * Start out with the style the palettes of the themes are made for.
	 * Every value that is not one of the two on offer counts as missing:
	 * an empty one is what a run that was interrupted between two settings
	 * writes leaves behind, and the others are entries that are no longer
	 * offered, which is what the settings of an older build carry.
	 */
	const QString style_value =
		value(Key_General_Style).toString().toLower();

	if (style_value != Style_Fluent && style_value != Style_Fusion)
		setValue(Key_General_Style, Style_Fusion);

	// Save setup with .sr files by default
	if (!contains(Key_General_SaveWithSetup))
		setValue(Key_General_SaveWithSetup, true);

	// Enable zoom-to-fit after acquisition by default
	if (!contains(Key_View_ZoomToFitAfterAcq))
		setValue(Key_View_ZoomToFitAfterAcq, true);

	// Allow vertical dragging by default
	if (!contains(Key_View_AllowVerticalDragging))
		setValue(Key_View_AllowVerticalDragging, true);

	// Enable colored trace backgrounds by default
	if (!contains(Key_View_ColoredBG))
		setValue(Key_View_ColoredBG, false);

	// Enable showing sampling points by default
	if (!contains(Key_View_ShowSamplingPoints))
		setValue(Key_View_ShowSamplingPoints, false);

	// Enable filling logic signal high areas by default
	if (!contains(Key_View_FillSignalHighAreas))
		setValue(Key_View_FillSignalHighAreas, false);

	if (!contains(Key_View_DefaultDivHeight))
		setValue(Key_View_DefaultDivHeight,
		3 * QFontMetrics(QApplication::font()).height());

	if (!contains(Key_View_DefaultLogicHeight))
		setValue(Key_View_DefaultLogicHeight, 25);

	if (!contains(Key_View_ShowHoverMarker))
		setValue(Key_View_ShowHoverMarker, false);

	if (!contains(Key_View_SnapDistance))
		setValue(Key_View_SnapDistance, 1000);

	if (!contains(Key_View_CursorShowInterval))
		setValue(Key_View_CursorShowInterval, true);

	if (!contains(Key_View_CursorShowFrequency))
		setValue(Key_View_CursorShowFrequency, true);

	// %c was used for the row name in the past so we need to transition such users
	if (!contains(Key_Dec_ExportFormat) ||
		value(Key_Dec_ExportFormat).toString() == "%s %d: %c: %1")
		setValue(Key_Dec_ExportFormat, "%s %d: %r: %1");

	// Default to 500 lines of backlog
	if (!contains(Key_Log_BufferSize))
		setValue(Key_Log_BufferSize, 500);

	// Notify user of existing stack trace by default
	if (!contains(Key_Log_NotifyOfStacktrace))
		setValue(Key_Log_NotifyOfStacktrace, true);

	// Default theme is bright, so use its color scheme if undefined
	if (!contains(Key_View_CursorFillColor)){
		if (value(Key_General_Theme).toInt() == Theme_Dark)
			set_dark_theme_default_colors();
		else
			set_bright_theme_default_colors();
	}
	if (!contains(Key_ThresholdLevel))
		setValue(Key_ThresholdLevel, 2.3);

	// setValue(Key_View_ShowSamplingPoints, false);   //force 
	// setValue(Key_View_ShowHoverMarker, false);      //force 
}

void GlobalSettings::set_bright_theme_default_colors()
{
	setValue(Key_View_FillSignalHighAreaColor,
		QColor(0, 0, 0, 5 * 256 / 100).rgba());
	setValue(Key_View_CursorFillColor,
		QColor(220, 231, 243).rgba());	
}

void GlobalSettings::set_dark_theme_default_colors()
{
	setValue(Key_View_FillSignalHighAreaColor,
		QColor(0, 0, 0, 5 * 256 / 100).rgba());
	setValue(Key_View_CursorFillColor,
		QColor(98, 98, 49).rgba());
}

bool GlobalSettings::current_theme_is_dark()
{
	return is_dark_theme_;
}

void GlobalSettings::apply_theme()
{
	const int theme = qBound(0, value(Key_General_Theme).toInt(),
		(int)Themes.size() - 1);

	/*
	 * The themes used to each carry a style sheet (qdarkstyle); those are
	 * gone, so the application itself never carries one. The appearance
	 * now comes from the widget style plus the palette alone.
	 */
	qApp->setStyleSheet("");

	/* Light and dark both come from palettes built here, so that the UI
	 * looks the same on every desktop and stays independent of whatever
	 * the style would pick for the color scheme of the system. */
	const bool dark = (theme == Theme_System) ?
		system_prefers_dark(default_palette_) : (theme == Theme_Dark);

	/*
	 * The FluentUI3 style reads its color scheme from this property, the
	 * same one the style of that platform reads in the Qt versions that
	 * carry it: 0 is light and 1 is dark. It has to be set before the
	 * style is, because a style hands out the palette that goes with the
	 * scheme it settled on while it is being installed.
	 */
	qApp->setProperty("_q_colorscheme", dark ? 1 : 0);

	/*
	 * The palettes are made for Fusion, and Fusion is the style that
	 * paints with them. The Windows style paints with the palette as well
	 * -- it is the style of that platform written in Qt, not the native
	 * one, which would follow the system colors and ignore the palette --
	 * so it can be offered as a choice. The style has to be set before the
	 * palette, because setStyle() installs the standard palette of the
	 * new style.
	 *
	 * The FluentUI3 style is the one exception: it paints with a palette
	 * of its own, which it puts in place of whatever palette it is given,
	 * and the colors of Fluent are part of the look it is chosen for. It
	 * takes its two palettes from the property above, so it still follows
	 * the light and dark of this setting.
	 */
	const QString style_name = value(Key_General_Style).toString();
	QStyle *base_style = nullptr;

	if (style_name == Style_Fluent) {
		register_fluentui_resources();
		base_style = new FluentUI3Style;
	} else if (!style_name.isEmpty())
		base_style = QStyleFactory::create(style_name);

	if (!base_style)
		base_style = QStyleFactory::create(Style_Fusion);
	if (base_style)
		qApp->setStyle(base_style);

	qApp->setPalette(dark ? make_dark_palette() : make_light_palette());

	is_dark_theme_ = dark;

	qDebug() << "Theme:" << Themes.at(theme) << "style:" <<
		qApp->style()->objectName() << "window:" <<
		qApp->palette().color(QPalette::Window).name() <<
		"dark:" << is_dark_theme_;

	QPixmapCache::clear();

	/*
	 * QApplication::setPalette() notifies the top level windows and lets
	 * the widgets below them follow through the palette they inherit from
	 * their parent. A widget that carries a style sheet is left out of
	 * that propagation, so it would keep the colors of the previous
	 * theme. Tell every widget about the new palette instead of relying
	 * on the propagation.
	 */
	QEvent palette_event(QEvent::ApplicationPaletteChange);
	const QWidgetList widgets = QApplication::allWidgets();
	for (QWidget *widget : widgets)
		QCoreApplication::sendEvent(widget, &palette_event);

	/*
	 * A widget that was asked for its size remembers it, and it forgets it
	 * when the change of style reaches it. It does not reach all of them:
	 * after a switch to a style whose rows are taller, some of the rows of
	 * a page keep the height of the previous style, and the height of a
	 * page is what the settings dialog is sized by. Send the change to
	 * every widget, the same way the palette is sent, so that every row
	 * reports the height of the style that is in use.
	 */
	QEvent style_event(QEvent::StyleChange);
	for (QWidget *widget : widgets)
		QCoreApplication::sendEvent(widget, &style_event);

	/*
	 * The palette is propagated by the style, and a widget that carries a
	 * style sheet is left out of that propagation: an empty style sheet
	 * discards the palette pass instead of passing it on, so the widget
	 * keeps the colors it was given at the start, and the copy it was
	 * given at the start is the one the style made in the scheme that was
	 * in use back then. Emptying the style sheet again is what puts the
	 * palette back in the hands of the style and lets it reach those
	 * widgets. The rule is only handed out when the sheet is empty to
	 * begin with, so a non-empty one is not disturbed, and a sheet is
	 * needed to disable the discarded one in the first place.
	 */
	const QByteArray sheet = qApp->styleSheet().toUtf8();
	if (!sheet.isEmpty()) {
		qApp->setStyleSheet(QString());
		qApp->setStyleSheet(QString::fromUtf8(sheet));
	}

	/*
	 * Widgets that draw with theme dependent colors of their own pick them
	 * up when the setting is announced, so the announcement has to happen
	 * once the palette and is_dark_theme_ are up to date.
	 */
	for (GlobalSettingsInterface *cb : callbacks_)
		cb->on_setting_changed(Key_General_Theme, value(Key_General_Theme));

	/*
	 * The window decorations are not part of the palette and are set
	 * per window, so every window that is already open has to be told
	 * about the new theme; leaving that to the next QEvent::Show would
	 * only reach the ones that have not been shown yet.
	 */
	for (QWidget *w : QApplication::topLevelWidgets())
		util::apply_window_theme(w, is_dark_theme_);
}

void GlobalSettings::apply_language()
{
	Application* a = qobject_cast<Application*>(QApplication::instance());
	a->switch_language(value(Key_General_Language).toString());
}

void GlobalSettings::add_change_handler(GlobalSettingsInterface *cb)
{
	callbacks_.push_back(cb);
}

void GlobalSettings::remove_change_handler(GlobalSettingsInterface *cb)
{
	for (auto cb_it = callbacks_.begin(); cb_it != callbacks_.end(); cb_it++)
		if (*cb_it == cb) {
			callbacks_.erase(cb_it);
			break;
		}
}

void GlobalSettings::setValue(const QString &key, const QVariant &value)
{
	// Save previous value if we're tracking changes,
	// not altering an already-existing saved setting
	if (tracking_)
		tracked_changes_.emplace(key, QSettings::value(key));

	QSettings::setValue(key, value);

	// TODO Emulate noquote()
	qDebug() << "Setting" << key << "changed to" << value;

	// Call all registered callbacks
	for (GlobalSettingsInterface *cb : callbacks_)
		cb->on_setting_changed(key, value);
}

void GlobalSettings::start_tracking()
{
	tracking_ = true;
	tracked_changes_.clear();
}

void GlobalSettings::stop_tracking()
{
	tracking_ = false;
	tracked_changes_.clear();
}

void GlobalSettings::undo_tracked_changes()
{
	tracking_ = false;

	bool appearance_reverted = false;

	for (auto& entry : tracked_changes_) {
		if (entry.first == Key_General_Theme ||
			entry.first == Key_General_Style)
			appearance_reverted = true;
		setValue(entry.first, entry.second);
	}

	tracked_changes_.clear();

	/*
	 * The theme and the style take effect as soon as they are picked, so
	 * putting the settings back has to put the appearance back with
	 * them. Restoring only the values would leave the palette on the
	 * theme the user just undid while the widgets that read the settings
	 * move on - the traces end up drawn in the colors of the theme that
	 * is not displayed.
	 */
	if (appearance_reverted)
		apply_theme();
}

void GlobalSettings::store_gvariant(QSettings &settings, GVariant *v)
{
	const GVariantType *var_type = g_variant_get_type(v);
	char *var_type_str = g_variant_type_dup_string(var_type);

	QByteArray var_data = QByteArray((const char*)g_variant_get_data(v),
		g_variant_get_size(v));

	settings.setValue("value", var_data);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	settings.setValue("type", (const char *)var_type_str);
#else
	settings.setValue("type", var_type_str);
#endif

	g_free(var_type_str);
}

GVariant* GlobalSettings::restore_gvariant(QSettings &settings)
{
	QString raw_type = settings.value("type").toString();
	GVariantType *var_type = g_variant_type_new(raw_type.toUtf8());

	QByteArray data = settings.value("value").toByteArray();

#if GLIB_CHECK_VERSION(2, 67, 3)  // See https://discourse.gnome.org/t/port-your-module-from-g-memdup-to-g-memdup2-now/5538
	gpointer var_data = g_memdup2((gconstpointer)data.constData(), (gsize)data.size());
#else
	gpointer var_data = g_memdup((gconstpointer)data.constData(), (guint)data.size());
#endif

	GVariant *value = g_variant_new_from_data(var_type, var_data,
		data.size(), false, g_free, var_data);

	g_variant_type_free(var_type);

	return value;
}

void GlobalSettings::store_variantbase(QSettings &settings, Glib::VariantBase v)
{
	const QByteArray var_data = QByteArray((const char*)v.get_data(), v.get_size());

	settings.setValue("value", var_data);
	settings.setValue("type", QString::fromStdString(v.get_type_string()));
}

Glib::VariantBase GlobalSettings::restore_variantbase(QSettings &settings)
{
	QString raw_type = settings.value("type").toString();
	GVariantType *var_type = g_variant_type_new(raw_type.toUtf8());

	QByteArray data = settings.value("value").toByteArray();

#if GLIB_CHECK_VERSION(2, 67, 3)  // See https://discourse.gnome.org/t/port-your-module-from-g-memdup-to-g-memdup2-now/5538
	gpointer var_data = g_memdup2((gconstpointer)data.constData(), (gsize)data.size());
#else
	gpointer var_data = g_memdup((gconstpointer)data.constData(), (guint)data.size());
#endif

	GVariant *value = g_variant_new_from_data(var_type, var_data,
		data.size(), false, g_free, var_data);

	Glib::VariantBase ret_val = Glib::VariantBase(value, true);

	g_variant_type_free(var_type);
	g_variant_unref(value);

	return ret_val;
}

void GlobalSettings::store_timestamp(QSettings &settings, const char *name, const pv::util::Timestamp &ts)
{
	stringstream ss;
	boost::archive::text_oarchive oa(ss);
	oa << boost::serialization::make_nvp(name, ts);
	settings.setValue(name, QString::fromStdString(ss.str()));
}

pv::util::Timestamp GlobalSettings::restore_timestamp(QSettings &settings, const char *name)
{
	util::Timestamp result;
	stringstream ss;
	ss << settings.value(name).toString().toStdString();

	try {
		boost::archive::text_iarchive ia(ss);
		ia >> boost::serialization::make_nvp(name, result);
	} catch (boost::archive::archive_exception&) {
		qDebug() << "Could not restore setting" << name;
	}

	return result;
}

} // namespace pv
