/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2020 Soeren Apel <soeren@apelpie.net>
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

#include <algorithm>
#include <climits>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QFontMetrics>
#include <QHash>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPolygonF>
#include <QPushButton>
#include <QSet>
#include <QStyle>
#include <QStyleOptionHeader>
#include <QToolBar>
#include <QVector>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <libsigrokdecode/libsigrokdecode.h>

#include "view.hpp"

#include "pv/globalsettings.hpp"
#include "pv/session.hpp"
#include "pv/util.hpp"
#include "pv/widgets/splitbutton.hpp"
#include "pv/data/decode/decoder.hpp"

using pv::data::DecodeSignal;
using pv::data::SignalBase;
using pv::data::decode::Decoder;
using pv::util::Timestamp;

using std::make_shared;
using std::max;
using std::shared_ptr;

namespace pv {
namespace views {
namespace tabular_decoder {

/* Marked for extraction here, which is where tr() looks them up. */
const char* SaveTypeNames[SaveTypeCount] = {
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "CSV, commas escaped"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "CSV, fields quoted"),
};

const char* ViewModeNames[ViewModeCount] = {
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "Show all"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View",
		"Show all and focus on newest"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View",
		"Show visible in main view")
};

const char* ValueFormatNames[ValueFormatCount] = {
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "As written"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "Hex"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "Decimal"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "Octal"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "Binary"),
	QT_TRANSLATE_NOOP("pv::views::tabular_decoder::View", "ASCII")
};


CustomFilterProxyModel::CustomFilterProxyModel(QObject* parent) :
	QSortFilterProxyModel(parent),
	range_filtering_enabled_(false)
{
}

bool CustomFilterProxyModel::filterAcceptsRow(int sourceRow,
	const QModelIndex &sourceParent) const
{
	(void)sourceParent;
	assert(sourceModel() != nullptr);

	return row_matches_filters(sourceRow, -1);
}

bool CustomFilterProxyModel::row_matches_filters(int sourceRow,
	int ignore_column) const
{
	// The values ticked off in a column's list of choices.
	for (auto it = excluded_values_.constBegin();
			it != excluded_values_.constEnd(); ++it) {
		if ((it.key() == ignore_column) || it.value().isEmpty())
			continue;

		const QString value = sourceModel()->data(
			sourceModel()->index(sourceRow, it.key()),
			Qt::DisplayRole).toString();

		if (it.value().contains(value))
			return false;
	}

	// The text may sit in any of the readable columns, hidden ones
	// included; the times and sample numbers are left out, since a
	// digit found there would match nearly every row.
	if (!text_filter_.isEmpty()) {
		bool found = false;

		for (int column = 0; column < sourceModel()->columnCount(); column++) {
			if (!AnnotationCollectionModel::is_text_column(column))
				continue;

			const QModelIndex idx = sourceModel()->index(sourceRow, column);

			if (sourceModel()->data(idx, Qt::DisplayRole).toString()
					.contains(text_filter_, Qt::CaseInsensitive)) {
				found = true;
				break;
			}
		}

		if (!found)
			return false;
	}

	bool result = true;

	if (range_filtering_enabled_) {
		const QModelIndex ann_start_sample_idx = sourceModel()->index(sourceRow, 0);
		const uint64_t ann_start_sample =
			sourceModel()->data(ann_start_sample_idx, Qt::DisplayRole).toULongLong();

		const QModelIndex ann_end_sample_idx = sourceModel()->index(sourceRow, 6);
		const uint64_t ann_end_sample =
			sourceModel()->data(ann_end_sample_idx, Qt::DisplayRole).toULongLong();

		// We consider all annotations as visible that either
		// a) begin to the left of the range and end within the range or
		// b) begin and end within the range or
		// c) begin within the range and end to the right of the range
		// ...which is equivalent to the negation of "begins and ends outside the range"

		const bool left_of_range = (ann_end_sample < range_start_sample_);
		const bool right_of_range = (ann_start_sample > range_end_sample_);
		const bool entirely_outside_of_range = left_of_range || right_of_range;

		result = !entirely_outside_of_range;
	}

	return result;
}

void CustomFilterProxyModel::set_excluded_values(int column,
	const QSet<QString>& excluded)
{
	if (excluded.isEmpty())
		excluded_values_.remove(column);
	else
		excluded_values_.insert(column, excluded);

	invalidate_filter();
}

QSet<QString> CustomFilterProxyModel::excluded_values(int column) const
{
	return excluded_values_.value(column);
}

QSet<int> CustomFilterProxyModel::filtered_columns() const
{
	QSet<int> columns;

	for (auto it = excluded_values_.constBegin();
			it != excluded_values_.constEnd(); ++it)
		if (!it.value().isEmpty())
			columns.insert(it.key());

	return columns;
}

void CustomFilterProxyModel::clear_excluded_values()
{
	if (excluded_values_.isEmpty())
		return;

	excluded_values_.clear();

	invalidate_filter();
}

QList<QPair<QString, int> > CustomFilterProxyModel::value_counts(
	int column) const
{
	QHash<QString, int> counts;

	const QAbstractItemModel* const src = sourceModel();
	if (src)
		for (int row = 0; row < src->rowCount(); row++)
			if (row_matches_filters(row, column))
				counts[src->data(src->index(row, column),
					Qt::DisplayRole).toString()]++;

	QList<QPair<QString, int> > values;
	for (auto it = counts.constBegin(); it != counts.constEnd(); ++it)
		values.append(qMakePair(it.key(), it.value()));

	// What the capture is mostly made of first, so the list opens
	// on the entries that are worth looking at.
	std::sort(values.begin(), values.end(),
		[](const QPair<QString, int>& a, const QPair<QString, int>& b) {
			if (a.second != b.second)
				return a.second > b.second;

			return a.first < b.first;
		});

	return values;
}

void CustomFilterProxyModel::invalidate_filter()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
	// endFilterChange() came in with Qt 6.10; only rows are filtered here
	beginFilterChange();
	endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
	invalidateFilter();
