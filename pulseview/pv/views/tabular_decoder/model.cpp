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

#include <QApplication>
#include <QDebug>
#include <QString>

#include "pv/views/tabular_decoder/view.hpp"

#include "view.hpp"

#include "pv/data/decode/decoder.hpp"
#include "pv/util.hpp"
#include "pv/globalsettings.hpp"

using std::make_shared;

using pv::util::Timestamp;
using pv::util::format_time_si;
using pv::util::format_time_minutes;
using pv::util::SIPrefix;

namespace pv {
namespace views {
namespace tabular_decoder {

// Checked for every cell that is drawn, so it allocates nothing.
static bool is_digit_in_radix(QChar c, int radix)
{
	const ushort u = c.unicode();

	if ((u >= '0') && (u <= '9'))
		return (u - '0') < radix;

	if (radix == 16) {
		const ushort upper = c.toUpper().unicode();
		return (upper >= 'A') && (upper <= 'F');
	}

	return false;
}

/*
 * The base a decoder writes bare numbers in. UART and its like have a
 * "format" option that says so ("dec", "hex", "oct", "bin"); the rest
 * write hex, which is what a dump reader expects anyway.
 */
static int decoder_radix(const pv::data::decode::Decoder *decoder)
{
	if (!decoder)
		return 16;

	const auto &options = decoder->options();
	const auto it = options.find("format");
	if ((it == options.end()) || !it->second)
		return 16;
	if (!g_variant_is_of_type(it->second, G_VARIANT_TYPE_STRING))
		return 16;

	const string format = g_variant_get_string(it->second, nullptr);
	if (format == "dec")
		return 10;
	if (format == "oct")
		return 8;
	if (format == "bin")
		return 2;

	return 16;  // "hex", "ascii" (which spells bytes as characters) and others
}

/*
 * The number a decoder's text is about. Decoders write it without a "0x"
 * in the base they were told to use ("48" from UART in hex mode), so the
 * base comes from the decoder: reading that "48" as decimal would make it
 * 0x30. Text without a number - "Start", "CRC error" - is shown as it is.
 */
static bool annotation_value(const QString &text, int radix, quint32 *value)
{
	const QString s = text.trimmed();

	// A byte written as the character itself: 'H'
	if ((s.size() == 3) && (s.at(0) == '\'') && (s.at(2) == '\'')) {
		*value = s.at(1).unicode();
		return true;
	}

	// Hex, with or without a label in front of it: "0x48", "DATA: 0x48"
	const int hex_at = s.indexOf(QStringLiteral("0x"), 0, Qt::CaseInsensitive);
	if (hex_at >= 0) {
		QString digits;
		for (int i = hex_at + 2; i < s.size(); i++) {
			const QChar c = s.at(i);
			if (!c.isLetterOrNumber())
				break;
			digits.append(c);
		}

		bool ok = false;
		const uint v = digits.toUInt(&ok, 16);
		if (ok && !digits.isEmpty()) {
			*value = v;
			return true;
		}

		// "0x" in the text means the number is hex, so a hex that cannot
		// be read leaves nothing to show in the other formats.
		return false;
	}

	// Hex mode always writes two digits, so a lone one is the character
	// itself: 0x41 arrives as "A", and hex would read that as 0x0A.
	if ((s.size() == 1) && (radix == 16)) {
		*value = s.at(0).unicode();
		return true;
	}

	// The whole text is one number: "48", "FF", "1010".
	{
		bool all_digits = !s.isEmpty();
		for (const QChar c : s)
			all_digits = all_digits && is_digit_in_radix(c, radix);

		if (all_digits) {
			bool ok = false;
			const uint v = s.toUInt(&ok, radix);
			if (ok) {
				*value = v;
				return true;
			}
		}
	}

	// A number behind a label: "DATA: 48", "[0A]".
	int i = 0;
	while ((i < s.size()) && !s.at(i).isDigit())
		i++;
	QString digits;
	for (; (i < s.size()) && is_digit_in_radix(s.at(i), radix); i++)
		digits.append(s.at(i));

	// A number that runs straight into a letter is not one in this base:
	// "1A2B" in decimal must not come out as 1.
	if (!digits.isEmpty() && (i < s.size()) && s.at(i).isLetterOrNumber())
		return false;

	bool ok = false;
	const uint v = digits.toUInt(&ok, radix);
	if (ok && !digits.isEmpty()) {
		*value = v;
		return true;
	}

	return false;
}

AnnotationCollectionModel::AnnotationCollectionModel(QObject* parent) :
	QAbstractTableModel(parent),
	all_annotations_(nullptr),
	dataset_(nullptr),
	signal_(nullptr),
	first_hidden_column_(0),
	prev_segment_(0),
	prev_last_row_(0),
	had_highlight_before_(false),
	colored_annotations_(true),
	value_format_(ValueRaw)
{
	// Note: when adding entries, consider ViewVisibleFilterProxyModel::filterAcceptsRow()

	uint8_t i = 0;
	header_data_.emplace_back(tr("Sample"));    i++; // Column #0
	header_data_.emplace_back(tr("Time"));      i++; // Column #1
	header_data_.emplace_back(tr("Decoder"));   i++; // Column #2
	header_data_.emplace_back(tr("Ann Row"));   i++; // Column #3
	header_data_.emplace_back(tr("Ann Class")); i++; // Column #4
	header_data_.emplace_back(tr("Value"));     i++; // Column #5

	first_hidden_column_ = i;
	header_data_.emplace_back(tr("End Sample"));         // Column #6, hidden
	header_data_.emplace_back(tr("Duration"));           // Column #7, hidden
}

int AnnotationCollectionModel::get_hierarchy_level(const Annotation* ann) const
{
	int level = 0;

	const unsigned int ann_stack_level = ann->row_data()->row()->decoder()->get_stack_level();
	level = (signal_->decoder_stack().size() - 1 - ann_stack_level);

	return level;
}

QVariant AnnotationCollectionModel::data_from_ann(const Annotation* ann, int index) const
{
	switch (index) {
	case 0: return QVariant((qulonglong)ann->start_sample());  // Column #0, Start Sample
	case 1: {                                                  // Column #1, Start Time
			Timestamp t = ann->start_sample() / signal_->get_samplerate();
			QString unit = signal_->get_samplerate() ? tr("s") : tr("sa");
			QString s;
			if ((t < 60) || (signal_->get_samplerate() == 0))  // i.e. if unit is sa
				s = format_time_si(t, SIPrefix::unspecified, 3, unit, false);
			else
				s = format_time_minutes(t, 3, false);
			return QVariant(s);
		}
	case 2: return QVariant(ann->row()->decoder()->name());    // Column #2, Decoder
	case 3: return QVariant(ann->row()->description());        // Column #3, Ann Row
	case 4: return QVariant(ann->ann_class_description());     // Column #4, Ann Class
	case 5: {                                                   // Column #5, Value
			const QString text = ann->longest_annotation();
			quint32 value = 0;

			// A bit row says 0 or 1 and nothing else: turned into another
			// format it would read as a number it is not.
			const QString trimmed = text.trimmed();
			if ((trimmed == "0") || (trimmed == "1"))
				return QVariant(text);

			if (value_format_ == ValueRaw)
				return QVariant(text);

			if (!annotation_value(text, decoder_radix(ann->row()->decoder()),
				&value))
				return QVariant(text);

			switch (value_format_) {
			case ValueHex:
				// 0x stays lowercase, the digits do not.
				return QVariant(QString("0x") +
					QString("%1").arg(value, 2, 16, QChar('0')).toUpper());
			case ValueDec:
				return QVariant(QString::number(value));
			case ValueOct:
				return QVariant(QString("0%1").arg(value, 3, 8, QChar('0')));
			case ValueBin:
				return QVariant(QString("%1").arg(value, 8, 2, QChar('0')));
			case ValueAscii:
				return QVariant(QString(QChar(
					((value >= 0x20) && (value <= 0x7E)) ? value : '.')));
			default:
				return QVariant(text);
			}
		}
	case 6: return QVariant((qulonglong)ann->end_sample());    // Column #6, End Sample
	case 7: {                                                  // Column #7, Duration
			// Without a sample rate the numbers are sample counts, the
			// same way the Start Time column reads.
			const double samplerate = signal_->get_samplerate();
			const uint64_t samples = ann->end_sample() - ann->start_sample();
			const Timestamp t = samplerate ? (samples / samplerate) : samples;
			const QString unit = samplerate ? tr("s") : tr("sa");
			QString s;
			if ((t < 60) || (samplerate == 0))
				s = format_time_si(t, SIPrefix::unspecified, 3, unit, false);
			else
				s = format_time_minutes(t, 3, false);
			return QVariant(s);
		}
	default: return QVariant();
	}
}

QVariant AnnotationCollectionModel::sort_key_from_ann(const Annotation* ann, int index) const
{
	switch (index) {
	case 0:  // Start Sample
	case 1:  // Start Time - ordered by the sample it was made from
		return QVariant((qulonglong)ann->start_sample());
	case 6:  // End Sample
		return QVariant((qulonglong)ann->end_sample());
	case 7:  // Duration
		return QVariant((qulonglong)(ann->end_sample() - ann->start_sample()));
	default:
		return data_from_ann(ann, index);
	}
}

QVariant AnnotationCollectionModel::data(const QModelIndex& index, int role) const
{
	if (!signal_ || !index.isValid() || !index.internalPointer())
		return QVariant();

	const Annotation* ann =
		static_cast<const Annotation*>(index.internalPointer());

	// The cells elide long text, so the tooltip carries the whole value.
	if ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole))
		return data_from_ann(ann, index.column());

