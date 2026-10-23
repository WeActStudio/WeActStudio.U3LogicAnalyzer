/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on Pulseview.
 *
 * Copyright (C) 2015 Victor Anjin <virinext@gmail.com>
 * Copyright (C) 2019 Soeren Apel <soeren@apelpie.net>
 * Copyright (C) 2026 Q2H2
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <limits>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFont>
#include <QKeyEvent>
#include <QScrollBar>
#include <QSize>
#include <QString>
#include <QPainter>
#include <QPaintEvent>

#include "QHexView.hpp"

using std::make_pair;

const unsigned int BYTES_PER_LINE   = 16;
const unsigned int GAP_ADR_HEX      = 10;
const unsigned int GAP_HEX_TEXT     = 10;
const unsigned int GAP_TEXT_SLIDER  = 5;


QHexView::QHexView(QWidget *parent):
	QAbstractScrollArea(parent),
	mode_(ChunkedDataMode),
	format_(FormatAscii),
	data_(nullptr),
	data_size_(0),
	posAddr_(0),
	posHex_(0),
	posText_(0),
	charWidth_(0),
	charHeight_(0),
	selectBegin_(0),
	selectEnd_(0),
	selectInit_(0),
	cursorPos_(0),
	select_area_(AreaBytes),
	address_digits_(1),
	mouse_pressed_(false),
	current_chunk_id_(0),
	current_chunk_offset_(0),
	current_offset_(0),
	current_chunk_sample_(0),
	next_chunk_sample_(0),
	visible_range_(0, 0),
	highlighted_sample_(std::numeric_limits<uint64_t>::max()),
	theme_dark_(false)
{
	setFont(QFont("Courier", 10));

	charWidth_ = fontMetrics().boundingRect('X').width();
	charHeight_ = fontMetrics().height();
	setFocusPolicy(Qt::StrongFocus);

	update_theme_colors();
}

/*
 * The chunk colors and the band that marks the range the main view shows
 * belong to the scheme: painted with the bright ones on a dark view, the
 * band is nearly white and the text inside it is lost. The palette is read
 * again on every paint, because the theme can be switched while a hex dump
 * is open - decided once in the constructor, the colors stayed as they were.
 */
void QHexView::update_theme_colors()
{
	const bool dark = (palette().color(QPalette::Base).toHsv().value() < 128);
	if (!chunk_colors_.empty() && (dark == theme_dark_))
		return;

	theme_dark_ = dark;
	chunk_colors_.clear();

	if (dark) {
		chunk_colors_.emplace_back(255, 165, 0);
		chunk_colors_.emplace_back(0, 191, 255);
		chunk_colors_.emplace_back(255, 105, 180);
		visible_range_color_ = QColor("#2f5478");
	} else {
		chunk_colors_.emplace_back(100, 149, 237); // QColorConstants::Svg::cornflowerblue
		chunk_colors_.emplace_back(60, 179, 113);  // QColorConstants::Svg::mediumseagreen
		chunk_colors_.emplace_back(210, 180, 140); // QColorConstants::Svg::tan
		visible_range_color_ = QColor("#cfe0f5");
	}
}

void QHexView::set_mode(Mode m)
{
	mode_ = m;

	// This is not expected to be set when data is showing,
	// so we don't update the viewport here
}

void QHexView::set_display_format(DisplayFormat format)
{
	if (format == format_)
		return;

	format_ = format;
	update_layout();
	viewport()->update();
}

/* Width of one cell in either column, and of a cell plus the gap that
 * follows it. The hit test, the cursor and the width of the whole view are
 * all built on these, so a format is added in one place. */
unsigned int QHexView::byte_cell_chars() const
{
	return 2;  // "66"
}

unsigned int QHexView::byte_cell_step() const
{
	return 3;
}

unsigned int QHexView::text_cell_chars() const
{
	switch (format_) {
	case FormatAscii:   return 1;
	case FormatHex:     return 2;
	case FormatDec:
	case FormatOct:     return 3;
	case FormatBin:     return 8;
	case FormatCArray:  return 5;  // "0x66,"
	}

	return 1;
}