#endif
}

void CustomFilterProxyModel::set_sample_range(uint64_t start_sample,
	uint64_t end_sample)
{
	range_start_sample_ = start_sample;
	range_end_sample_ = end_sample;

	invalidate_filter();
}

void CustomFilterProxyModel::set_text_filter(const QString& text)
{
	text_filter_ = text;

	invalidate_filter();
}

void CustomFilterProxyModel::enable_range_filtering(bool value)
{
	range_filtering_enabled_ = value;

	invalidate_filter();
}


QSize CustomTableView::minimumSizeHint() const
{
	QSize size(QTableView::sizeHint());

	size.setWidth(10);

	return size;
}

QSize CustomTableView::sizeHint() const
{
	QSize size(QTableView::sizeHint());

	size.setWidth(250);

	return size;
}

void CustomTableView::keyPressEvent(QKeyEvent *event)
{
	if ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter)) {
		activatedByKey(currentIndex());
		return;
	}

	if (event->matches(QKeySequence::Copy)) {
		copy_selection_to_clipboard();
		return;
	}

	QTableView::keyPressEvent(event);
}

/*
 * Hands the selected cells over as tab separated fields, which is what
 * a spreadsheet or a text editor pastes back into a grid. Only the
 * columns on show are taken, in the order they are shown.
 */
void CustomTableView::copy_selection_to_clipboard() const
{
	const QItemSelectionModel *const selection = selectionModel();
	if (!selection)
		return;

	QVector<int> rows, selected_columns;

	for (const QModelIndex& idx : selection->selectedIndexes()) {
		if (!rows.contains(idx.row()))
			rows << idx.row();
		if (!selected_columns.contains(idx.column()))
			selected_columns << idx.column();
	}

	if (rows.isEmpty())
		return;

	QVector<int> columns;
	for (int pos = 0; pos < horizontalHeader()->count(); pos++) {
		const int column = horizontalHeader()->logicalIndex(pos);

		if (selected_columns.contains(column) && !isColumnHidden(column))
			columns << column;
	}

	std::sort(rows.begin(), rows.end());

	QStringList lines;
	for (int row : rows) {
		QStringList fields;

		for (int column : columns)
			fields << model()->data(model()->index(row, column),
				Qt::DisplayRole).toString();

		lines << fields.join('\t');
	}

	QGuiApplication::clipboard()->setText(
		lines.join(QStringLiteral("\r\n")));
}


FilterHeaderView::FilterHeaderView(QWidget* parent) :
	QHeaderView(Qt::Horizontal, parent),
	funnel_pressed_(false),
	hovered_section_(-1),
	pressed_section_(-1)
{
	setSectionsClickable(true);

	// The section under the mouse has to be drawn as such, and the
	// section being pressed with it.
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
}

void FilterHeaderView::set_filter_columns(const QSet<int>& columns)
{
	filter_columns_ = columns;

	update();
}

void FilterHeaderView::set_active_filters(const QSet<int>& columns)
{
	if (active_filters_ == columns)
		return;

	active_filters_ = columns;

	update();
}

QRect FilterHeaderView::funnel_rect(const QRect& section_rect)
{
	return QRect(section_rect.right() - 12, section_rect.center().y() - 4,
		10, 9);
}

int FilterHeaderView::funnel_hit_width(const QRect& section_rect)
{
	// The glyph is a small target, so the whole end of a section that
	// can spare it reacts to the click. A narrow column is nearly all
	// label, and there the click is left to the sorting.
	return (section_rect.width() >= 48) ? 20 : 12;
}

QRect FilterHeaderView::funnel_hit_rect(const QRect& section_rect)
{
	QRect hit(section_rect);

	hit.setLeft(section_rect.right() - funnel_hit_width(section_rect) + 1);

	return hit;
}

bool FilterHeaderView::is_funnel_at(const QPoint& pos) const
{
	const int logical = logicalIndexAt(pos);
	if ((logical < 0) || !filter_columns_.contains(logical))
		return false;

	const QRect section(sectionViewportPosition(logical), 0,
		sectionSize(logical), height());

	return funnel_hit_rect(section).contains(pos);
}

/*
 * The label is drawn here rather than left to the style so that the
 * funnel and the sort arrow both fit at the end of the section. The
 * style still paints the section itself.
 */