	if (role == SortRole)
		return sort_key_from_ann(ann, index.column());

	// The row the mouse points at in the main view is drawn in the colour the
	// trace uses for this annotation, with the text flipped so it stays
	// readable on it. Columns left of a stacked decoder's level stay plain.
	const bool must_highlight = (highlight_sample_num_ > 0) &&
		((int64_t)ann->start_sample() <= highlight_sample_num_) &&
		((int64_t)ann->end_sample() >= highlight_sample_num_);
	const bool colored = colored_annotations_ &&
		(index.column() >= get_hierarchy_level(ann));

	if (role == Qt::ForegroundRole) {
		if (colored && must_highlight)
			return QApplication::palette().brush(
				GlobalSettings::current_theme_is_dark() ?
					QPalette::Window : QPalette::WindowText);

		return QApplication::palette().brush(QPalette::WindowText);
	}

	if (role == Qt::BackgroundRole) {
		if (colored)
			return QBrush(must_highlight ? ann->color() :
				(GlobalSettings::current_theme_is_dark() ?
					ann->dark_color() : ann->bright_color()));
	}

	return QVariant();
}

Qt::ItemFlags AnnotationCollectionModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

void AnnotationCollectionModel::set_colored_annotations(bool colored)
{
	colored_annotations_ = colored;
}

void AnnotationCollectionModel::set_value_format(int format)
{
	if (format == value_format_)
		return;

	value_format_ = format;

	// The whole column changes, and so does what a filter matches on it.
	if (dataset_ && !dataset_->empty())
		dataChanged(index(0, 5), index(dataset_->size() - 1, 5));
}

uint8_t AnnotationCollectionModel::first_hidden_column() const
{
	return first_hidden_column_;
}

bool AnnotationCollectionModel::is_filterable_column(int column)
{
	// Decoder, Ann Row and Ann Class, see the column comments above.
	return (column == 2) || (column == 3) || (column == 4);
}

bool AnnotationCollectionModel::is_text_column(int column)
{
	// Decoder, Ann Row, Ann Class and Value, see the column comments.
	return (column >= 2) && (column <= 5);
}

QVariant AnnotationCollectionModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if ((section < 0) || (section >= (int)header_data_.size()))
		return QVariant();