unsigned int QHexView::text_cell_step() const
{
	// The C array already ends in a comma, which separates the cells.
	return (format_ == FormatCArray) ? text_cell_chars() : (text_cell_chars() + 1);
}

/* The column on the left is the dump itself, and stays hex without 0x in
 * every format - the other formats only change the column on the right. */
QString QHexView::byte_text(uint8_t value) const
{
	return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
}

QString QHexView::text_text(uint8_t value) const
{
	switch (format_) {
	case FormatAscii:
		return QString(QChar(((value < 0x20) || (value > 0x7E)) ? '.' : value));
	case FormatHex:
		return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
	case FormatDec:
		return QString("%1").arg(value, 3, 10, QChar(' '));
	case FormatOct:
		return QString("%1").arg(value, 3, 8, QChar('0'));
	case FormatBin:
		return QString("%1").arg(value, 8, 2, QChar('0'));
	case FormatCArray:
		return QString("0x") + QString("%1,").arg(value, 2, 16, QChar('0')).toUpper();
	}

	return QString();
}

/* The two areas of a line start where the one before ends, and both the
 * address width and the cell width decide that. */
void QHexView::update_layout()
{
	posAddr_ = 0;
	posHex_  = address_digits_ * charWidth_ + GAP_ADR_HEX;
	posText_ = posHex_ + BYTES_PER_LINE * byte_cell_step() * charWidth_ + GAP_HEX_TEXT;
}

void QHexView::set_data(const DecodeBinaryClass* data)
{
	data_ = data;

	size_t size = 0;
	if (data) {
		size_t chunks = data_->chunks.size();
		for (size_t i = 0; i < chunks; i++)
			size += data_->chunks[i].data.size();
	}
	data_size_ = size;
	address_digits_ = (uint8_t)QString::number(data_size_, 16).length();

	update_layout();

	viewport()->update();
}

void QHexView::set_visible_sample_range(uint64_t start, uint64_t end)
{
	visible_range_ = make_pair(start, end);

	viewport()->update();
}

void QHexView::set_highlighted_data_sample(uint64_t sample)
{
	highlighted_sample_ = sample;

	viewport()->update();
}

unsigned int QHexView::get_bytes_per_line() const
{
	return BYTES_PER_LINE;
}

void QHexView::clear()
{
	verticalScrollBar()->setValue(0);
	data_ = nullptr;
	data_size_ = 0;

	highlighted_sample_ = std::numeric_limits<uint64_t>::max();

	viewport()->update();
}

QSizePolicy QHexView::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

QSize QHexView::minimumSizeHint() const
{
	return QSize(10, 10);
}

QSize QHexView::sizeHint() const
{
	return minimumSizeHint();
}

pair<size_t, size_t> QHexView::get_selection() const
{
	size_t start = selectBegin_ / 2;
	size_t end = selectEnd_ / 2;

	if (start == end) {
		// Nothing is currently selected
		start = 0;
		end = data_size_;
	} if (end < data_size_)
		end++;

	return std::make_pair(start, end);
}

size_t QHexView::create_hex_line(size_t start, size_t end, QString* dest,
	bool with_offset, bool with_ascii)
{
	dest->clear();

	// Determine start address for the row
	uint64_t row = start / BYTES_PER_LINE;
	uint64_t offset = row * BYTES_PER_LINE;
	end = std::min((uint64_t)end, offset + BYTES_PER_LINE);

	if (with_offset)
		dest->append(QString("%1 ").arg(row * BYTES_PER_LINE, address_digits_, 16, QChar('0')).toUpper());

	initialize_byte_iterator(offset);
	for (size_t i = offset; i < offset + BYTES_PER_LINE; i++) {
		uint8_t value = 0;

		if (i < end)
			value = get_next_byte();

		if ((i < start) || (i >= end))
			dest->append("   ");
		else
			dest->append(QString("%1 ").arg(value, 2, 16, QChar('0')).toUpper());
	}

	if (with_ascii) {
		initialize_byte_iterator(offset);
		for (size_t i = offset; i < end; i++) {
			uint8_t value = get_next_byte();

			if ((value < 0x20) || (value > 0x7E))
				value = '.';

			if (i < start)
				dest->append(' ');
			else
				dest->append((char)value);
		}
	}

	return end;
}