void FilterHeaderView::paintSection(QPainter* painter, const QRect& rect,
	int logical_index) const
{
	QStyleOptionHeader option;
	initStyleOption(&option);
	option.rect = rect;
	option.section = logical_index;

	option.state |= QStyle::State_Enabled;
	if (window() && window()->isActiveWindow())
		option.state |= QStyle::State_Active;
	if (logical_index == hovered_section_)
		option.state |= QStyle::State_MouseOver;
	if (logical_index == pressed_section_)
		option.state |= QStyle::State_Sunken;

	// initStyleOption() asks the model for the text of section -1,
	// which is why the label of this section is read here.
	const QAbstractItemModel* const model = this->model();
	const QString text = model ? model->headerData(logical_index,
		Qt::Horizontal, Qt::DisplayRole).toString() : QString();
	const QVariant alignment = model ? model->headerData(logical_index,
		Qt::Horizontal, Qt::TextAlignmentRole) : QVariant();
	const Qt::Alignment text_alignment = alignment.isValid() ?
		Qt::Alignment(alignment.toInt()) : defaultAlignment();

	option.text.clear();
	option.sortIndicator = QStyleOptionHeader::None;
	style()->drawControl(QStyle::CE_Header, &option, painter, this);

	int right = rect.right() - 1;

	if (filter_columns_.contains(logical_index)) {
		const QRect funnel = funnel_rect(rect);

		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setPen(option.palette.color(QPalette::ButtonText));
		// Filled while the column is narrowed, an outline when not.
		painter->setBrush(active_filters_.contains(logical_index) ?
			option.palette.brush(QPalette::Highlight) : Qt::NoBrush);

		QPolygonF shape;
		shape << QPointF(funnel.left(), funnel.top())
			<< QPointF(funnel.right(), funnel.top())
			<< QPointF(funnel.center().x() + 0.5,
				funnel.top() + 4.5)
			<< QPointF(funnel.center().x() + 0.5, funnel.bottom())
			<< QPointF(funnel.center().x() - 0.5, funnel.bottom())
			<< QPointF(funnel.center().x() - 0.5,
				funnel.top() + 4.5);
		painter->drawPolygon(shape);
		painter->restore();

		right = funnel.left() - 2;
	}

	if (isSortIndicatorShown() && (sortIndicatorSection() == logical_index)) {
		QStyleOptionHeader arrow(option);
		arrow.rect = QRect(right - 11, rect.top(), 12, rect.height());
		// The style shows a down arrow for ascending order.
		arrow.sortIndicator =
			(sortIndicatorOrder() == Qt::AscendingOrder) ?
			QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
		arrow.state |= QStyle::State_Enabled;

		style()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &arrow,
			painter, this);

		right = arrow.rect.left() - 2;
	}

	const QRect text_rect(rect.left() + 4, rect.top(),
		qMax(0, right - rect.left() - 3), rect.height());

	// The style's own label drawing, so the text keeps the colour
	// and the font it has in every other header.
	QStyleOptionHeader label(option);
	label.rect = text_rect;
	label.text = option.fontMetrics.elidedText(text, Qt::ElideRight,
		text_rect.width());
	label.textAlignment = text_alignment | Qt::AlignVCenter;
	label.state |= QStyle::State_Enabled;
	style()->drawControl(QStyle::CE_HeaderLabel, &label, painter, this);
}

void FilterHeaderView::mousePressEvent(QMouseEvent* event)
{
	funnel_pressed_ = is_funnel_at(event->pos());

	const int logical = logicalIndexAt(event->pos());
	if (logical != pressed_section_) {
		pressed_section_ = logical;
		update();
	}

	if (funnel_pressed_) {
		// Swallowed, so that asking for the values does not sort.
		const QRect section(sectionViewportPosition(logical), 0,
			sectionSize(logical), height());

		Q_EMIT filter_requested(logical,
			mapToGlobal(funnel_rect(section).bottomLeft()));
		return;
	}

	QHeaderView::mousePressEvent(event);
}

void FilterHeaderView::mouseReleaseEvent(QMouseEvent* event)
{
	if (pressed_section_ >= 0) {
		pressed_section_ = -1;
		update();
	}

	if (funnel_pressed_) {
		funnel_pressed_ = false;
		return;
	}

	QHeaderView::mouseReleaseEvent(event);
}

void FilterHeaderView::mouseMoveEvent(QMouseEvent* event)
{
	const int logical = logicalIndexAt(event->pos());
	if (logical != hovered_section_) {
		hovered_section_ = logical;
		update();
	}

	QHeaderView::mouseMoveEvent(event);
}

void FilterHeaderView::leaveEvent(QEvent* event)
{
	if (hovered_section_ >= 0) {
		hovered_section_ = -1;
		update();
	}

	QHeaderView::leaveEvent(event);
}

void FilterHeaderView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (is_funnel_at(event->pos()))
		return;

	QHeaderView::mouseDoubleClickEvent(event);
}


ColumnFilterPopup::ColumnFilterPopup(QWidget* parent) :
	QMenu(parent),
	column_(-1)
{
	title_action_ = new QAction(this);
	title_action_->setEnabled(false);
	addAction(title_action_);
	addSeparator();

	list_ = new QListWidget(this);
	list_->setSelectionMode(QAbstractItemView::NoSelection);
	list_->setUniformItemSizes(true);
	list_->setMinimumWidth(190);
	list_->setMaximumHeight(280);

	clear_button_ = new QPushButton(tr("Clear filter"), this);

	QWidget* content = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(content);
	layout->setContentsMargins(6, 2, 6, 6);
	layout->setSpacing(4);
	layout->addWidget(list_);
	layout->addWidget(clear_button_);

	QWidgetAction* action = new QWidgetAction(this);
	action->setDefaultWidget(content);
	addAction(action);

	connect(list_, SIGNAL(itemChanged(QListWidgetItem*)),
		this, SLOT(on_item_changed()));
	connect(clear_button_, SIGNAL(clicked()),
		this, SLOT(on_clear_clicked()));
}

void ColumnFilterPopup::show_values(int column, const QString& title,
	const QList<QPair<QString, int> >& values,
	const QSet<QString>& excluded, const QPoint& global_pos)
{
	column_ = column;
	title_action_->setText(title);

	{
		// The list is filled up for every column that opens.
		const QSignalBlocker blocker(list_);
		list_->clear();

		for (const QPair<QString, int>& value : values) {
			// How often a value turns up says what the table is made
			// of, which is what this list is read for.
			QListWidgetItem* item = new QListWidgetItem(
				QStringLiteral("%1 (%2)").arg(value.first)
					.arg(value.second), list_);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(excluded.contains(value.first) ?
				Qt::Unchecked : Qt::Checked);
			item->setData(Qt::UserRole, value.first);
		}
	}

	// As tall as the values need and no more; a longer list scrolls.
	int rows_height = 0;
	for (int row = 0; row < list_->count(); row++)
		rows_height += list_->sizeHintForRow(row);
	list_->setFixedHeight(qMin(rows_height + 2 * list_->frameWidth(), 280));

	clear_button_->setEnabled(!excluded.isEmpty());

	popup(global_pos);
}

void ColumnFilterPopup::on_item_changed()
{
	QStringList excluded;

	for (int row = 0; row < list_->count(); row++) {
		const QListWidgetItem* const item = list_->item(row);

		if (item->checkState() != Qt::Checked)
			excluded << item->data(Qt::UserRole).toString();
	}

	clear_button_->setEnabled(!excluded.isEmpty());

	Q_EMIT excluded_changed(column_, excluded);
}

