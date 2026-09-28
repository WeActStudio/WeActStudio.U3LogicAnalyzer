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

#include "config.h"

#include <glib.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

#include "settings.hpp"

#include "pv/application.hpp"
#include "pv/devicemanager.hpp"
#include "pv/globalsettings.hpp"
#include "pv/logging.hpp"
#include "pv/widgets/colorbutton.hpp"

#include <libsigrokcxx/libsigrokcxx.hpp>


#ifdef ENABLE_DECODE
#include <libsigrokdecode/libsigrokdecode.h>
#endif

using pv::widgets::ColorButton;

namespace pv {
namespace dialogs {

/**
 * Special version of a QListView that has the width of the first column as minimum size.
 *
 * @note Inspired by https://github.com/qt-creator/qt-creator/blob/master/src/plugins/coreplugin/dialogs/settingsdialog.cpp
 */
class PageListWidget: public QListWidget
{
public:
	PageListWidget() :
		QListWidget()
	{
		setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	}

	QSize sizeHint() const final
	{
		int width = sizeHintForColumn(0) + frameWidth() * 2 + 5;
		if (verticalScrollBar()->isVisible())
			width += verticalScrollBar()->width();
		return QSize(width, 100);
	}
};

Settings::Settings(DeviceManager &device_manager, QWidget *parent) :
	QDialog(parent),
	device_manager_(device_manager),
	fit_passes_(0)
{
	resize(600, 400);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// Create log view
	log_view_ = create_log_view();

	// Create pages
	page_list = new PageListWidget();
	page_list->setViewMode(QListView::ListMode);
	page_list->setMovement(QListView::Static);
	page_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	pages = new QStackedWidget;
	create_pages();
	page_list->setCurrentIndex(page_list->model()->index(0, 0));

	/*
	 * The pages are shown through a scroll area, so that the height
	 * they ask for does not decide the height of the dialog. The two
	 * styles the application is made for lay a row out at very
	 * different heights -- a check box asks for 15 pixels under Fusion
	 * and for 34 under FluentUI3, which over the rows of the views page
	 * is 230 pixels -- so a dialog that is sized to whichever style is
	 * in use would grow when the style is changed from inside it, by
	 * the height of a page that is not even on screen, and would keep
	 * the height it grew to. The pages scroll instead.
	 */
	page_area = new QScrollArea();
	page_area->setWidget(pages);
	page_area->setWidgetResizable(true);
	page_area->setFrameShape(QFrame::NoFrame);
	page_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Create the rest of the dialog
	QHBoxLayout *tab_layout = new QHBoxLayout;
	tab_layout->addWidget(page_list);
	tab_layout->addWidget(page_area, Qt::AlignLeft);

	QDialogButtonBox *button_box = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	QVBoxLayout* root_layout = new QVBoxLayout(this);
	root_layout->addLayout(tab_layout);
	root_layout->addWidget(button_box);

	/*
	 * Open with the width and the height of the pages. A scroll area
	 * reports a height of its own rather than the height of what it
	 * shows, so the pages are asked for their size to give the dialog
	 * the size it would come out with on its own.
	 *
	 * The height is let go of again as soon as the dialog has been
	 * sized (see showEvent), so that a style that asks for more room
	 * than the dialog has leaves the pages scrolling instead of pushing
	 * the dialog taller. The width is kept: it comes from the page
	 * list, the frames and the log view rather than from the height of
	 * a row, and a page narrower than its own width would clip.
	 */
	page_area->setMinimumSize(pages->sizeHint());

	connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(page_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
		this, SLOT(on_page_changed(QListWidgetItem*, QListWidgetItem*)));

	// Start to record changes
	GlobalSettings settings;
	settings.start_tracking();
}

void Settings::changeEvent(QEvent *event)
{
	if (event->type() != QEvent::LanguageChange) {
		QDialog::changeEvent(event);
		return;
	}

	/* Every page is built once and keeps the texts of the language
	 * that was active when it was built, so build them again. The
	 * settings themselves are applied as they are changed, so
	 * nothing is lost by throwing the widgets away. */
	const int page = page_list->currentRow();

	GlobalSettings settings;
	settings.stop_tracking();

	while (pages->count()) {
		QWidget *old_page = pages->widget(0);
		pages->removeWidget(old_page);
		old_page->deleteLater();
	}
	page_list->clear();

	create_pages();

	if (page >= 0 && page < page_list->count())
		page_list->setCurrentRow(page);

	settings.start_tracking();

	QDialog::changeEvent(event);
}

void Settings::create_pages()
{
	// General page
	pages->addWidget(get_general_settings_form(pages));

	QListWidgetItem *generalButton = new QListWidgetItem(page_list);
	generalButton->setIcon(QIcon(":/icons/settings-general.png"));
	generalButton->setText(tr("General"));
	generalButton->setTextAlignment(Qt::AlignVCenter);
	generalButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	// View page
	pages->addWidget(get_view_settings_form(pages));

	QListWidgetItem *viewButton = new QListWidgetItem(page_list);
	viewButton->setIcon(QIcon(":/icons/settings-views.svg"));
	viewButton->setText(tr("Views"));
	viewButton->setTextAlignment(Qt::AlignVCenter);
	viewButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

#ifdef ENABLE_DECODE
	// Decoder page
	pages->addWidget(get_decoder_settings_form(pages));

	QListWidgetItem *decoderButton = new QListWidgetItem(page_list);
	decoderButton->setIcon(QIcon(":/icons/add-decoder.svg"));
	decoderButton->setText(tr("Decoders"));
	decoderButton->setTextAlignment(Qt::AlignVCenter);
	decoderButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
#endif

	// About page
	pages->addWidget(get_about_page(pages));

	QListWidgetItem *aboutButton = new QListWidgetItem(page_list);
	aboutButton->setIcon(QIcon(":/icons/information.svg"));
	aboutButton->setText(tr("About"));
	aboutButton->setTextAlignment(Qt::AlignVCenter);
	aboutButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	// Logging page
	pages->addWidget(get_logging_page(pages));

	QListWidgetItem *loggingButton = new QListWidgetItem(page_list);
	loggingButton->setIcon(QIcon(":/icons/information.svg"));
	loggingButton->setText(tr("Logging"));
	loggingButton->setTextAlignment(Qt::AlignVCenter);
	loggingButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

QCheckBox *Settings::create_checkbox(const QString& key, const char* slot) const
{
	GlobalSettings settings;

	QCheckBox *cb = new QCheckBox();
	cb->setChecked(settings.value(key).toBool());
	connect(cb, SIGNAL(stateChanged(int)), this, slot);
	return cb;
}

QPlainTextEdit *Settings::create_log_view() const
{
	GlobalSettings settings;

	QPlainTextEdit *log_view = new QPlainTextEdit();

	log_view->setReadOnly(true);
	log_view->setWordWrapMode(QTextOption::NoWrap);
	log_view->setCenterOnScroll(true);

	log_view->appendHtml(logging.get_log());
	connect(&logging, SIGNAL(logged_text(QString)),
		log_view, SLOT(appendHtml(QString)));

	return log_view;
}

QWidget *Settings::get_general_settings_form(QWidget *parent) const
{
	GlobalSettings settings;
	QCheckBox *cb;

	QWidget *form = new QWidget(parent);
	QVBoxLayout *form_layout = new QVBoxLayout(form);

	// General settings
	QGroupBox *general_group = new QGroupBox(tr("General"));
	form_layout->addWidget(general_group);

	QFormLayout *general_layout = new QFormLayout();
	general_group->setLayout(general_layout);

	// Generate language combobox
	QComboBox *language_cb = new QComboBox();
	Application* a = qobject_cast<Application*>(QApplication::instance());

	QString current_language = settings.value(GlobalSettings::Key_General_Language, "en").toString();
	for (const QString& language : a->get_languages()) {
		const QLocale locale = QLocale(language);
		const QString desc = locale.languageToString(locale.language());
		language_cb->addItem(desc, language);

		if (language == current_language) {
			int index = language_cb->findText(desc, Qt::MatchFixedString);
			language_cb->setCurrentIndex(index);
		}
	}
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	connect(language_cb, SIGNAL(currentTextChanged(const QString&)),
		this, SLOT(on_general_language_changed(const QString&)));
#else
	connect(language_cb, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(on_general_language_changed(const QString&)));
#endif
	general_layout->addRow(tr("User interface language"), language_cb);

	// Theme combobox
	QComboBox *theme_cb = new QComboBox();
	theme_cb->addItem(tr("Follow system"), Theme_System);
	theme_cb->addItem(tr("Light"), Theme_Light);
	theme_cb->addItem(tr("Dark"), Theme_Dark);

	theme_cb->setCurrentIndex(
		settings.value(GlobalSettings::Key_General_Theme).toInt());
	connect(theme_cb, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_general_theme_changed(int)));
	general_layout->addRow(tr("User interface theme"), theme_cb);

	// Style combobox
	/*
	 * The two styles the application is made for, and nothing else: the
	 * list does not depend on what the Qt build happens to carry. The
	 * names are not translated, and neither is the name of the first one:
	 * it is a third-party style under the MIT license rather than a look
	 * that belongs to any one version of Windows, so it is listed under
	 * the name it comes with.
	 *
	 * The styles the factory offers besides these are left out on purpose.
	 * windowsvista is drawn by the system instead of by Qt and keeps the
	 * colors of the system, so on the dark theme its controls come out
	 * white inside an otherwise dark window. windows11 is the look of the
	 * first entry but only the newer Qt versions carry it, so offering it
	 * would make the choice depend on the build. Windows is drawn by Qt
	 * and does follow the palette, but it is the look of a desktop two
	 * versions old: its sunken frames and square arrow buttons sit next
	 * to the rest of the UI as if they belonged to another application.
	 */
	QComboBox *style_cb = new QComboBox();
	style_cb->addItem(QStringLiteral("FluentUI3"), Style_Fluent);
	style_cb->addItem(QStringLiteral("Fusion"), Style_Fusion);

	int style_index = style_cb->findData(
		settings.value(GlobalSettings::Key_General_Style).toString().toLower());
	if (style_index < 0)
		style_index = style_cb->findData(Style_Fusion);
	style_cb->setCurrentIndex(style_index < 0 ? 0 : style_index);

	connect(style_cb, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_general_style_changed(int)));
	general_layout->addRow(tr("User interface style"), style_cb);