void QHexView::initialize_byte_iterator(size_t offset)
{
	current_chunk_id_ = 0;
	current_chunk_offset_ = 0;
	current_offset_ = offset;

	size_t chunks = data_->chunks.size();
	for (size_t i = 0; i < chunks; i++) {
		size_t size = data_->chunks[i].data.size();

		if (offset >= size) {
			current_chunk_id_++;
			offset -= size;
		} else {
			current_chunk_offset_ = offset;
			break;
		}
	}

	if (current_chunk_id_ < data_->chunks.size())
		current_chunk_ = data_->chunks[current_chunk_id_];

	current_chunk_sample_ = current_chunk_.sample;

	// Obtain sample of next chunk if there is one
	if ((current_chunk_id_ + 1) < data_->chunks.size())
		next_chunk_sample_ = data_->chunks[current_chunk_id_ + 1].sample;
	else
		next_chunk_sample_ = std::numeric_limits<uint64_t>::max();
}

uint8_t QHexView::get_next_byte(bool* is_new_chunk)
{
	if (is_new_chunk != nullptr)
		*is_new_chunk = (current_chunk_offset_ == 0);

	uint8_t v = 0;
	if (current_chunk_offset_ < current_chunk_.data.size())
		v = current_chunk_.data[current_chunk_offset_];

	current_chunk_sample_ = current_chunk_.sample;

	if (is_new_chunk) {
		// Obtain sample of next chunk if there is one
		if ((current_chunk_id_ + 1) < data_->chunks.size())
			next_chunk_sample_ = data_->chunks[current_chunk_id_ + 1].sample;
		else
			next_chunk_sample_ = std::numeric_limits<uint64_t>::max();
	}

	current_offset_++;
	current_chunk_offset_++;

	if (current_offset_ > data_size_) {
		qWarning() << "QHexView::get_next_byte() overran binary data boundary:" <<
			current_offset_ << "of" << data_size_ << "bytes";
		return 0xEE;
	}

	if ((current_chunk_offset_ == current_chunk_.data.size()) && (current_offset_ < data_size_)) {
		current_chunk_id_++;
		current_chunk_offset_ = 0;
		current_chunk_ = data_->chunks[current_chunk_id_];
	}

	return v;
}

QSize QHexView::getFullSize() const
{
	size_t width = posText_ + (BYTES_PER_LINE * text_cell_step() * charWidth_);
	if (verticalScrollBar()->isEnabled())
		width += GAP_TEXT_SLIDER + verticalScrollBar()->width();
	if (!data_ || (data_size_ == 0) || (data_->chunks.empty()))
		width = std::min(width, (size_t)10);
	if (!data_ || (data_size_ == 0))
		return QSize(width, 0);

	size_t height = data_size_ / BYTES_PER_LINE;

	if (data_size_ % BYTES_PER_LINE)
		height++;

	height *= charHeight_;
	return QSize(width, height);
}