void ColumnFilterPopup::on_clear_clicked()
{
	{
		const QSignalBlocker blocker(list_);
		for (int row = 0; row < list_->count(); row++)
			list_->item(row)->setCheckState(Qt::Checked);
	}

	clear_button_->setEnabled(false);

	Q_EMIT excluded_changed(column_, QStringList());
}


View::View(Session &session, bool is_main_view, QMainWindow *parent) :
	ViewBase(session, is_main_view, parent),

	// Note: Place defaults in View::reset_view_state(), not here
	parent_(parent),
	decoder_selector_(new QComboBox()),
	view_mode_selector_(new QComboBox()),
	value_format_selector_(new QComboBox()),
	row_filter_(new QLineEdit()),
	row_count_label_(new QLabel()),
	colored_annotations_button_(new QToolButton()),
	save_button_(new widgets::SplitButton()),
	save_action_(new QAction(this)),
	table_view_(new CustomTableView()),
	filter_header_(new FilterHeaderView()),
	filter_popup_(new ColumnFilterPopup(this)),
	model_(new AnnotationCollectionModel(this)),
	filter_proxy_model_(new CustomFilterProxyModel(this)),
	signal_(nullptr)
{
	QVBoxLayout *root_layout = new QVBoxLayout(this);
	root_layout->setContentsMargins(0, 0, 0, 0);
	root_layout->addWidget(table_view_);

	// Create toolbar
	QToolBar* toolbar = new QToolBar();
	toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
	toolbar->setMovable(false);
	parent->addToolBar(toolbar);

	// Populate toolbar
	toolbar->addWidget(new QLabel(tr("Decoder:")));
	toolbar->addWidget(decoder_selector_);
	toolbar->addSeparator();
	toolbar->addWidget(new QLabel(tr("Value:")));
	toolbar->addWidget(value_format_selector_);
	toolbar->addSeparator();
	toolbar->addWidget(save_button_);
	toolbar->addSeparator();
	toolbar->addWidget(view_mode_selector_);
	toolbar->addSeparator();
	toolbar->addWidget(colored_annotations_button_);

	// Pushed to the right: how much of the table is on show, and the
	// way to narrow it down.
	QWidget* toolbar_spacer = new QWidget();
	toolbar_spacer->setSizePolicy(QSizePolicy::Expanding,
		QSizePolicy::Preferred);
	toolbar->addWidget(toolbar_spacer);
	toolbar->addWidget(row_count_label_);
	toolbar->addWidget(row_filter_);

	connect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));
	connect(view_mode_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_view_mode_changed(int)));
	connect(value_format_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_value_format_changed(int)));

	// Configure widgets
	decoder_selector_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	row_filter_->setClearButtonEnabled(true);
	row_filter_->setPlaceholderText(tr("Filter rows"));
	row_filter_->setMaximumWidth(220);
	connect(row_filter_, SIGNAL(textChanged(const QString&)),
		this, SLOT(on_filter_text_changed(const QString&)));

	for (int i = 0; i < ViewModeCount; i++) {
		QString translated = tr(ViewModeNames[i]);
    	view_mode_selector_->addItem(translated, QVariant::fromValue(i));
	}

	value_format_selector_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	for (int i = 0; i < ValueFormatCount; i++)
		value_format_selector_->addItem(tr(ValueFormatNames[i]),
			QVariant::fromValue(i));
	value_format_selector_->setToolTip(tr("How the Value column is written"));


	// The table's own display toggles. They used to sit in the
	// settings dialog, where nobody looks for them.
	//
	// The style leaves a checked tool button as little more than a
	// hairline and this bar carries none of the rules the bars of the
	// main view set for themselves, so the on state is filled here with
	// the same colours they use.
	const QString toggle_style(QStringLiteral(
		"QToolButton {"
		"    padding: 2px 8px;"
		"    border: 1px solid transparent;"
		"    border-radius: 2px;"
		"}"
		"QToolButton:hover {"
		"    background: rgba(180, 180, 180, 60);"
		"    border: 1px solid rgba(120, 120, 120, 100);"
		"}"
		"QToolButton:pressed {"
		"    background: rgba(150, 150, 150, 80);"
		"}"
		"QToolButton:checked {"
		"    background: rgba(150, 150, 150, 110);"
		"    border: 1px solid rgba(140, 140, 140, 160);"
		"}"));
	colored_annotations_button_->setStyleSheet(toggle_style);

	colored_annotations_button_->setCheckable(true);
	colored_annotations_button_->setToolButtonStyle(Qt::ToolButtonTextOnly);
	colored_annotations_button_->setText(tr("Colors"));
	colored_annotations_button_->setToolTip(tr("Colored annotation rows"));

	connect(colored_annotations_button_, SIGNAL(toggled(bool)),
		this, SLOT(on_colored_annotations_toggled(bool)));

	// Configure actions
	save_action_->setText(tr("&Save"));
	save_action_->setIcon(QIcon(":/icons/document-save-as.png"));
	save_action_->setShortcut(pv::util::key_sequence(Qt::CTRL, Qt::Key_S));
	connect(save_action_, SIGNAL(triggered(bool)),
		this, SLOT(on_actionSave_triggered()));

	QMenu *save_menu = new QMenu(save_button_);
	connect(save_menu, SIGNAL(triggered(QAction*)),
		this, SLOT(on_actionSave_triggered(QAction*)));

	for (int i = 0; i < SaveTypeCount; i++) {
		QAction *const action =	save_menu->addAction(tr(SaveTypeNames[i]));
		action->setData(QVariant::fromValue(i));
	}

	save_button_->setDefaultAction(save_action_);
	save_button_->set_menu(save_menu);

	// Set up the models and the table view
	filter_proxy_model_->setSourceModel(model_);
	table_view_->setModel(filter_proxy_model_);

	// The header carries the per-column value filters, so it goes in
	// before it is configured below.
	table_view_->setHorizontalHeader(filter_header_);
	filter_header_->set_filter_columns(filter_columns());

	connect(filter_header_, SIGNAL(filter_requested(int, const QPoint&)),
		this, SLOT(on_table_header_filter_requested(int, const QPoint&)));
	connect(filter_popup_, SIGNAL(excluded_changed(int, const QStringList&)),
		this, SLOT(on_column_filter_changed(int, const QStringList&)));

	table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
	table_view_->setSelectionMode(QAbstractItemView::ContiguousSelection);
	filter_proxy_model_->setSortRole(AnnotationCollectionModel::SortRole);
	table_view_->setSortingEnabled(true);
	// Time is what an annotation list is read in.
	table_view_->sortByColumn(1, Qt::AscendingOrder);

	table_view_->viewport()->setAttribute(Qt::WA_Hover, false);

	table_view_->setShowGrid(true);
	table_view_->setGridStyle(Qt::SolidLine);

	for (uint8_t i = model_->first_hidden_column(); i < model_->columnCount(); i++)
		table_view_->setColumnHidden(i, true);

	table_view_->setColumnHidden(0, true);
	table_view_->setColumnHidden(2, true);

	const int font_height = QFontMetrics(QApplication::font()).height();
	table_view_->verticalHeader()->setDefaultSectionSize((font_height * 5) / 4);
	table_view_->verticalHeader()->setVisible(false);

	table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	table_view_->horizontalHeader()->setStretchLastSection(true);
	table_view_->horizontalHeader()->setSectionsMovable(true);
	table_view_->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

	table_view_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	parent->setSizePolicy(table_view_->sizePolicy());

	connect(table_view_, SIGNAL(clicked(const QModelIndex&)),
		this, SLOT(on_table_item_clicked(const QModelIndex&)));
	connect(table_view_, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(on_table_item_double_clicked(const QModelIndex&)));
	connect(table_view_, SIGNAL(activatedByKey(const QModelIndex&)),
		this, SLOT(on_table_item_double_clicked(const QModelIndex&)));
	connect(table_view_->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(on_table_header_requested(const QPoint&)));

	// Set up metadata event handler
	session_.metadata_obj_manager()->add_observer(this);

	// The toggle reads from the stored setting, and writes it back
	// through GlobalSettings so the state survives a restart.
	GlobalSettings settings;
	const bool colored = settings.value(
		GlobalSettings::Key_Dec_ColoredAnnotations, true).toBool();

	model_->set_colored_annotations(colored);

	colored_annotations_button_->setChecked(colored);

	GlobalSettings::add_change_handler(this);

	reset_view_state();
	restore_table_state();
	update_row_count();
}

