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

#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QString>

#include "splitbutton.hpp"

namespace pv {
namespace widgets {

SplitButton::SplitButton(QWidget* parent) :
	QToolButton(parent),
	menu_(nullptr),
	menu_pressed_(false)
{
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	/*
	 * The style draws a button of this bar with little of its own, and
	 * the bars these buttons sit in carry none of the rules the bars of
	 * the main view set for themselves, so the states are filled here
	 * with the colours those rules use. The padding on the right is what
	 * holds the label clear of the seam at the end.
	 */
	setStyleSheet(QStringLiteral(
		"QToolButton {"
		"    padding: 2px 8px;"
		"    padding-right: 28px;"
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
}

void SplitButton::set_menu(QMenu* menu)
{
	menu_ = menu;
}

void SplitButton::paintEvent(QPaintEvent* event)
{
	QToolButton::paintEvent(event);

	if (!menu_)
		return;

	const QRect area(width() - kMenuWidth, 0, kMenuWidth, height());
	const QColor color(palette().color(QPalette::ButtonText));
	const int cx = area.center().x();
	const int cy = area.center().y();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// The seam between acting and picking a format.
	QColor seam(color);
	seam.setAlpha(70);
	painter.setPen(seam);
	painter.drawLine(area.left(), 7, area.left(), height() - 8);

	QPen pen(color);
	pen.setWidthF(1.3);
	pen.setCapStyle(Qt::RoundCap);
	painter.setPen(pen);
	painter.drawLine(cx - 3, cy - 1, cx, cy + 2);
	painter.drawLine(cx, cy + 2, cx + 3, cy - 1);
}

void SplitButton::mousePressEvent(QMouseEvent* event)
{
	if (!menu_ || (event->pos().x() < width() - kMenuWidth)) {
		menu_pressed_ = false;
		QToolButton::mousePressEvent(event);
		return;
	}

	menu_pressed_ = true;
	setDown(true);
}

void SplitButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (menu_pressed_) {
		menu_pressed_ = false;
		setDown(false);
		menu_->popup(mapToGlobal(QPoint(0, height())));
		return;
	}

	QToolButton::mouseReleaseEvent(event);
}

} // namespace widgets
} // namespace pv