void QHexView::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());

	update_theme_colors();

	QFont normal_font = painter.font();
	QFont bold_font = painter.font();
	bold_font.setWeight(QFont::Bold);

	bool bold_font_was_used = false;

	// Calculate and update the widget and paint area sizes
	QSize widgetSize = getFullSize();
	setMinimumWidth(10);
	QSize areaSize = viewport()->size() - QSize(0, charHeight_);

	// Only show scrollbar if the content goes beyond the visible area
	if (widgetSize.height() > areaSize.height()) {
		verticalScrollBar()->setEnabled(true);
		verticalScrollBar()->setPageStep(areaSize.height() / charHeight_);
		verticalScrollBar()->setRange(0, ((widgetSize.height() - areaSize.height())) / charHeight_ + 1);
	} else
		verticalScrollBar()->setEnabled(false);

	// A line of eight-bit groups is wider than any view, so the content
	// scrolls sideways in the same way it scrolls down.
	// The address column is frozen, so only the part of the view right of
	// it can show data, and the scroll has to reach that much further.
	QScrollBar *hbar = horizontalScrollBar();
	const int page_width =
		std::max(1, viewport()->width() - (int)posHex_);
	if (widgetSize.width() > page_width) {
		hbar->setRange(0, widgetSize.width() - page_width);
		hbar->setPageStep(page_width);
		hbar->setEnabled(true);
	} else {
		hbar->setRange(0, 0);
		hbar->setEnabled(false);
	}

	const int scroll_x = hbar->value();

	// Fill widget background
	painter.fillRect(event->rect(), palette().color(QPalette::Base));

	if (!data_ || (data_size_ == 0) || (data_->chunks.empty())) {
		painter.setPen(palette().color(QPalette::Text));
		QString s = tr("No data available");
		int x = (areaSize.width() - fontMetrics().boundingRect(s).width()) / 2;
		int y = areaSize.height() / 2;
		painter.drawText(x, y, s);
		return;
	}

	// Determine first/last line indices
	size_t firstLineIdx = verticalScrollBar()->value();

	size_t lastLineIdx = firstLineIdx + (areaSize.height() / charHeight_);
	if (lastLineIdx > (data_size_ / BYTES_PER_LINE)) {
		lastLineIdx = data_size_ / BYTES_PER_LINE;
		if (data_size_ % BYTES_PER_LINE)
			lastLineIdx++;
	}

	// The data scrolls sideways; the address column stays where it is and
	// is painted last, over whatever slid underneath it.
	painter.save();
	// Clip first, or the data slides in under the address column and shows
	// through the gap to its left.
	painter.setClipRect(QRect((int)posHex_, 0,
		viewport()->width() - (int)posHex_, height()), Qt::IntersectClip);
	painter.translate(-scroll_x, 0);

	// Paint divider line between the two columns
	const int line_x = (int)(posText_ - (GAP_HEX_TEXT / 2));
	painter.setPen(palette().color(QPalette::Midlight));
	painter.drawLine(line_x, event->rect().top(), line_x, height());

	// The head of the line stops at the divider, which it would otherwise
	// cover where it is drawn.
	painter.fillRect(QRect((int)posHex_, event->rect().top(),
		line_x - (int)posHex_, charHeight_ + 2),
		palette().color(QPalette::Window));

	// Paint top row with the byte numbers of the line
	painter.setPen(palette().color(QPalette::ButtonText));
	for (int offset = 0; offset <= 0xF; offset++)
		painter.drawText((int)(posHex_ + (offset * byte_cell_step() + byte_cell_chars() - 1) * charWidth_),
			charHeight_ - 3, QString::number(offset, 16).toUpper());

	// Paint hex values
	QBrush regular_brush = palette().buttonText();
	QBrush selected_brush = palette().highlight();
	QBrush visible_range_brush = QBrush(visible_range_color_);

	bool multiple_chunks = (data_->chunks.size() > 1);
	unsigned int chunk_color = 0;

	initialize_byte_iterator(firstLineIdx * BYTES_PER_LINE);

	int yStart = 2 * charHeight_;
	for (size_t lineIdx = firstLineIdx, y = yStart; lineIdx < lastLineIdx; lineIdx++) {

		int x = posHex_;
		for (size_t i = 0; (i < BYTES_PER_LINE) && (current_offset_ < data_size_); i++) {
			size_t pos = (lineIdx * BYTES_PER_LINE + i) * 2;

			// Fetch byte
			bool is_new_chunk;
			uint8_t byte_value = get_next_byte(&is_new_chunk);

			if (is_new_chunk) {
				chunk_color++;
				if (chunk_color == chunk_colors_.size())
					chunk_color = 0;

				// New chunk means also new chunk sample, so check for required changes
				if (bold_font_was_used)
					painter.setFont(normal_font);
				if ((highlighted_sample_ >= current_chunk_sample_) && (highlighted_sample_ < next_chunk_sample_)) {
					painter.setFont(bold_font);
					bold_font_was_used = true;
				}
			}

			// Restore default paint style
			painter.setBackground(regular_brush);
			painter.setBackgroundMode(Qt::TransparentMode);
			if (!multiple_chunks)
				painter.setPen(palette().color(QPalette::Text));
			else
				painter.setPen(chunk_colors_[chunk_color]);

			// Highlight needed because it's the range visible in main view?
			if ((current_chunk_sample_ >= visible_range_.first) && (current_chunk_sample_ < visible_range_.second)) {
				painter.setBackgroundMode(Qt::OpaqueMode);
				painter.setBackground(visible_range_brush);
			}

			// Highlight for selection range needed? (takes priority over visible range highlight)
			if ((select_area_ == AreaBytes) && (pos >= selectBegin_) && (pos < selectEnd_)) {
				painter.setBackgroundMode(Qt::OpaqueMode);
				painter.setBackground(selected_brush);
				painter.setPen(palette().color(QPalette::HighlightedText));
			}

			// The byte, always hex without 0x
			painter.drawText(x, y, byte_text(byte_value));

			x += byte_cell_step() * charWidth_;
		}

		y += charHeight_;
	}

	// Paint the column on the right, in the format that was chosen
	initialize_byte_iterator(firstLineIdx * BYTES_PER_LINE);
	yStart = 2 * charHeight_;
	for (size_t lineIdx = firstLineIdx, y = yStart; lineIdx < lastLineIdx; lineIdx++) {

		int x = posText_;
		for (size_t i = 0; (i < BYTES_PER_LINE) && (current_offset_ < data_size_); i++) {
			// Fetch byte
			bool is_new_chunk;
			uint8_t ch = get_next_byte(&is_new_chunk);

			if (is_new_chunk) {
				// New chunk means also new chunk sample, so check for required changes
				if (bold_font_was_used)
					painter.setFont(normal_font);
				if ((highlighted_sample_ >= current_chunk_sample_) && (highlighted_sample_ < next_chunk_sample_)) {
					painter.setFont(bold_font);
					bold_font_was_used = true;
				}
			}

			// Restore default paint style
			painter.setBackgroundMode(Qt::TransparentMode);
			painter.setBackground(regular_brush);
			painter.setPen(palette().color(QPalette::Text));

			// Highlight needed because it's the range visible in main view?
			if ((current_chunk_sample_ >= visible_range_.first) && (current_chunk_sample_ < visible_range_.second)) {
				painter.setBackgroundMode(Qt::OpaqueMode);
				painter.setBackground(visible_range_brush);
			}

			// Highlight for selection range needed? (takes priority over visible range highlight)
			size_t pos = (lineIdx * BYTES_PER_LINE + i) * 2;
			if ((select_area_ == AreaText) && (pos >= selectBegin_) && (pos < selectEnd_)) {
				painter.setBackgroundMode(Qt::OpaqueMode);
				painter.setBackground(selected_brush);
				painter.setPen(palette().color(QPalette::HighlightedText));
			}

			painter.drawText(x, y, text_text(ch));
			x += text_cell_step() * charWidth_;
		}

		y += charHeight_;
	}

	// Restore painter defaults
	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setBackground(regular_brush);

	// Paint cursor
	if (hasFocus()) {
		const int byte_in_line = (int)((cursorPos_ / 2) % BYTES_PER_LINE);
		const int line = (int)((cursorPos_ / 2) / BYTES_PER_LINE - firstLineIdx);
		// The caret marks the cell the selection is in.
		const int cursorX = (select_area_ == AreaText)
			? (int)(posText_ + (byte_in_line * text_cell_step() + text_cell_chars() - 1) * charWidth_)
			: (int)(posHex_ + byte_in_line * byte_cell_step() * charWidth_);
		const int cursorY = charHeight_ + line * charHeight_ + 4;
		painter.fillRect(cursorX, cursorY, 2, charHeight_, palette().color(QPalette::WindowText));
	}

	painter.restore();

	// The address column, drawn last so it covers the data that scrolled
	// under it. Only the data moves, as with a frozen column in a sheet.
	painter.fillRect(QRect(posAddr_, event->rect().top(),
		posHex_ - (GAP_ADR_HEX / 2), height()), palette().color(QPalette::Window));
	painter.setPen(palette().color(QPalette::ButtonText));

	for (size_t lineIdx = firstLineIdx, y = 2 * charHeight_;
			lineIdx < lastLineIdx; lineIdx++) {
		QString address = QString("%1").arg(lineIdx * 16, address_digits_, 16,
			QChar('0')).toUpper();
		painter.drawText(posAddr_, y, address);
		y += charHeight_;
	}
}