View::~View()
{
	GlobalSettings::remove_change_handler(this);

	// Before the selector is emptied by clear_decode_signals() below.
	save_table_state();

	session_.metadata_obj_manager()->remove_observer(this);
	clear_decode_signals();
}

ViewType View::get_type() const
{
	return ViewTypeTabularDecoder;
}

void View::reset_view_state()
{
	ViewBase::reset_view_state();

	delayed_view_updater_.stop();

	// The filters belonged to the decoding that is going away.
	row_filter_->clear();
	filter_proxy_model_->clear_excluded_values();
	filter_header_->set_active_filters(QSet<int>());

	disconnect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));

	decoder_selector_->clear();

	connect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));
}

void View::clear_decode_signals()
{
	for (const shared_ptr<DecodeSignal>& signal : decode_signals_) {
		disconnect(signal.get(), SIGNAL(name_changed(const QString&)),
			this, SLOT(on_signal_name_changed(const QString&)));
		disconnect(signal.get(), SIGNAL(decoder_stacked(void*)),
			this, SLOT(on_decoder_stacked(void*)));
		disconnect(signal.get(), SIGNAL(decoder_removed(void*)),
			this, SLOT(on_decoder_removed(void*)));

		if (signal.get() == signal_) {
			disconnect(signal.get(), SIGNAL(color_changed(QColor)));
			disconnect(signal.get(), SIGNAL(new_annotations()));
			disconnect(signal.get(), SIGNAL(decode_reset()));
		}
	}

	disconnect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));

	ViewBase::clear_decode_signals();

	reset_data();
	reset_view_state();

	connect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));
}

void View::add_decode_signal(shared_ptr<data::DecodeSignal> signal)
{
	ViewBase::add_decode_signal(signal);

	connect(signal.get(), SIGNAL(name_changed(const QString&)),
		this, SLOT(on_signal_name_changed(const QString&)));

	// Note: At time of initial creation, decode signals have no decoders so we
	// need to watch for decoder stacking events

	connect(signal.get(), SIGNAL(decoder_stacked(void*)),
		this, SLOT(on_decoder_stacked(void*)));
	connect(signal.get(), SIGNAL(decoder_removed(void*)),
		this, SLOT(on_decoder_removed(void*)));

	// Add the top-level decoder provided by an already-existing signal
	auto stack = signal->decoder_stack();
	if (!stack.empty()) {
		shared_ptr<Decoder>& dec = stack.at(0);
		decoder_selector_->addItem(signal->name(), QVariant::fromValue((void*)dec.get()));
	}

	apply_wanted_decoder();
}

void View::remove_decode_signal(shared_ptr<data::DecodeSignal> signal)
{
	disconnect(signal.get(), SIGNAL(name_changed(const QString&)),
		this, SLOT(on_signal_name_changed(const QString&)));
	disconnect(signal.get(), SIGNAL(decoder_stacked(void*)),
		this, SLOT(on_decoder_stacked(void*)));
	disconnect(signal.get(), SIGNAL(decoder_removed(void*)),
		this, SLOT(on_decoder_removed(void*)));

	if (signal.get() == signal_) {
		disconnect(signal.get(), SIGNAL(color_changed(QColor)));
		disconnect(signal.get(), SIGNAL(new_annotations()));
		disconnect(signal.get(), SIGNAL(decode_reset()));
	}

	disconnect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));

	// Remove all decoders provided by this signal
	for (const shared_ptr<Decoder>& dec : signal->decoder_stack()) {
		int index = decoder_selector_->findData(QVariant::fromValue((void*)dec.get()));

		if (index != -1)
			decoder_selector_->removeItem(index);
	}

	ViewBase::remove_decode_signal(signal);

	if (signal.get() == signal_) {
		reset_data();
		update_data();
		reset_view_state();
	}

	connect(decoder_selector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_selected_decoder_changed(int)));
}

