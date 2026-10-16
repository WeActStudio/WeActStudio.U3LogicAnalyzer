/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnalyzer is based on PulseView.
 *
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

#ifndef PULSEVIEW_PV_WIDGETS_SPLITBUTTON_HPP
#define PULSEVIEW_PV_WIDGETS_SPLITBUTTON_HPP

#include <QToolButton>

class QMenu;

namespace pv {
namespace widgets {

/**
 * A button that acts with its body and opens a menu with its end, as a
 * save button of a view does with the formats.
 *
 * The end of the button is drawn here and the width it takes is left out
 * of the label: the styles in use draw no indicator of their own for it
 * and leave it a few pixels wide, which is neither visible nor aimable.
 */
class SplitButton : public QToolButton
{
	Q_OBJECT

public:
	explicit SplitButton(QWidget* parent = nullptr);

	/**
	 * The menu the end of the button opens. It is kept here rather than
	 * handed to QToolButton, which would reserve a part of the button for
	 * an indicator this style does not draw.
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

} // namespace widgets
} // namespace pv

#endif // PULSEVIEW_PV_WIDGETS_SPLITBUTTON_HPP