	if (orientation == Qt::Horizontal) {
		if (role == Qt::DisplayRole)
			return header_data_[section];

		// The funnel at the end of such a column is a small mark, so
		// the whole section says what it is for.
		if ((role == Qt::ToolTipRole) && is_filterable_column(section))
			return tr("Filter this column");
	}

	return QVariant();
}

QModelIndex AnnotationCollectionModel::index(int row, int column,
	const QModelIndex& parent_idx) const
{
	(void)parent_idx;
	assert(column >= 0);

	if (!dataset_ || (row < 0))
		return QModelIndex();

	QModelIndex idx;

	if ((size_t)row < dataset_->size())
		idx = createIndex(row, column, (void*)dataset_->at(row));

	return idx;
}

QModelIndex AnnotationCollectionModel::parent(const QModelIndex& index) const
{
	(void)index;

	return QModelIndex();
}

int AnnotationCollectionModel::rowCount(const QModelIndex& parent_idx) const
{
	(void)parent_idx;

	if (!dataset_)
		return 0;

	return dataset_->size();
}

int AnnotationCollectionModel::columnCount(const QModelIndex& parent_idx) const
{
	(void)parent_idx;

	return header_data_.size();
}

void AnnotationCollectionModel::set_signal_and_segment(data::DecodeSignal* signal, uint32_t current_segment)
{
	layoutAboutToBeChanged();

	if (!signal) {
		all_annotations_ = nullptr;
		dataset_ = nullptr;
		signal_ = nullptr;

		dataChanged(QModelIndex(), QModelIndex());
		layoutChanged();
		return;
	}

	if (signal_)
		for (const shared_ptr<Decoder>& dec : signal_->decoder_stack())
			disconnect(dec.get(), nullptr, this, SLOT(on_annotation_visibility_changed()));

	all_annotations_ = signal->get_all_annotations_by_segment(current_segment);
	signal_ = signal;

	for (const shared_ptr<Decoder>& dec : signal_->decoder_stack())
		connect(dec.get(), SIGNAL(annotation_visibility_changed()),
			this, SLOT(on_annotation_visibility_changed()));

	// The table follows the rows and classes that were switched off
	// in the main view, so the filtered set is the one on show.
	update_annotations_without_hidden();
	dataset_ = &all_annotations_without_hidden_;

	if (!dataset_ || dataset_->empty()) {
		// An empty set is a change like any other: without telling the
		// view, it keeps showing what the previous segment held.
		if ((prev_segment_ != current_segment) || (prev_last_row_ != 0)) {
			dataChanged(QModelIndex(), QModelIndex());
			layoutChanged();
		}

		prev_segment_ = current_segment;
		prev_last_row_ = 0;
		return;
	}

	const size_t new_row_count = dataset_->size() - 1;

	// Force the view associated with this model to update when the segment changes
	if (prev_segment_ != current_segment) {
		dataChanged(index(0, 0), index(new_row_count, 0));
		layoutChanged();
	} else {
		// Force the view associated with this model to update when we have more annotations
		if (prev_last_row_ < new_row_count) {
			dataChanged(index(prev_last_row_, 0), index(new_row_count, 0));
			layoutChanged();
		}
	}

	prev_segment_ = current_segment;
	prev_last_row_ = new_row_count;
}