void View::save_settings(QSettings &settings) const
{
	ViewBase::save_settings(settings);

	settings.setValue("view_mode", view_mode_selector_->currentIndex());

	save_table_state();
}

/*
 * Column widths, order, hidden columns and the decoder in use are a user
 * preference rather than part of the session: the view is rebuilt every time
 * it is opened, so without this the table comes back in its default shape.
 */
void View::save_table_state() const
{
	GlobalSettings settings;

	settings.setValue("TabularDecoder/header_state",
		table_view_->horizontalHeader()->saveState());
	settings.setValue("TabularDecoder/columns", model_->columnCount());
	settings.setValue("TabularDecoder/sort_column",
		table_view_->horizontalHeader()->sortIndicatorSection());
	settings.setValue("TabularDecoder/sort_order",
		(int)table_view_->horizontalHeader()->sortIndicatorOrder());
	settings.setValue("TabularDecoder/decoder", decoder_selector_->currentText());
}

void View::restore_table_state()
{
	GlobalSettings settings;

	// Left alone if the columns have changed since it was written.
	const QByteArray state =
		settings.value("TabularDecoder/header_state").toByteArray();
	if (!state.isEmpty() &&
		settings.value("TabularDecoder/columns").toInt() == model_->columnCount())
		table_view_->horizontalHeader()->restoreState(state);

	// The order the table is really in, so the two cannot disagree.
	table_view_->sortByColumn(
		settings.value("TabularDecoder/sort_column", 1).toInt(),
		(Qt::SortOrder)settings.value("TabularDecoder/sort_order",
			(int)Qt::AscendingOrder).toInt());

	value_format_selector_->setCurrentIndex(
		settings.value("TabularDecoder/value_format", ValueRaw).toInt());

	wanted_decoder_ =
		settings.value("TabularDecoder/decoder").toString();
	apply_wanted_decoder();
}

void View::apply_wanted_decoder()
{
	if (wanted_decoder_.isEmpty())
		return;

	const int index = decoder_selector_->findText(wanted_decoder_);

	// Still could turn up: a newer decoding may be added at any time.
	if (index == -1)
		return;

	wanted_decoder_.clear();
	decoder_selector_->setCurrentIndex(index);
}

void View::restore_settings(QSettings &settings)
{
	ViewBase::restore_settings(settings);

	if (settings.contains("view_mode"))
		view_mode_selector_->setCurrentIndex(settings.value("view_mode").toInt());
}

QSet<int> View::filter_columns() const
{
	QSet<int> columns;

	for (int column = 0; column < model_->columnCount(); column++)
		if (AnnotationCollectionModel::is_filterable_column(column))
			columns.insert(column);

	return columns;
}

void View::reset_data()
{
	delayed_view_updater_.stop();

	signal_ = nullptr;
	decoder_ = nullptr;
}

void View::update_data()
{
	// The table shows the segment the main view is on.
	current_segment_ = main_view_segment();

	model_->set_signal_and_segment(signal_, current_segment_);

	update_row_count();
}

/* How much of the table is on show: the two numbers differ once a
 * filter or the main view's range leaves rows out. */
void View::update_row_count()
{
	const int shown = filter_proxy_model_->rowCount();
	const int total = model_->rowCount();

	const QString text = (shown == total) ?
		tr("%1 rows").arg(total) :
		tr("%1 of %2 rows").arg(shown).arg(total);

	if (row_count_label_->text() != text)
		row_count_label_->setText(text);
}

void View::on_filter_text_changed(const QString& text)
{
	filter_proxy_model_->set_text_filter(text);

	update_row_count();

	// Force repaint, otherwise the filtering isn't shown for some reason
	table_view_->viewport()->update();
}

void View::save_data_as_csv(unsigned int save_type) const
{
	// Note: We try to follow RFC 4180 (https://tools.ietf.org/html/rfc4180)

	assert(decoder_);
	assert(signal_);

	if (!signal_)
		return;

	// Save what is selected; the whole table when nothing is selected.
	const bool save_selection =
		table_view_->selectionModel()->hasSelection();
	const int row_count = filter_proxy_model_->rowCount();

	GlobalSettings settings;
	const QString dir = settings.value("MainWindow/SaveDirectory").toString();

	const QString file_name = QFileDialog::getSaveFileName(
		parent_, tr("Save Annotations as CSV"), dir, tr("CSV Files (*.csv);;Text Files (*.txt);;All Files (*)"));

	if (file_name.isEmpty())
		return;

	QFile file(file_name);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QTextStream out_stream(&file);

		// Write out header columns in visual order, not logical order
		bool first_field = true;
		for (int i = 0; i < table_view_->horizontalHeader()->count(); i++) {
			const int column = table_view_->horizontalHeader()->logicalIndex(i);

			if (table_view_->horizontalHeader()->isSectionHidden(column))
				continue;

			const QString title = filter_proxy_model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

			if (!first_field)
				out_stream << ",";
			first_field = false;

			if (save_type == SaveTypeCSVEscaped)
				out_stream << title;
			else
				out_stream << '"' << title << '"';
		}
		out_stream << '\r' << '\n';

		// Collected here so a cancelled dialog costs nothing.
		QVector<int> rows;
		for (int row = 0; row < row_count; row++)
			if (!save_selection ||
				table_view_->selectionModel()->isRowSelected(row, QModelIndex()))
				rows << row;

		for (int row : rows) {

			// Write out columns in visual order, not logical order
			bool first_cell = true;
			for (int c = 0; c < table_view_->horizontalHeader()->count(); c++) {
				const int column = table_view_->horizontalHeader()->logicalIndex(c);

				if (table_view_->horizontalHeader()->isSectionHidden(column))
					continue;

				const QModelIndex idx = filter_proxy_model_->index(row, column);
				QString s = filter_proxy_model_->data(idx, Qt::DisplayRole).toString();

				if (!first_cell)
					out_stream << ",";
				first_cell = false;

				if (save_type == SaveTypeCSVEscaped)
					out_stream << s.replace(",", "\\,");
				else
					out_stream << '"' << s.replace("\"", "\"\"") << '"';
			}

			out_stream << '\r' << '\n';
		}

		if (out_stream.status() == QTextStream::Ok)
			return;
	}

	QMessageBox msg(parent_);
	msg.setText(tr("Error") + "\n\n" + tr("File %1 could not be written to.").arg(file_name));
	msg.setStandardButtons(QMessageBox::Ok);
	msg.setIcon(QMessageBox::Warning);
	msg.exec();
}

