#include "hovertoolbutton.hpp"
#include <QMenu>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

namespace pv {
namespace widgets {

HoverToolButton::HoverToolButton(QWidget *parent):
	QToolButton(parent)
{

}

void HoverToolButton::setMenu(QMenu *menu)
{
    m_menu = menu;
    QToolButton::setMenu(menu);

    if (m_menu) {
        m_menu->installEventFilter(this);
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void HoverToolButton::enterEvent(QEnterEvent *event)
#else
void HoverToolButton::enterEvent(QEvent *event)
#endif
{
    QToolButton::enterEvent(event);

	QToolButton::showMenu();
}

bool HoverToolButton::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_menu && event->type() == QEvent::Hide) {
        QToolButton::setAttribute(Qt::WA_UnderMouse, false);
        QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(10, 10), QPoint(0, 0));
        QCoreApplication::sendEvent(this, &hoverEvent);
        setDown(false);

        return false;
    }

    if (m_menu && m_menu->isVisible() && event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint globalPos = mouseEvent->globalPos();

        QRect menuRect = m_menu->geometry();
        QRect btnRect  = geometry();
        btnRect.moveTo(mapToGlobal(QPoint(0, 0)));

        if (!menuRect.contains(globalPos) && !btnRect.contains(globalPos)) {
            m_menu->close();
        }
        return false;  // Do not intercept, let the event propagate
    }

    return false;
}

}  // namespace widgets
}  // namespace pv