void QHexView::keyPressEvent(QKeyEvent *event)
{
	bool setVisible = false;

	// Cursor movements
	if (event->matches(QKeySequence::MoveToNextChar)) {
		setCursorPos(cursorPos_ + 1);
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToPreviousChar)) {
		setCursorPos(cursorPos_ - 1);
		resetSelection(cursorPos_);
		setVisible = true;
	}

	if (event->matches(QKeySequence::MoveToEndOfLine)) {
		setCursorPos(cursorPos_ | ((BYTES_PER_LINE * 2) - 1));
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToStartOfLine)) {
		setCursorPos(cursorPos_ | (cursorPos_ % (BYTES_PER_LINE * 2)));
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToPreviousLine)) {
		setCursorPos(cursorPos_ - BYTES_PER_LINE * 2);
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToNextLine)) {
		setCursorPos(cursorPos_ + BYTES_PER_LINE * 2);
		resetSelection(cursorPos_);
		setVisible = true;
	}

	if (event->matches(QKeySequence::MoveToNextPage)) {
		setCursorPos(cursorPos_ + (viewport()->height() / charHeight_ - 1) * 2 * BYTES_PER_LINE);
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToPreviousPage)) {
		setCursorPos(cursorPos_ - (viewport()->height() / charHeight_ - 1) * 2 * BYTES_PER_LINE);
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToEndOfDocument)) {
		setCursorPos(data_size_ * 2);
		resetSelection(cursorPos_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::MoveToStartOfDocument)) {
		setCursorPos(0);
		resetSelection(cursorPos_);
		setVisible = true;
	}

	// Select commands
	if (event->matches(QKeySequence::SelectAll)) {
		resetSelection(0);
		setSelection(2 * data_size_);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectNextChar)) {
		int pos = cursorPos_ + 1;
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectPreviousChar)) {
		int pos = cursorPos_ - 1;
		setSelection(pos);
		setCursorPos(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectEndOfLine)) {
		int pos = cursorPos_ - (cursorPos_ % (2 * BYTES_PER_LINE)) + (2 * BYTES_PER_LINE);
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectStartOfLine)) {
		int pos = cursorPos_ - (cursorPos_ % (2 * BYTES_PER_LINE));
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectPreviousLine)) {
		int pos = cursorPos_ - (2 * BYTES_PER_LINE);
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectNextLine)) {
		int pos = cursorPos_ + (2 * BYTES_PER_LINE);
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}

	if (event->matches(QKeySequence::SelectNextPage)) {
		int pos = cursorPos_ + (((viewport()->height() / charHeight_) - 1) * 2 * BYTES_PER_LINE);
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectPreviousPage)) {
		int pos = cursorPos_ - (((viewport()->height() / charHeight_) - 1) * 2 * BYTES_PER_LINE);
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectEndOfDocument)) {
		int pos = data_size_ * 2;
		setCursorPos(pos);
		setSelection(pos);
		setVisible = true;
	}
	if (event->matches(QKeySequence::SelectStartOfDocument)) {
		setCursorPos(0);
		setSelection(0);
		setVisible = true;
	}

	if (event->matches(QKeySequence::Copy) && (data_))
		copy_selection();

	if (setVisible)
		ensureVisible();

	viewport()->update();
}

