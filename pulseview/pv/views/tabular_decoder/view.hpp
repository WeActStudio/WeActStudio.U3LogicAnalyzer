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

#ifndef PULSEVIEW_PV_VIEWS_TABULAR_DECODER_VIEW_HPP
#define PULSEVIEW_PV_VIEWS_TABULAR_DECODER_VIEW_HPP

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QHash>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPair>
#include <QPushButton>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QToolButton>

#include "pv/globalsettings.hpp"
#include "pv/metadata_obj.hpp"
#include "pv/views/viewbase.hpp"
#include "pv/data/decodesignal.hpp"

namespace pv {
class Session;

namespace views {

namespace tabular_decoder {

// When adding an entry here, don't forget to update SaveTypeNames as well
enum SaveType {
	SaveTypeCSVEscaped,
	SaveTypeCSVQuoted,
	SaveTypeCount  // Indicates how many save types there are, must always be last
};

// When adding an entry here, don't forget to update ViewModeNames as well
enum ViewModeType {
	ViewModeAll,
	ViewModeLatest,
	ViewModeVisible,
	ViewModeCount // Indicates how many view mode types there are, must always be last
};

extern const char* SaveTypeNames[SaveTypeCount];
extern const char* ViewModeNames[ViewModeCount];


class AnnotationCollectionModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	AnnotationCollectionModel(QObject* parent = nullptr);

	/**
	 * What the table sorts by. The displayed time is a string such as
	 * "900 us", which would be ordered as text, so the numbers come from
	 * this role instead.
	 */
	static constexpr int SortRole = Qt::UserRole;

	int get_hierarchy_level(const Annotation* ann) const;
	QVariant data_from_ann(const Annotation* ann, int index) const;
	QVariant sort_key_from_ann(const Annotation* ann, int index) const;
	QVariant data(const QModelIndex& index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	uint8_t first_hidden_column() const;

	/**
	 * The columns that hold one of a few categories rather than a value
	 * of their own: the decoder, the annotation row ("class") and its
	 * class ("subclass"). A list of choices is worth offering there.
	 */
	static bool is_filterable_column(int column);

	/**
	 * The columns that hold something to read, as opposed to the times
	 * and sample numbers of an annotation: only those are searched for
	 * text, since a digit turns up in nearly every time.
	 */
	static bool is_text_column(int column);
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
		const QModelIndex& parent_idx = QModelIndex()) const override;

	QModelIndex parent(const QModelIndex& index) const override;

	int rowCount(const QModelIndex& parent_idx = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent_idx = QModelIndex()) const override;

	void set_signal_and_segment(data::DecodeSignal* signal, uint32_t current_segment);
	void set_colored_annotations(bool colored);

	void update_annotations_without_hidden();
	QModelIndex update_highlighted_rows(QModelIndex first, QModelIndex last,
		int64_t sample_num);

private Q_SLOTS:
	void on_annotation_visibility_changed();

private:
	vector<QVariant> header_data_;
	const deque<const Annotation*>* all_annotations_;
	deque<const Annotation*> all_annotations_without_hidden_;
	const deque<const Annotation*>* dataset_;
	data::DecodeSignal* signal_;
	uint8_t first_hidden_column_;
	uint32_t prev_segment_;
	uint64_t prev_last_row_;
	int64_t highlight_sample_num_;
	bool had_highlight_before_;
	bool colored_annotations_;
};


class CustomFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	CustomFilterProxyModel(QObject* parent = 0);

	void set_sample_range(uint64_t start_sample, uint64_t end_sample);
	void set_text_filter(const QString& text);

	void enable_range_filtering(bool value);

	/// The values of a column that are meant to stay out of the table.
	void set_excluded_values(int column, const QSet<QString>& excluded);
	QSet<QString> excluded_values(int column) const;
	/// The columns a value filter is narrowing right now.
	QSet<int> filtered_columns() const;
	void clear_excluded_values();

	/**
	 * What a column holds and how often, over the rows the other
	 * filters let through, most frequent first.
	 */
	QList<QPair<QString, int> > value_counts(int column) const;

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
	/**
	 * Tells the model that the filter has to be run again. Qt 6 wants the
	 * change to be bracketed, Qt 5 has no such call, hence the two ways.
	 */
	void invalidate_filter();

	/**
	 * Everything the table is narrowed by. Leaving one column's own
	 * value filter out is what its list of choices is built from.
	 */
	bool row_matches_filters(int sourceRow, int ignore_column) const;

	uint64_t range_start_sample_, range_end_sample_;
	bool range_filtering_enabled_;
	QString text_filter_;
	QHash<int, QSet<QString> > excluded_values_;
};


class CustomTableView : public QTableView
{
	Q_OBJECT

public:
	virtual QSize minimumSizeHint() const override;
	virtual QSize sizeHint() const override;

protected:
	virtual void keyPressEvent(QKeyEvent *event) override;

private:
	void copy_selection_to_clipboard() const;

Q_SIGNALS:
	void activatedByKey(const QModelIndex &index);
};


/**
 * The save button of the table: the body saves straight away and the
 * end of it opens the menu of formats. The style in use draws no
 * indicator for that end and leaves it a few pixels wide, so the
 * chevron is drawn here and the end is made wide enough to aim at.
 */
class SplitButton : public QToolButton
{
	Q_OBJECT

public:
	explicit SplitButton(QWidget* parent = nullptr);