	// Misc
	cb = create_checkbox(GlobalSettings::Key_General_SaveWithSetup,
		SLOT(on_general_save_with_setup_changed(int)));
	general_layout->addRow(tr("Save session &setup along with .sr file"), cb);

	// cb = create_checkbox(GlobalSettings::Key_General_StartAllSessions,
	// 	SLOT(on_general_start_all_sessions_changed(int)));
	// general_layout->addRow(tr("Start acquisition for all open sessions when clicking 'Run'"), cb);


	return form;
}

QWidget *Settings::get_view_settings_form(QWidget *parent) const
{
	GlobalSettings settings;
	QCheckBox *cb;

	QWidget *form = new QWidget(parent);
	QVBoxLayout *form_layout = new QVBoxLayout(form);

	// Trace view settings
	QGroupBox *trace_view_group = new QGroupBox(tr("Trace View"));
	form_layout->addWidget(trace_view_group);

	QFormLayout *trace_view_layout = new QFormLayout();
	trace_view_group->setLayout(trace_view_layout);

	cb = create_checkbox(GlobalSettings::Key_View_ColoredBG,
		SLOT(on_view_coloredBG_changed(int)));
	trace_view_layout->addRow(tr("Use colored trace &background"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_ZoomToFitDuringAcq,
		SLOT(on_view_zoomToFitDuringAcq_changed(int)));
	trace_view_layout->addRow(tr("Constantly perform &zoom-to-fit during acquisition"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_ZoomToFitAfterAcq,
		SLOT(on_view_zoomToFitAfterAcq_changed(int)));
	trace_view_layout->addRow(tr("Perform a zoom-to-&fit when acquisition stops"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_TriggerIsZeroTime,
		SLOT(on_view_triggerIsZero_changed(int)));
	trace_view_layout->addRow(tr("Show time zero at the &trigger"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_StickyScrolling,
		SLOT(on_view_stickyScrolling_changed(int)));
	trace_view_layout->addRow(tr("Always keep &newest samples at the right edge during capture"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_AllowVerticalDragging,
		SLOT(on_view_allowVerticalDragging_changed(int)));
	trace_view_layout->addRow(tr("Allow &vertical dragging in the view area"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_ShowSamplingPoints,
		SLOT(on_view_showSamplingPoints_changed(int)));
	trace_view_layout->addRow(tr("Show data &sampling points"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_FillSignalHighAreas,
		SLOT(on_view_fillSignalHighAreas_changed(int)));
	trace_view_layout->addRow(tr("Fill &high areas of logic signals"), cb);

	ColorButton* high_fill_cb = new ColorButton(parent);
	high_fill_cb->set_color(QColor::fromRgba(
		settings.value(GlobalSettings::Key_View_FillSignalHighAreaColor).value<uint32_t>()));
	connect(high_fill_cb, SIGNAL(selected(QColor)),
		this, SLOT(on_view_fillSignalHighAreaColor_changed(QColor)));
	trace_view_layout->addRow(tr("Color to fill high areas of logic signals with"), high_fill_cb);

	cb = create_checkbox(GlobalSettings::Key_View_ShowAnalogMinorGrid,
		SLOT(on_view_showAnalogMinorGrid_changed(int)));
	trace_view_layout->addRow(tr("Show analog minor grid in addition to div grid"), cb);

	cb = create_checkbox(GlobalSettings::Key_View_ShowHoverMarker,
		SLOT(on_view_showHoverMarker_changed(int)));
	trace_view_layout->addRow(tr("Highlight mouse cursor using a vertical marker line"), cb);

	QSpinBox *snap_distance_sb = new QSpinBox();
	snap_distance_sb->setRange(0, 4000);
	snap_distance_sb->setSuffix(tr(" pixels"));
	snap_distance_sb->setValue(
		settings.value(GlobalSettings::Key_View_SnapDistance).toInt());
	connect(snap_distance_sb, SIGNAL(valueChanged(int)), this,
		SLOT(on_view_snapDistance_changed(int)));
	trace_view_layout->addRow(tr("Maximum distance from edges before markers snap to them"), snap_distance_sb);

	ColorButton* cursor_fill_cb = new ColorButton(parent);
	cursor_fill_cb->set_color(QColor::fromRgba(
		settings.value(GlobalSettings::Key_View_CursorFillColor).value<uint32_t>()));
	connect(cursor_fill_cb, SIGNAL(selected(QColor)),
		this, SLOT(on_view_cursorFillColor_changed(QColor)));
	trace_view_layout->addRow(tr("Color to fill cursor area with"), cursor_fill_cb);

	QComboBox *thr_disp_mode_cb = new QComboBox();
	thr_disp_mode_cb->addItem(tr("None"), GlobalSettings::ConvThrDispMode_None);
	thr_disp_mode_cb->addItem(tr("Background"), GlobalSettings::ConvThrDispMode_Background);
	thr_disp_mode_cb->addItem(tr("Dots"), GlobalSettings::ConvThrDispMode_Dots);
	thr_disp_mode_cb->setCurrentIndex(
		settings.value(GlobalSettings::Key_View_ConversionThresholdDispMode).toInt());
	connect(thr_disp_mode_cb, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_view_conversionThresholdDispMode_changed(int)));
	trace_view_layout->addRow(tr("Conversion threshold display mode (analog traces only)"), thr_disp_mode_cb);

	QSpinBox *default_div_height_sb = new QSpinBox();
	default_div_height_sb->setRange(20, 1000);
	default_div_height_sb->setSuffix(tr(" pixels"));
	default_div_height_sb->setValue(
		settings.value(GlobalSettings::Key_View_DefaultDivHeight).toInt());
	connect(default_div_height_sb, SIGNAL(valueChanged(int)), this,
		SLOT(on_view_defaultDivHeight_changed(int)));
	trace_view_layout->addRow(tr("Default analog trace div height"), default_div_height_sb);

	QSpinBox *default_logic_height_sb = new QSpinBox();
	default_logic_height_sb->setRange(5, 1000);
	default_logic_height_sb->setSuffix(tr(" pixels"));
	default_logic_height_sb->setValue(
		settings.value(GlobalSettings::Key_View_DefaultLogicHeight).toInt());
	connect(default_logic_height_sb, SIGNAL(valueChanged(int)), this,
		SLOT(on_view_defaultLogicHeight_changed(int)));
	trace_view_layout->addRow(tr("Default logic trace height"), default_logic_height_sb);

	return form;
}

QWidget *Settings::get_decoder_settings_form(QWidget *parent)
{
#ifdef ENABLE_DECODE
	GlobalSettings settings;
	QCheckBox *cb;

	QWidget *form = new QWidget(parent);
	QVBoxLayout *form_layout = new QVBoxLayout(form);

	// Decoder settings
	QGroupBox *decoder_group = new QGroupBox(tr("Decoders"));
	form_layout->addWidget(decoder_group);

	QFormLayout *decoder_layout = new QFormLayout();
	decoder_group->setLayout(decoder_layout);

	cb = create_checkbox(GlobalSettings::Key_Dec_InitialStateConfigurable,
		SLOT(on_dec_initialStateConfigurable_changed(int)));
	decoder_layout->addRow(tr("Allow configuration of &initial signal state"), cb);

	cb = create_checkbox(GlobalSettings::Key_Dec_AlwaysShowAllRows,
		SLOT(on_dec_alwaysshowallrows_changed(int)));
	decoder_layout->addRow(tr("Always show all &rows, even if no annotation is visible"), cb);

	// Annotation export settings
	ann_export_format_ = new QLineEdit();
	ann_export_format_->setText(
		settings.value(GlobalSettings::Key_Dec_ExportFormat).toString());
	connect(ann_export_format_, SIGNAL(textChanged(const QString&)),
		this, SLOT(on_dec_exportFormat_changed(const QString&)));
	decoder_layout->addRow(tr("Annotation export format"), ann_export_format_);
	QLabel *description_1 = new QLabel(tr("%s = sample range; %d: decoder name; %r: row name; %c: class name"));
	description_1->setAlignment(Qt::AlignRight);
	decoder_layout->addRow(description_1);
	QLabel *description_2 = new QLabel(tr("%1: longest annotation text; %a: all annotation texts; %q: use quotation marks"));
	description_2->setAlignment(Qt::AlignRight);
	decoder_layout->addRow(description_2);

	return form;
#else
	(void)parent;
	return nullptr;
#endif
}

QWidget *Settings::get_about_page(QWidget *parent) const
{
	Application* a = qobject_cast<Application*>(QApplication::instance());

	QLabel *icon = new QLabel();
	icon->setPixmap(QPixmap(QString::fromUtf8(":/icons/pulseview.svg")));

	// Setup the license field with the project homepage link
	QLabel *gpl_home_info = new QLabel();
	gpl_home_info->setText(tr("%1<br /><a href=\"http://%2\">%2</a>").arg(
		tr("GNU GPL, version 3 or later"),
		QApplication::organizationDomain()));
	gpl_home_info->setOpenExternalLinks(true);

	QString s;

	s.append("<style type=\"text/css\"> tr .id { white-space: pre; padding-right: 5px; } </style>");

	s.append("<table>");

	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Versions, libraries and features:") + "</b></td></tr>");
	for (pair<QString, QString> &entry : a->get_version_info())
		s.append(QString("<tr><td><i>%1</i></td><td>%2</td></tr>")
			.arg(entry.first, entry.second));

	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Firmware search paths:") + "</b></td></tr>");
	for (QString &entry : a->get_fw_path_list())
		s.append(QString("<tr><td colspan=\"2\">%1</td></tr>").arg(entry));

#ifdef ENABLE_DECODE
	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Protocol decoder search paths:") + "</b></td></tr>");
	for (QString &entry : a->get_pd_path_list())
		s.append(QString("<tr><td colspan=\"2\">%1</td></tr>").arg(entry));
	s.append(tr("<tr><td colspan=\"2\">(Note: Set environment variable SIGROKDECODE_DIR to add a custom directory)</td></tr>"));
#endif

	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Supported hardware drivers:") + "</b></td></tr>");
	for (pair<QString, QString> &entry : a->get_driver_list())
		s.append(QString("<tr><td class=\"id\"><i>%1</i></td><td>%2</td></tr>")
			.arg(entry.first, entry.second));

	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Supported input formats:") + "</b></td></tr>");
	for (pair<QString, QString> &entry : a->get_input_format_list())
		s.append(QString("<tr><td class=\"id\"><i>%1</i></td><td>%2</td></tr>")
			.arg(entry.first, entry.second));

	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Supported output formats:") + "</b></td></tr>");
	for (pair<QString, QString> &entry : a->get_output_format_list())
		s.append(QString("<tr><td class=\"id\"><i>%1</i></td><td>%2</td></tr>")
			.arg(entry.first, entry.second));

#ifdef ENABLE_DECODE
	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Supported protocol decoders:") + "</b></td></tr>");
	for (pair<QString, QString> &entry : a->get_pd_list())
		s.append(QString("<tr><td class=\"id\"><i>%1</i></td><td>%2</td></tr>")
			.arg(entry.first, entry.second));
#endif

	s.append("<tr><td colspan=\"2\"></td></tr>");
	s.append("<tr><td colspan=\"2\"><b>" +
		tr("Available Translations:") + "</b></td></tr>");
	for (const QString& language : a->get_languages()) {
		if (language == "en")
			continue;

		const QLocale locale = QLocale(language);
		const QString desc = locale.languageToString(locale.language());
		const QString editors = a->get_language_editors(language);

		s.append(QString("<tr><td class=\"id\"><i>%1</i></td><td>(%2)</td></tr>")
			.arg(desc, editors));
	}

	s.append("</table>");

	QTextDocument *supported_doc = new QTextDocument();
	supported_doc->setHtml(s);

	QTextBrowser *support_list = new QTextBrowser();
	support_list->setDocument(supported_doc);

	QHBoxLayout *h_layout = new QHBoxLayout();
	h_layout->setAlignment(Qt::AlignLeft);
	h_layout->addWidget(icon);
	h_layout->addWidget(gpl_home_info);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(h_layout);
	layout->addWidget(support_list);

	QWidget *page = new QWidget(parent);
	page->setLayout(layout);

	return page;
}

QWidget *Settings::get_logging_page(QWidget *parent) const
{
	GlobalSettings settings;

	// Log level
	QSpinBox *loglevel_sb = new QSpinBox();
	loglevel_sb->setMaximum(SR_LOG_SPEW);
	loglevel_sb->setValue(logging.get_log_level());
	connect(loglevel_sb, SIGNAL(valueChanged(int)), this,
		SLOT(on_log_logLevel_changed(int)));

	QHBoxLayout *loglevel_layout = new QHBoxLayout();
	loglevel_layout->addWidget(new QLabel(tr("Log level:")));
	loglevel_layout->addWidget(loglevel_sb);

	// Background buffer size
	QSpinBox *buffersize_sb = new QSpinBox();
	buffersize_sb->setSuffix(tr(" lines"));
	buffersize_sb->setMinimum(Logging::MIN_BUFFER_SIZE);
	buffersize_sb->setMaximum(Logging::MAX_BUFFER_SIZE);
	buffersize_sb->setValue(
		settings.value(GlobalSettings::Key_Log_BufferSize).toInt());
	connect(buffersize_sb, SIGNAL(valueChanged(int)), this,
		SLOT(on_log_bufferSize_changed(int)));

	QHBoxLayout *buffersize_layout = new QHBoxLayout();
	buffersize_layout->addWidget(new QLabel(tr("Length of background buffer:")));
	buffersize_layout->addWidget(buffersize_sb);

	// Save to file
	QPushButton *save_log_pb = new QPushButton(
		QIcon(":/icons/document-save-as.png"),
		tr("&Save to File"));
	connect(save_log_pb, SIGNAL(clicked(bool)),
		this, SLOT(on_log_saveToFile_clicked(bool)));

	// Pop out
	QPushButton *pop_out_pb = new QPushButton(
		QIcon(":/icons/window-new.png"),
		tr("&Pop out"));
	connect(pop_out_pb, SIGNAL(clicked(bool)),
		this, SLOT(on_log_popOut_clicked(bool)));

	QHBoxLayout *control_layout = new QHBoxLayout();
	control_layout->addLayout(loglevel_layout);
	control_layout->addLayout(buffersize_layout);
	control_layout->addWidget(save_log_pb);
	control_layout->addWidget(pop_out_pb);

	QVBoxLayout *root_layout = new QVBoxLayout();
	root_layout->addLayout(control_layout);
	root_layout->addWidget(log_view_);

	QWidget *page = new QWidget(parent);
	page->setLayout(root_layout);

	return page;
}

void Settings::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	if (!page_area->minimumHeight())
		return;

	/*
	 * The dialog has the height of its pages by now, so let go of the
	 * height that was asked for: from here on the pages scroll rather
	 * than ask for the dialog to be taller, which is what keeps a
	 * change of style from changing the size of the dialog. Letting go
	 * in the next turn of the event loop keeps the size the layout has
	 * just given the dialog.
	 */
	QTimer::singleShot(0, this, [this]() { page_area->setMinimumHeight(0); });
}

void Settings::fit_height_to_pages()
{
	if (fit_passes_ <= 0)
		return;
	fit_passes_--;

	/*
	 * The height the pages ask for is the height of the tallest of
	 * them, since any of them can be shown, and the parts around them
	 * take the rest of the height of the dialog.
	 */
	const int around = height() - page_area->height();
	int wanted = pages->sizeHint().height() + around;

	/* A page taller than the screen would put the buttons out of
	 * reach; the rest of the page is reached by scrolling it. */
	const QScreen *on_screen = screen();
	if (!on_screen)
		on_screen = QGuiApplication::primaryScreen();
	if (on_screen)
		wanted = qMin(wanted, on_screen->availableGeometry().height() - 80);

	if (wanted < height())
		resize(width(), wanted);

	/*
	 * The rows of the pages report the height of the new style only
	 * after a pass of the layout has run over them, so the height is
	 * measured again for a moment. Coming down in more than one step
	 * costs nothing: the pages are laid out again anyway.
	 */
	if (fit_passes_)
		QTimer::singleShot(50, this, [this]() { fit_height_to_pages(); });
}

void Settings::accept()
{
	GlobalSettings settings;
	settings.stop_tracking();

	QDialog::accept();
}

void Settings::reject()
{
	GlobalSettings settings;
	settings.undo_tracked_changes();

	QDialog::reject();
}

void Settings::on_page_changed(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;

	pages->setCurrentIndex(page_list->row(current));
}

void Settings::on_general_language_changed(const QString &text)
{
	GlobalSettings settings;
	Application* a = qobject_cast<Application*>(QApplication::instance());

	for (const QString& language : a->get_languages()) {
		QLocale locale = QLocale(language);
		QString desc = locale.languageToString(locale.language());

		if (text == desc)
			settings.setValue(GlobalSettings::Key_General_Language, language);
	}
}

void Settings::on_general_theme_changed(int value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_General_Theme, value);
	settings.apply_theme();
}

void Settings::on_general_style_changed(int index)
{
	QComboBox *box = qobject_cast<QComboBox *>(sender());
	if (!box)
		return;

	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_General_Style, box->itemData(index));
	settings.apply_theme();

	/*
	 * A row of the pages comes out at a height the style decides, so
	 * the height the pages ask for has just moved with it. Come back
	 * to it, but never up: the page the style is switched from has
	 * nothing to gain from a dialog that is taller than its own page,
	 * and the pages of a taller style scroll.
	 */
	fit_passes_ = 6;
	fit_height_to_pages();
}

void Settings::on_general_save_with_setup_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_General_SaveWithSetup, state ? true : false);
}

void Settings::on_general_start_all_sessions_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_General_StartAllSessions, state ? true : false);
}

