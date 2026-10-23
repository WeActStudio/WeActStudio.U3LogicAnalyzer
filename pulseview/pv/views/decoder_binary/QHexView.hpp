/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnalyzer is based on PulseView.
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

#ifndef PULSEVIEW_PV_VIEWS_DECODER_BINARY_QHEXVIEW_HPP
#define PULSEVIEW_PV_VIEWS_DECODER_BINARY_QHEXVIEW_HPP

#include <QAbstractScrollArea>

#include <pv/data/decodesignal.hpp>

using std::pair;
using std::size_t;
using pv::data::DecodeBinaryClass;
using pv::data::DecodeBinaryDataChunk;

class QHexView: public QAbstractScrollArea
{
	Q_OBJECT

public:
	enum Mode {
		ChunkedDataMode,    ///< Displays all data chunks in succession
		MemoryEmulationMode ///< Reconstructs memory contents from data chunks
	};

	/* How a byte is written in the column on the right. The column on the
	 * left is always hex without 0x: that is what a dump is read for. */
	enum DisplayFormat {
		FormatAscii,    ///< f
		FormatHex,      ///< 66
		FormatDec,      ///< 102
		FormatOct,      ///< 146
		FormatBin,      ///< 01100110
		FormatCArray    ///< 0x66,
	};

	/* Which of the two columns the selection was made in. Each copies its
	 * bytes the way it writes them. */
	enum SelectArea {
		AreaBytes,
		AreaText
	};

public:
	QHexView(QWidget *parent = nullptr);

	void set_mode(Mode m);
	void set_display_format(DisplayFormat format);
	void set_data(const DecodeBinaryClass* data);

	/* Sets range of samples that are visible in the main view */
	void set_visible_sample_range(uint64_t start, uint64_t end);

	/* Sets sample whose associated data we should highlight */
	void set_highlighted_data_sample(uint64_t sample);

	unsigned int get_bytes_per_line() const;

	void clear();
	virtual QSizePolicy sizePolicy() const;
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;

	pair<size_t, size_t> get_selection() const;

	size_t create_hex_line(size_t start, size_t end, QString* dest,
		bool with_offset=false, bool with_ascii=false);

Q_SIGNALS:
	/* A byte was clicked. The samples it covers say where it is in the trace. */
	void byte_selected(uint64_t start_sample, uint64_t end_sample);

protected:
	void initialize_byte_iterator(size_t offset);
	uint8_t get_next_byte(bool* is_new_chunk = nullptr);

	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	QSize getFullSize() const;
	void resetSelection();
	void resetSelection(int pos);
	void setSelection(int pos);
	void ensureVisible();
	void setCursorPos(int pos);
	size_t cursorPosFromMousePos(const QPoint &position, bool *on_text = nullptr,
		bool *on_bytes = nullptr);
	void update_theme_colors();
	void update_layout();
	QString byte_text(uint8_t value) const;
	QString text_text(uint8_t value) const;
	unsigned int byte_cell_chars() const;
	unsigned int byte_cell_step() const;
	unsigned int text_cell_chars() const;
	unsigned int text_cell_step() const;
	pair<uint64_t, uint64_t> byte_sample_range(size_t offset) const;
	void copy_selection();

private:
	Mode mode_;
	DisplayFormat format_;
	const DecodeBinaryClass* data_;
	size_t data_size_;

	size_t posAddr_, posHex_, posText_;
	size_t charWidth_, charHeight_;
	size_t selectBegin_, selectEnd_, selectInit_, cursorPos_;
	SelectArea select_area_;
	uint8_t address_digits_;
	bool mouse_pressed_;

	size_t current_chunk_id_, current_chunk_offset_, current_offset_;
	DecodeBinaryDataChunk current_chunk_; // Cache locally so that we're not messed up when the vector is re-allocating its data
	uint64_t current_chunk_sample_, next_chunk_sample_;

	pair<uint64_t, uint64_t> visible_range_;
	uint64_t highlighted_sample_;

	vector<QColor> chunk_colors_;
	QColor visible_range_color_;
	bool theme_dark_;
};

#endif // PULSEVIEW_PV_VIEWS_DECODER_BINARY_QHEXVIEW_HPP