	/**
	 * The menu the end of the button opens. It is kept here rather than
	 * handed to QToolButton, which would reserve a part of the button
	 * for an indicator this style does not draw.
	 */
	void set_menu(QMenu* menu);

protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	/// How much of the end opens the menu, drawn and clicked alike.
	static const int kMenuWidth = 20;

	QMenu* menu_;
	bool menu_pressed_;
};


/**
 * A header that carries a funnel at the end of the columns holding one
 * of a few categories. Clicking it asks the view for the list of values.
 */
class FilterHeaderView : public QHeaderView
{
	Q_OBJECT

public:
	explicit FilterHeaderView(QWidget* parent = nullptr);

	/// The columns that get a funnel at all.
	void set_filter_columns(const QSet<int>& columns);
	/// The columns a value filter is narrowing right now.
	void set_active_filters(const QSet<int>& columns);

	/// Where the funnel is drawn, at the end of a section.
	static QRect funnel_rect(const QRect& section_rect);
	/// What reacts to a click, which is a wider target than the glyph.
	static QRect funnel_hit_rect(const QRect& section_rect);

	/**
	 * How much of the end of a section answers to a click on the funnel.
	 * A narrow column is nearly all label, so only the glyph takes the
	 * click there and the rest goes on sorting.
	 */
	static int funnel_hit_width(const QRect& section_rect);

Q_SIGNALS:
	void filter_requested(int column, const QPoint& global_pos);

protected:
	void paintSection(QPainter* painter, const QRect& rect,
		int logical_index) const override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;

private:
	bool is_funnel_at(const QPoint& pos) const;

	QSet<int> filter_columns_;
	QSet<int> active_filters_;
	bool funnel_pressed_;
	int hovered_section_;
	int pressed_section_;
};


/**
 * The values of one column with a checkbox each. A plain menu would close
 * after every click, so the list is a widget inside it.
 */
class ColumnFilterPopup : public QMenu
{
	Q_OBJECT

public:
	explicit ColumnFilterPopup(QWidget* parent = nullptr);

	void show_values(int column, const QString& title,
		const QList<QPair<QString, int> >& values,
		const QSet<QString>& excluded, const QPoint& global_pos);

Q_SIGNALS:
	void excluded_changed(int column, const QStringList& excluded);

private Q_SLOTS:
	void on_item_changed();
	void on_clear_clicked();

private:
	QAction* title_action_;
	QListWidget* list_;
	QPushButton* clear_button_;
	int column_;
};


class View : public ViewBase, public MetadataObjObserverInterface,
	public GlobalSettingsInterface
{
	Q_OBJECT

public:
	explicit View(Session &session, bool is_main_view=false, QMainWindow *parent = nullptr);
	~View();

	virtual ViewType get_type() const;

	/**
	 * Resets the view to its default state after construction. It does however
	 * not reset the signal bases or any other connections with the session.
	 */
	virtual void reset_view_state();

	virtual void clear_decode_signals();
	virtual void add_decode_signal(shared_ptr<data::DecodeSignal> signal);
	virtual void remove_decode_signal(shared_ptr<data::DecodeSignal> signal);

	virtual void save_settings(QSettings &settings) const;
	virtual void restore_settings(QSettings &settings);

private:
	void reset_data();
	void update_data();

	void save_data_as_csv(unsigned int save_type) const;

	void update_row_count();

	QSet<int> filter_columns() const;

	void save_table_state() const;
	void restore_table_state();
	void apply_wanted_decoder();

private Q_SLOTS:
	void on_selected_decoder_changed(int index);
	void on_filter_text_changed(const QString& text);
	void on_view_mode_changed(int index);
	void on_colored_annotations_toggled(bool checked);

	void on_signal_name_changed(const QString &name);
	void on_signal_color_changed(const QColor &color);
	void on_new_annotations();

	void on_decoder_reset();
	void on_decoder_stacked(void* decoder);
	void on_decoder_removed(void* decoder);

	void on_actionSave_triggered(QAction* action = nullptr);

	void on_table_item_clicked(const QModelIndex& index);
	void on_table_item_double_clicked(const QModelIndex& index);
	void on_table_header_requested(const QPoint& pos);
	void on_table_header_toggled(bool checked);
	void on_table_header_filter_requested(int column,
		const QPoint& global_pos);
	void on_column_filter_changed(int column, const QStringList& excluded);

	virtual void on_metadata_object_changed(MetadataObject* obj,
		MetadataValueType value_type);
	virtual void on_setting_changed(const QString &key,
		const QVariant &value);

	virtual void capture_state_updated(int state);
	virtual void perform_delayed_view_update();

private:
	QWidget* parent_;

	QComboBox* decoder_selector_;
	QComboBox* view_mode_selector_;
	QLineEdit* row_filter_;
	QLabel* row_count_label_;
	QToolButton* colored_annotations_button_;

	SplitButton* save_button_;
	QAction* save_action_;

	CustomTableView* table_view_;
	FilterHeaderView* filter_header_;
	ColumnFilterPopup* filter_popup_;
	AnnotationCollectionModel* model_;
	CustomFilterProxyModel* filter_proxy_model_;

	data::DecodeSignal* signal_;
	const data::decode::Decoder* decoder_;

	/// Decoder to switch to as soon as it turns up in the selector.
	QString wanted_decoder_;
};

} // namespace tabular_decoder
} // namespace views
} // namespace pv

#endif // PULSEVIEW_PV_VIEWS_TABULAR_DECODER_VIEW_HPP
