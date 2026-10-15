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

#include <extdef.h>

#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>

#include <pv/globalsettings.hpp>
#include <pv/session.hpp>
#include <pv/util.hpp>

#include "ruler.hpp"
#include "view.hpp"

using namespace Qt;

using std::function;
using std::max;
using std::min;
using std::shared_ptr;
using std::vector;

namespace pv {
namespace views {
namespace trace {

const float Ruler::RulerHeight = 2.5f; // x Text Height

const float Ruler::HoverArrowSize = 0.5f; // x Text Height

Ruler::Ruler(View &parent) :
	MarginWidget(parent)
{
	setMouseTracking(true);

	connect(&view_, SIGNAL(hover_point_changed(const QWidget*, QPoint)),
		this, SLOT(on_hover_point_changed(const QWidget*, QPoint)));
	connect(&view_, SIGNAL(offset_changed()),
		this, SLOT(invalidate_tick_position_cache()));
	connect(&view_, SIGNAL(scale_changed()),
		this, SLOT(invalidate_tick_position_cache()));
	connect(&view_, SIGNAL(tick_prefix_changed()),
		this, SLOT(invalidate_tick_position_cache()));
	connect(&view_, SIGNAL(tick_precision_changed()),
		this, SLOT(invalidate_tick_position_cache()));
	connect(&view_, SIGNAL(tick_period_changed()),
		this, SLOT(invalidate_tick_position_cache()));
	connect(&view_, SIGNAL(time_unit_changed()),
		this, SLOT(invalidate_tick_position_cache()));
}

QSize Ruler::sizeHint() const
{
	const int text_height = calculate_text_height();
	return QSize(0, RulerHeight * text_height);
}

QSize Ruler::extended_size_hint() const
{
	QRectF max_rect;
	vector< shared_ptr<TimeItem> > items(view_.time_items());
	for (auto &i : items)
		max_rect = max_rect.united(i->label_rect(QRect()));
	return QSize(0, sizeHint().height() - max_rect.top() / 2 +
		ViewItem::HighlightRadius);
}

QString Ruler::format_time_with_distance(
	const pv::util::Timestamp& distance,
	const pv::util::Timestamp& t,
	pv::util::SIPrefix prefix,
	pv::util::TimeUnit unit,
	unsigned precision,
	bool sign)
{
	const unsigned limit = 60;

	// If we have to use samples then we have no alternative formats
	if (unit == pv::util::TimeUnit::Samples)
		return pv::util::format_time_si_adjusted(t, prefix, precision, "sa", sign);

	QString unit_string;
	if (unit == pv::util::TimeUnit::Time)
		unit_string = "s";
	// Note: In case of pv::util::TimeUnit::None, unit_string remains empty

	/*
	 * Zero is written the way the ticks next to it are - "0 ms" beside
	 * "+100 ms" - and not as a bare "0" that drops the unit the whole scale
	 * carries. With TimeUnit::None no unit was asked for, so it stays bare.
	 */
	if (t.is_zero()) {
		if (unit_string.isEmpty())
			return "0";
		return pv::util::format_time_si_adjusted(t, prefix, precision, unit_string, sign);
	}

	// View zoomed way out -> low precision (0), big distance (>=60s)
	// -> DD:HH:MM
	if ((precision == 0) && (distance >= limit))
		return pv::util::format_time_minutes(t, 0, sign);

	// View in "normal" range -> medium precision, medium step size
	// -> HH:MM:SS.mmm... or xxxx (si unit) if less than limit seconds
	// View zoomed way in -> high precision (>3), low step size (<1s)
	// -> HH:MM:SS.mmm... or xxxx (si unit) if less than limit seconds
	if (abs(t) < limit)
		return pv::util::format_time_si_adjusted(t, prefix, precision, unit_string, sign);
	else
		return pv::util::format_time_minutes(t, precision, sign);
}

pv::util::Timestamp Ruler::get_absolute_time_from_x_pos(uint32_t x) const
{
	return view_.offset() + ((double)x + 0.5) * view_.scale();
}

pv::util::Timestamp Ruler::get_ruler_time_from_x_pos(uint32_t x) const
{
	return view_.ruler_offset() + ((double)x + 0.5) * view_.scale();
}

pv::util::Timestamp Ruler::get_ruler_time_from_absolute_time(const pv::util::Timestamp& abs_time) const
{
	return abs_time + view_.zero_offset();
}

pv::util::Timestamp Ruler::get_absolute_time_from_ruler_time(const pv::util::Timestamp& ruler_time) const
{
	return ruler_time - view_.zero_offset();
}

void Ruler::contextMenuEvent(QContextMenuEvent *event)
{
	MarginWidget::contextMenuEvent(event);

	// Don't show a context menu if the MarginWidget found a widget that shows one
	if (event->isAccepted())
		return;

	context_menu_x_pos_ = event->pos().x();

	QMenu *const menu = new QMenu(this);

	QAction *const create_marker = new QAction(tr("Create marker here"), this);
	connect(create_marker, SIGNAL(triggered()), this, SLOT(on_createMarker()));
	menu->addAction(create_marker);

	QAction *const set_zero_position = new QAction(tr("Set as zero point"), this);
	connect(set_zero_position, SIGNAL(triggered()), this, SLOT(on_setZeroPosition()));
	menu->addAction(set_zero_position);

	if (view_.zero_offset().convert_to<double>() != 0) {
		QAction *const reset_zero_position = new QAction(tr("Reset zero point"), this);
		connect(reset_zero_position, SIGNAL(triggered()), this, SLOT(on_resetZeroPosition()));
		menu->addAction(reset_zero_position);
	}

	QAction *const toggle_hover_marker = new QAction(this);
	connect(toggle_hover_marker, SIGNAL(triggered()), this, SLOT(on_toggleHoverMarker()));
	menu->addAction(toggle_hover_marker);

	GlobalSettings settings;
	const bool hover_marker_shown =
		settings.value(GlobalSettings::Key_View_ShowHoverMarker).toBool();
	toggle_hover_marker->setText(hover_marker_shown ?
		tr("Disable mouse hover marker") : tr("Enable mouse hover marker"));

	event->setAccepted(true);
	// menu->popup(event->globalPos());
}

void Ruler::resizeEvent(QResizeEvent*)
{
	// the tick calculation depends on the width of this widget
	invalidate_tick_position_cache();
}

vector< shared_ptr<ViewItem> > Ruler::items()
{
	const vector< shared_ptr<TimeItem> > time_items(view_.time_items());
	return vector< shared_ptr<ViewItem> >(
		time_items.begin(), time_items.end());
}

void Ruler::item_hover(const shared_ptr<ViewItem> &item, QPoint pos)
{
	(void)pos;

	hover_item_ = dynamic_pointer_cast<TimeItem>(item);
}

shared_ptr<TimeItem> Ruler::get_reference_item() const
{
	// Note: time() returns 0 if item returns no valid time

	if (mouse_modifiers_ & Qt::ShiftModifier)
		return nullptr;

	if (hover_item_ && (hover_item_->time() != 0))
		return hover_item_;

	shared_ptr<TimeItem> ref_item;
	const vector< shared_ptr<TimeItem> > items(view_.time_items());

	for (auto i = items.rbegin(); i != items.rend(); i++) {
		if ((*i)->enabled() && (*i)->selected()) {
			if (!ref_item)
				ref_item = *i;
			else {
				// Return nothing if multiple items are selected
				ref_item.reset();
				break;
			}
		}
	}

	if (ref_item && (ref_item->time() == 0))
		ref_item.reset();

	return ref_item;
}

shared_ptr<ViewItem> Ruler::get_mouse_over_item(const QPoint &pt)
{
	const vector< shared_ptr<TimeItem> > items(view_.time_items());

	for (auto i = items.rbegin(); i != items.rend(); i++)
		if ((*i)->enabled() && (*i)->label_rect(rect()).contains(pt))
			return *i;

	return nullptr;
}

void Ruler::on_add_RuleFlag(QMouseEvent* ev)
{
	mouseDoubleClickEvent(ev);
}

void Ruler::mouseDoubleClickEvent(QMouseEvent *event)
{
	hover_item_ = view_.add_flag(get_absolute_time_from_x_pos(pv::util::mouse_position(event).x()));
}

void Ruler::paintEvent(QPaintEvent*)
{
	if (!tick_position_cache_) {
		auto ffunc = [this](const pv::util::Timestamp& t) {
			return format_time_with_distance(
				this->view_.tick_period(),
				t,
				this->view_.tick_prefix(),
				this->view_.time_unit(),
				this->view_.tick_precision());
		};

		tick_position_cache_ = calculate_tick_positions(
			view_.tick_period(),
			view_.ruler_offset(),
			view_.scale(),
			width(),
			view_.minor_tick_count(),
			ffunc);
	}

	const int ValueMargin = 3;

	const int text_height = calculate_text_height();
	const int ruler_height = RulerHeight * text_height;
	const int major_tick_y1 = text_height + ValueMargin * 2;
	const int minor_tick_y1 = (major_tick_y1 + ruler_height) / 2;

	QPainter p(this);

	// Draw the tick marks
	p.setPen(palette().color(foregroundRole()));

	for (const auto& tick: tick_position_cache_->major) {
		const int leftedge = 0;
		const int rightedge = width();
		const int x_tick = tick.first;
		/*
		 * A tick that lands exactly on an edge is drawn as well. A capture
		 * spanning a whole number of tick periods - 1 s on a 100 ms scale -
		 * puts the first and the last tick on x = 0 and x = width, and a
		 * strict comparison dropped both of their legends while every tick
		 * in between kept one.
		 */
		if ((x_tick >= leftedge) && (x_tick <= rightedge)) {
			const int x_left_bound = util::text_width(QFontMetrics(font()), tick.second) / 2;
			const int x_right_bound = rightedge - x_left_bound;
			const int x_legend = min(max(x_tick, x_left_bound), x_right_bound);
			p.drawText(x_legend, ValueMargin, 0, text_height,
				AlignCenter | AlignTop | TextDontClip, tick.second);
			/* The widget ends at its last pixel, so the line of an edge
			 * tick is held back onto it. */
			const int x_line = min(x_tick, rightedge - 1);
			p.drawLine(QPointF(x_line, major_tick_y1),
			QPointF(x_line, ruler_height));
		}
	}

	for (const auto& tick: tick_position_cache_->minor) {
		p.drawLine(QPointF(tick, minor_tick_y1),
				QPointF(tick, ruler_height));
	}

	// Draw the hover mark
	draw_hover_mark(p, text_height);

	p.setRenderHint(QPainter::Antialiasing);

	// The cursor labels are not drawn with the arrows exactly on the
	// bottom line of the widget, because then the selection shadow
	// would be clipped away.
	const QRect r = rect().adjusted(0, 0, 0, -ViewItem::HighlightRadius);

	// Draw the items
	const vector< shared_ptr<TimeItem> > items(view_.time_items());
	for (auto &i : items) {
		const bool highlight = !item_dragging_ &&
			i->label_rect(r).contains(mouse_point_);
		i->paint_label(p, r, highlight);
	}
}

void Ruler::draw_hover_mark(QPainter &p, int text_height)
{
	const int x = view_.hover_point().x();

	if (x == -1)
		return;

	p.setPen(QPen(Qt::NoPen));
	p.setBrush(QBrush(palette().color(foregroundRole())));

	const int b = RulerHeight * text_height;
	const float hover_arrow_size = HoverArrowSize * text_height;
	const QPointF points[] = {
		QPointF(x, b),
		QPointF(x - hover_arrow_size, b - hover_arrow_size),
		QPointF(x + hover_arrow_size, b - hover_arrow_size)
	};
	p.drawPolygon(points, countof(points));

	/*
	 * Say what time the arrow points at, in the format the ticks are
	 * labelled with. It sits between two of them, and its time can
	 * otherwise only be counted out from the ones around it.
	 */
	const pv::util::Timestamp time = get_ruler_time_from_x_pos(x);
	QString label = format_time_with_distance(
		view_.tick_period(), time, view_.tick_prefix(),
		view_.time_unit(), view_.tick_precision());

	/*
	 * Add which sample of the capture that is, which is what the data is
	 * indexed by. It is the absolute time that counts here, the zero point
	 * is only where the ruler starts. Left out while the ticks are already
	 * labelled with samples.
	 */
	const double samplerate = view_.session().get_samplerate();
	if ((samplerate > 0) &&
		(view_.time_unit() != pv::util::TimeUnit::Samples)) {
		const pv::util::Timestamp abs_time = get_absolute_time_from_x_pos(x);
		const int64_t sample =
			(abs_time * samplerate).convert_to<int64_t>();
		label += QString(" (%1)").arg(sample);
	}

	// Centre it on the arrow and keep it inside the ruler, which the
	// arrow itself reaches while the pointer is at one of its borders.
	// The label sits a little above the arrow, so that the two do not
	// read as one shape.
	const int LabelPadding = 3;
	const int LabelVPadding = 3;
	const int LabelRaise = 1;
	const QFontMetrics label_metrics(font());

	/*
	 * The height of a whole line, not just the ascent the ruler itself is
	 * laid out with: the text is centred in the tag, and a tag only as tall
	 * as the ascent leaves it with nothing underneath the text.
	 */
	const int label_text_height = label_metrics.height();
	const int label_width =
		util::text_width(label_metrics, label) + 2 * LabelPadding;
	const int label_height = label_text_height + 2 * LabelVPadding;
	const int label_x = min(max(x - label_width / 2, 0),
		width() - label_width);
	const QRect label_rect(label_x,
		b - hover_arrow_size - label_height - LabelRaise,
		label_width, label_height);

	/*
	 * The label lands on the ticks, so it brings the background along and
	 * covers them: letting them show through the text leaves both the text
	 * and the ticks hard to read. It is drawn as a rounded tag rather than
	 * as a plain rectangle, so that it reads as something put on top of the
	 * ruler instead of as a gap in it.
	 */
	const QColor label_color = palette().color(foregroundRole());
	const qreal corner_radius = 3.0;

	/*
	 * The outline is a faint one: at full strength it sits right next to
	 * the text in the same colour and the two read as one, while all it has
	 * to do is set the tag apart from the ticks behind it.
	 */
	QColor label_outline = label_color;
	label_outline.setAlpha(110);

	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(QPen(label_outline, 1));
	p.setBrush(palette().brush(QPalette::Base));
	// Half a pixel in, so that the one pixel outline lands on the pixel
	// grid instead of being smeared over two of them.
	p.drawRoundedRect(QRectF(label_rect).adjusted(0.5, 0.5, -0.5, -0.5),
		corner_radius, corner_radius);

	p.setPen(label_color);
	p.drawText(label_rect, AlignCenter, label);
}

int Ruler::calculate_text_height() const
{
	return QFontMetrics(font()).ascent();
}

TickPositions Ruler::calculate_tick_positions(
	const pv::util::Timestamp& major_period,
	const pv::util::Timestamp& offset,
	const double scale,
	const int width,
	const unsigned int minor_tick_count,
	function<QString(const pv::util::Timestamp&)> format_function)
{
	TickPositions tp;

	const pv::util::Timestamp minor_period = major_period / minor_tick_count;
	const pv::util::Timestamp first_major_division = floor(offset / major_period);
	const pv::util::Timestamp first_minor_division = ceil(offset / minor_period);
	const pv::util::Timestamp t0 = first_major_division * major_period;

	int division = (round(first_minor_division -
		first_major_division * minor_tick_count)).convert_to<int>() - 1;

	double x;

	do {
		pv::util::Timestamp t = t0 + division * minor_period;
		x = ((t - offset) / scale).convert_to<double>();

		if (division % minor_tick_count == 0) {
			// Recalculate 't' without using 'minor_period' which is a fraction
			t = t0 + division / minor_tick_count * major_period;
			tp.major.emplace_back(x, format_function(t));
		} else {
			tp.minor.emplace_back(x);
		}

		division++;
	} while (x < width);

	return tp;
}

void Ruler::on_hover_point_changed(const QWidget* widget, const QPoint &hp)
{
	(void)widget;
	(void)hp;

	update();
}

void Ruler::invalidate_tick_position_cache()
{
	tick_position_cache_ = boost::none;
}

void Ruler::on_createMarker()
{
	hover_item_ = view_.add_flag(get_absolute_time_from_x_pos(mouse_down_point_.x()));
}

void Ruler::on_setZeroPosition()
{
	view_.set_zero_position(get_absolute_time_from_x_pos(mouse_down_point_.x()));
}

void Ruler::on_resetZeroPosition()
{
	view_.reset_zero_position();
}

void Ruler::on_toggleHoverMarker()
{
	GlobalSettings settings;
	const bool state = settings.value(GlobalSettings::Key_View_ShowHoverMarker).toBool();
	settings.setValue(GlobalSettings::Key_View_ShowHoverMarker, !state);
}

} // namespace trace
} // namespace views
} // namespace pv