/* What the selection holds, written the way the column it was made in
 * writes it: the left one copies hex digits, the right one whatever format
 * was chosen for it. */
void QHexView::copy_selection()
{
	QString text;
	const bool as_chars = (select_area_ == AreaText) && (format_ == FormatAscii);

	initialize_byte_iterator(selectBegin_ / 2);

	size_t selectedSize = (selectEnd_ - selectBegin_ + 1) / 2;
	for (size_t i = 0; i < selectedSize; i++) {
		uint8_t byte_value = get_next_byte();

		// Decimal cells are padded so the column lines up; the padding is
		// not part of the value. Other formats have none, and a space byte
		// must stay a space.
		const bool padded = (select_area_ == AreaText) && (format_ == FormatDec);
		const QString cell = (select_area_ == AreaText) ?
			text_text(byte_value) : byte_text(byte_value);
		text += padded ? cell.trimmed() : cell;

		// Characters are copied as they are; digits are separated by the
		// space the two columns already show between cells.
		if (i % BYTES_PER_LINE == (BYTES_PER_LINE - 1))
			text += "\n";
		else if ((select_area_ == AreaBytes) ||
			(!as_chars && (format_ != FormatCArray)))
			text += " ";
	}

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text, QClipboard::Clipboard);
	if (clipboard->supportsSelection())
		clipboard->setText(text, QClipboard::Selection);
}