void Settings::on_view_zoomToFitDuringAcq_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ZoomToFitDuringAcq, state ? true : false);
}

void Settings::on_view_zoomToFitAfterAcq_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ZoomToFitAfterAcq, state ? true : false);
}

void Settings::on_view_triggerIsZero_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_TriggerIsZeroTime, state ? true : false);
}

void Settings::on_view_coloredBG_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ColoredBG, state ? true : false);
}

void Settings::on_view_stickyScrolling_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_StickyScrolling, state ? true : false);
}

void Settings::on_view_allowVerticalDragging_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_AllowVerticalDragging, state ? true : false);
}

void Settings::on_view_showSamplingPoints_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ShowSamplingPoints, state ? true : false);
}

void Settings::on_view_fillSignalHighAreas_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_FillSignalHighAreas, state ? true : false);
}

void Settings::on_view_fillSignalHighAreaColor_changed(QColor color)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_FillSignalHighAreaColor, color.rgba());
}

void Settings::on_view_showAnalogMinorGrid_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ShowAnalogMinorGrid, state ? true : false);
}

void Settings::on_view_showHoverMarker_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ShowHoverMarker, state ? true : false);
}

void Settings::on_view_snapDistance_changed(int value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_SnapDistance, value);
}

void Settings::on_view_cursorFillColor_changed(QColor color)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_CursorFillColor, color.rgba());
}