void View::on_selected_decoder_changed(int index)
{
	if (signal_) {
		disconnect(signal_, SIGNAL(color_changed(QColor)));
		disconnect(signal_, SIGNAL(new_annotations()));
		disconnect(signal_, SIGNAL(decode_reset()));
	}

	reset_data();

	decoder_ = (Decoder*)decoder_selector_->itemData(index).value<void*>();

	// Find the signal that contains the selected decoder
	for (const shared_ptr<DecodeSignal>& ds : decode_signals_)
		for (const shared_ptr<Decoder>& dec : ds->decoder_stack())
			if (decoder_ == dec.get())
				signal_ = ds.get();

	if (signal_) {
		connect(signal_, SIGNAL(color_changed(QColor)), this, SLOT(on_signal_color_changed(QColor)));
		connect(signal_, SIGNAL(new_annotations()), this, SLOT(on_new_annotations()));
		connect(signal_, SIGNAL(decode_reset()), this, SLOT(on_decoder_reset()));
	}

	update_data();

	// Force repaint, otherwise the new selection isn't shown for some reason
	table_view_->viewport()->update();
}

void View::on_table_header_filter_requested(int column, const QPoint& global_pos)
{
	const QString title = filter_proxy_model_->headerData(column,
		Qt::Horizontal, Qt::DisplayRole).toString();

	filter_popup_->show_values(column, title,
		filter_proxy_model_->value_counts(column),
		filter_proxy_model_->excluded_values(column), global_pos);
}

void View::on_column_filter_changed(int column, const QStringList& excluded)
{
	filter_proxy_model_->set_excluded_values(column,
		QSet<QString>(excluded.cbegin(), excluded.cend()));

	filter_header_->set_active_filters(
		filter_proxy_model_->filtered_columns());

	update_row_count();

	// Force repaint, otherwise the filtering isn't shown for some reason
	table_view_->viewport()->update();
}

void View::on_value_format_changed(int index)
{
	const int format = value_format_selector_->itemData(index).toInt();

	model_->set_value_format(format);

	GlobalSettings settings;
	settings.setValue("TabularDecoder/value_format", format);
}

void View::on_view_mode_changed(int index)
{
	if (index == ViewModeAll)
		filter_proxy_model_->enable_range_filtering(false);

	if (index == ViewModeVisible) {
		MetadataObject *md_obj =
			session_.metadata_obj_manager()->find_object_by_type(MetadataObjMainViewRange);
		assert(md_obj);

		int64_t start_sample = md_obj->value(MetadataValueStartSample).toLongLong();
		int64_t end_sample = md_obj->value(MetadataValueEndSample).toLongLong();

		filter_proxy_model_->enable_range_filtering(true);
		filter_proxy_model_->set_sample_range(max((int64_t)0, start_sample),
			max((int64_t)0, end_sample));
	}

	if (index == ViewModeLatest) {
		filter_proxy_model_->enable_range_filtering(false);

		table_view_->scrollTo(
			filter_proxy_model_->mapFromSource(model_->index(model_->rowCount() - 1, 0)),
			QAbstractItemView::PositionAtBottom);
	}

	update_row_count();
}

void View::on_signal_name_changed(const QString &name)
{
	(void)name;

	SignalBase* sb = qobject_cast<SignalBase*>(QObject::sender());
	assert(sb);

	DecodeSignal* signal = dynamic_cast<DecodeSignal*>(sb);
	assert(signal);

	// Update the top-level decoder provided by this signal
	auto stack = signal->decoder_stack();
	if (!stack.empty()) {
		shared_ptr<Decoder>& dec = stack.at(0);
		int index = decoder_selector_->findData(QVariant::fromValue((void*)dec.get()));

		if (index != -1)
			decoder_selector_->setItemText(index, signal->name());
	}
}

void View::on_signal_color_changed(const QColor &color)
{
	(void)color;

	// Force immediate repaint, otherwise it's updated after the header popup is closed
	table_view_->viewport()->update();
}

void View::on_new_annotations()
{
	if (view_mode_selector_->currentIndex() == ViewModeLatest) {
		update_data();
		table_view_->scrollTo(
			filter_proxy_model_->index(filter_proxy_model_->rowCount() - 1, 0),
			QAbstractItemView::PositionAtBottom);
	} else {
		if (!delayed_view_updater_.isActive())
			delayed_view_updater_.start();
	}
}

void View::on_decoder_reset()
{
	// Invalidate the model's data connection immediately - otherwise we
	// will use a stale pointer in model_->index() when called from the table view
	model_->set_signal_and_segment(signal_, current_segment_);
}