void QHexView::mouseMoveEvent(QMouseEvent *event)
{
	if (mouse_pressed_) {
		int actPos = cursorPosFromMousePos(event->pos());
		setCursorPos(actPos);
		setSelection(actPos);
		viewport()->update();
	}
}

void QHexView::mousePressEvent(QMouseEvent *event)
{
	mouse_pressed_ = true;

	bool on_text = false;
	bool on_bytes = false;
	int cPos = cursorPosFromMousePos(event->pos(), &on_text, &on_bytes);
	select_area_ = on_text ? AreaText : AreaBytes;

	const bool shift =
		(QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0;
	if (shift && (event->button() == Qt::LeftButton))
		setSelection(cPos);
	else
		resetSelection(cPos);

	setCursorPos(cPos);

	// A plain left click on a cell says which byte is meant, so the trace
	// can be moved there; extending a selection or a right click does not.
	if (on_bytes && !shift && (event->button() == Qt::LeftButton)) {
		const pair<uint64_t, uint64_t> range =
			byte_sample_range((size_t)(cPos / 2));
		if (range.second > range.first)
			Q_EMIT byte_selected(range.first, range.second);
	}

	viewport()->update();
}

void QHexView::mouseReleaseEvent(QMouseEvent *event)
{
	(void)event;
	mouse_pressed_ = false;
}

size_t QHexView::cursorPosFromMousePos(const QPoint &position, bool *on_text,
	bool *on_bytes)
{
	const size_t byte_cell = byte_cell_step() * charWidth_;
	const size_t text_cell = text_cell_step() * charWidth_;

	// The address column stays put and the data moves, so only the data
	// part is shifted by the scroll position. A point left of it - the
	// address of the line, or outside the view - belongs to the first
	// byte: a click always lands on a byte, but only a cell counts as one
	// being pointed at.
	size_t x = 0;
	bool text = false;
	bool bytes = false;

	if (position.x() >= (int)posHex_) {
		const size_t data_x =
			(size_t)position.x() + (size_t)horizontalScrollBar()->value();

		if (data_x >= posText_) {
			x = (data_x - posText_) / text_cell;
			text = true;
		} else {
			x = (data_x - posHex_) / byte_cell;
			bytes = true;
		}
	}

	if (x >= BYTES_PER_LINE)
		x = BYTES_PER_LINE - 1;

	if (on_text)
		*on_text = text;
	if (on_bytes)
		*on_bytes = bytes;

	const int line = (int)(position.y() / (int)charHeight_) - 1;
	const size_t y = (line < 0) ? 0 : ((size_t)line * 2 * BYTES_PER_LINE);
	const size_t firstLineIdx = verticalScrollBar()->value();
	const size_t pos = x * 2 + y + firstLineIdx * BYTES_PER_LINE * 2;

	return std::min(pos, data_size_ * 2);
}

void QHexView::resetSelection()
{
	selectBegin_ = selectInit_;
	selectEnd_ = selectInit_;
}

void QHexView::resetSelection(int pos)
{
	if (pos < 0)
		pos = 0;

	selectInit_ = pos;
	selectBegin_ = pos;
	selectEnd_ = pos;
}

void QHexView::setSelection(int pos)
{
	if (pos < 0)
		pos = 0;

	if ((size_t)pos >= selectInit_) {
		selectEnd_ = pos;
		selectBegin_ = selectInit_;
	} else {
		selectBegin_ = pos;
		selectEnd_ = selectInit_;
	}
}

void QHexView::setCursorPos(int position)
{
	if (position < 0)
		position = 0;

	int max_pos = data_size_ * 2;

	if (position > max_pos)
		position = max_pos;

	cursorPos_ = position;
}

void QHexView::ensureVisible()
{
	QSize areaSize = viewport()->size();

	int firstLineIdx = verticalScrollBar()->value();
	int lastLineIdx = firstLineIdx + areaSize.height() / charHeight_;

	int cursorY = cursorPos_ / (2 * BYTES_PER_LINE);

	if (cursorY < firstLineIdx)
		verticalScrollBar()->setValue(cursorY);
	else
		if(cursorY >= lastLineIdx)
			verticalScrollBar()->setValue(cursorY - areaSize.height() / charHeight_ + 1);

	// Both columns can be wider than the view, so the caret is kept in
	// sight sideways as well. The address column is in the way on the
	// left, so the caret only has to come out from behind it.
	{
		const bool in_text = (select_area_ == AreaText);
		const int cell = (int)((in_text ? text_cell_step() : byte_cell_step()) *
			charWidth_);
		const int x = (int)((in_text ? posText_ : posHex_) +
			((cursorPos_ / 2) % BYTES_PER_LINE) * cell);
		const int left = horizontalScrollBar()->value() + (int)posHex_;
		const int right = horizontalScrollBar()->value() + viewport()->width();
		if (x < left)
			horizontalScrollBar()->setValue(
				std::max(0, x - (int)posHex_));
		else if ((x + cell) > right)
			horizontalScrollBar()->setValue(x + cell - viewport()->width());
	}
}

/* The samples a byte covers. A chunk carries the bytes of one annotation
 * and starts at that annotation's sample, so the bytes split the span up to
 * the next chunk between them. */
pair<uint64_t, uint64_t> QHexView::byte_sample_range(size_t offset) const
{
	if (!data_ || data_->chunks.empty())
		return make_pair<uint64_t, uint64_t>(0, 0);

	// Decoders hand out one chunk per byte, so the last one has no chunk
	// after it to measure against; it keeps the width of the one before,
	// which keeps a click there from zooming in on a single sample.
	uint64_t last_per = 0;

	for (size_t i = 0; i < data_->chunks.size(); i++) {
		const DecodeBinaryDataChunk &chunk = data_->chunks[i];
		const size_t size = chunk.data.size();

		if (offset >= size) {
			if (((i + 1) < data_->chunks.size()) && size) {
				const uint64_t next = data_->chunks[i + 1].sample;
				if (next > chunk.sample)
					last_per = (next - chunk.sample + size - 1) / size;
			}

			offset -= size;
			continue;
		}

		const uint64_t start = chunk.sample;
		uint64_t per = 0;

		if (((i + 1) < data_->chunks.size()) && size) {
			const uint64_t next = data_->chunks[i + 1].sample;
			if (next > start)
				per = (next - start + size - 1) / size;
		}

		if (!per)
			per = last_per;
		if (!per)
			per = 1;

		return make_pair(start + offset * per, start + (offset + 1) * per);
	}

	return make_pair<uint64_t, uint64_t>(0, 0);
}