void AnnotationCollectionModel::update_annotations_without_hidden()
{
	uint64_t count = 0;

	if (!all_annotations_ || all_annotations_->empty()) {
		all_annotations_without_hidden_.clear();
		return;
	}

	for (const Annotation* ann : *all_annotations_) {
		if (!ann->visible())
			continue;

		if (all_annotations_without_hidden_.size() < (count + 100))
			all_annotations_without_hidden_.resize(count + 100);

		all_annotations_without_hidden_[count++] = ann;
	}

	all_annotations_without_hidden_.resize(count);
}

QModelIndex AnnotationCollectionModel::update_highlighted_rows(QModelIndex first,
	QModelIndex last, int64_t sample_num)
{
	bool has_highlight = false;
	QModelIndex result;

	highlight_sample_num_ = sample_num;

	if (!dataset_ || dataset_->empty())
		return result;

	if (sample_num >= 0) {
		last = last.sibling(last.row() + 1, 0);

		// Check if there are any annotations visible in the table view that
		// we would need to highlight - only then do we do so
		QModelIndex index = first;
		do {
			const Annotation* ann =	static_cast<const Annotation*>(index.internalPointer());
			if (!ann)  // Can happen if the table is being modified at this exact time
				return result;

			if (((int64_t)ann->start_sample() <= sample_num) &&
				((int64_t)ann->end_sample() >= sample_num)) {
				result = index;
				has_highlight = true;
				break;
			}

			index = index.sibling(index.row() + 1, 0);
		} while (index != last);
	}

	if (has_highlight || had_highlight_before_)
		dataChanged(first, last);

	had_highlight_before_ = has_highlight;

	return result;
}

void AnnotationCollectionModel::on_annotation_visibility_changed()
{
	layoutAboutToBeChanged();

	update_annotations_without_hidden();

	if (dataset_)
		dataChanged(index(0, 0), index(dataset_->size() - 1, 0));
	else
		dataChanged(QModelIndex(), QModelIndex());

	layoutChanged();
}

} // namespace tabular_decoder
} // namespace views
} // namespace pv