void View::on_decoder_stacked(void* decoder)
{
	Decoder* d = static_cast<Decoder*>(decoder);

	// Find the signal that contains the selected decoder
	DecodeSignal* signal = nullptr;

	for (const shared_ptr<DecodeSignal>& ds : decode_signals_)
		for (const shared_ptr<Decoder>& dec : ds->decoder_stack())
			if (d == dec.get())
				signal = ds.get();

	assert(signal);

	const shared_ptr<Decoder>& dec = signal->decoder_stack().at(0);
	int index = decoder_selector_->findData(QVariant::fromValue((void*)dec.get()));

	if (index == -1) {
		// Add the decoder to the list
		decoder_selector_->addItem(signal->name(), QVariant::fromValue((void*)d));
	}

	apply_wanted_decoder();
}

void View::on_decoder_removed(void* decoder)
{
	Decoder* d = static_cast<Decoder*>(decoder);

	// Remove the decoder from the list
	int index = decoder_selector_->findData(QVariant::fromValue((void*)d));

	if (index != -1)
		decoder_selector_->removeItem(index);
}

void View::on_actionSave_triggered(QAction* action)
{
	int save_type = SaveTypeCSVQuoted;

	if (action)
		save_type = action->data().toInt();

	save_data_as_csv(save_type);
}

void View::on_table_item_clicked(const QModelIndex& index)
{
	// (void)index;

	// Force repaint, otherwise the new selection isn't shown for some reason
	table_view_->viewport()->update();
	const QModelIndex src_idx = filter_proxy_model_->mapToSource(index);

	const Annotation* ann = static_cast<const Annotation*>(src_idx.internalPointer());

	if (!ann || !src_idx.isValid())
		return;

	shared_ptr<views::ViewBase> main_view = session_.main_view();

	main_view->focus_on_range(ann->start_sample(), ann->end_sample());
}

void View::on_table_item_double_clicked(const QModelIndex& index)
{
	const QModelIndex src_idx = filter_proxy_model_->mapToSource(index);

	const Annotation* ann = static_cast<const Annotation*>(src_idx.internalPointer());

	if (!ann || !src_idx.isValid())
		return;

	shared_ptr<views::ViewBase> main_view = session_.main_view();

	main_view->focus_on_range(ann->start_sample(), ann->end_sample());
}

void View::on_table_header_requested(const QPoint& pos)
{
	QMenu* menu = new QMenu(this);

	for (int i = 0; i < table_view_->horizontalHeader()->count(); i++) {
		int column = table_view_->horizontalHeader()->logicalIndex(i);

		const QString title =
			filter_proxy_model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
		QAction* action = new QAction(title, this);

		action->setCheckable(true);
		action->setChecked(!table_view_->horizontalHeader()->isSectionHidden(column));
		action->setData(column);

		connect(action, SIGNAL(toggled(bool)), this, SLOT(on_table_header_toggled(bool)));

		menu->addAction(action);
	}

	menu->popup(table_view_->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void View::on_table_header_toggled(bool checked)
{
	QAction* action = qobject_cast<QAction*>(QObject::sender());
	assert(action);

	const int column = action->data().toInt();

	table_view_->horizontalHeader()->setSectionHidden(column, !checked);
}

void View::on_metadata_object_changed(MetadataObject* obj,
	MetadataValueType value_type)
{
	// The main view moved to another segment: read the new one.
	if ((obj->type() == MetadataObjCurrentSegment) &&
		(value_type == MetadataValueSegmentId))
		update_data();

	// Check if we need to update the model's data range. We only work on the
	// end sample value because the start sample value is updated first and
	// we don't want to update the model twice
	if ((view_mode_selector_->currentIndex() == ViewModeVisible) &&
		(obj->type() == MetadataObjMainViewRange) &&
		(value_type == MetadataValueEndSample)) {

		int64_t start_sample = obj->value(MetadataValueStartSample).toLongLong();
		int64_t end_sample = obj->value(MetadataValueEndSample).toLongLong();

		filter_proxy_model_->set_sample_range(max((int64_t)0, start_sample),
			max((int64_t)0, end_sample));

		update_row_count();
	}

	if (obj->type() == MetadataObjMousePos) {
		QModelIndex first_visible_idx =
			filter_proxy_model_->mapToSource(filter_proxy_model_->index(0, 0));
		QModelIndex last_visible_idx =
			filter_proxy_model_->mapToSource(filter_proxy_model_->index(filter_proxy_model_->rowCount() - 1, 0));

		if (first_visible_idx.isValid()) {
			const QModelIndex first_highlighted_idx =
				model_->update_highlighted_rows(first_visible_idx, last_visible_idx,
					obj->value(MetadataValueStartSample).toLongLong());

			if (view_mode_selector_->currentIndex() == ViewModeVisible) {
				const QModelIndex idx = filter_proxy_model_->mapFromSource(first_highlighted_idx);
				table_view_->scrollTo(idx, QAbstractItemView::EnsureVisible);
			}

			// Force repaint, otherwise the table doesn't immediately update for some reason
			table_view_->viewport()->update();
		}
	}
}

/*
 * The two toggles in the toolbar are the only place these settings can
 * be changed now; setChecked() only signals on a real change, so the
 * round trip through on_setting_changed() stops here.
 */
void View::on_colored_annotations_toggled(bool checked)
{
	GlobalSettings settings;
	settings.setValue(GlobalSettings::Key_Dec_ColoredAnnotations, checked);
}

void View::on_setting_changed(const QString &key, const QVariant &value)
{
	if (key != GlobalSettings::Key_Dec_ColoredAnnotations)
		return;

	colored_annotations_button_->setChecked(value.toBool());
	model_->set_colored_annotations(value.toBool());

	// Force repaint, otherwise the change isn't shown for some reason
	table_view_->viewport()->update();
}

void View::capture_state_updated(int state)
{
	if (state == Session::Running) {
		delayed_view_updater_.stop();
		model_->set_signal_and_segment(signal_, current_segment_);
		update_row_count();
	}

	ViewBase::capture_state_updated(state);
}

void View::perform_delayed_view_update()
{
	update_data();
}


} // namespace tabular_decoder
} // namespace views
} // namespace pv