void Settings::on_view_conversionThresholdDispMode_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_ConversionThresholdDispMode, state);
}

void Settings::on_view_defaultDivHeight_changed(int value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_DefaultDivHeight, value);
}

void Settings::on_view_defaultLogicHeight_changed(int value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_View_DefaultLogicHeight, value);
}

void Settings::on_set_threshold_level(float value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_ThresholdLevel, value);
}

#ifdef ENABLE_DECODE
void Settings::on_dec_initialStateConfigurable_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_Dec_InitialStateConfigurable, state ? true : false);
}

void Settings::on_dec_exportFormat_changed(const QString &text)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_Dec_ExportFormat, text);
}

void Settings::on_dec_alwaysshowallrows_changed(int state)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_Dec_AlwaysShowAllRows, state ? true : false);
}
#endif

void Settings::on_log_logLevel_changed(int value)
{
	logging.set_log_level(value);
}

void Settings::on_log_bufferSize_changed(int value)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_Log_BufferSize, value);
}

void Settings::on_log_saveToFile_clicked(bool checked)
{
	(void)checked;

	const QString file_name = QFileDialog::getSaveFileName(
		this, tr("Save Log"), "", tr("Log Files (*.txt *.log);;All Files (*)"));

	if (file_name.isEmpty())
		return;

	QFile file(file_name);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		QTextStream out_stream(&file);
		out_stream << log_view_->toPlainText();

		if (out_stream.status() == QTextStream::Ok) {
			QMessageBox msg(this);
			msg.setText(tr("Success") + "\n\n" + tr("Log saved to %1.").arg(file_name));
			msg.setStandardButtons(QMessageBox::Ok);
			msg.setIcon(QMessageBox::Information);
			msg.exec();

			return;
		}
	}

	QMessageBox msg(this);
	msg.setText(tr("Error") + "\n\n" + tr("File %1 could not be written to.").arg(file_name));
	msg.setStandardButtons(QMessageBox::Ok);
	msg.setIcon(QMessageBox::Warning);
	msg.exec();
}

void Settings::on_log_popOut_clicked(bool checked)
{
	(void)checked;

	// Create the window as a sub-window so it closes when the main window closes
	QMainWindow *window = new QMainWindow(nullptr, Qt::SubWindow);

	window->setObjectName(QString::fromUtf8("Log Window"));
	window->setWindowTitle(tr("%1 Log").arg(PV_TITLE));

	// Use same width/height as the settings dialog
	window->resize(width(), height());

	window->setCentralWidget(create_log_view());
	window->show();
}

} // namespace dialogs
} // namespace pv
