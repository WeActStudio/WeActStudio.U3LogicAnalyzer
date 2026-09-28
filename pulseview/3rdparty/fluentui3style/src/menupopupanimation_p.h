#pragma once

#include <QObject>

class QMenu;
class MenuPopupAnimatorImpl;

inline constexpr const char* MenuPopupSuppressPaintingProperty =
    "_q_fluent_menu_popup_suppress_painting";

class MenuPopupAnimator final : public QObject
{
public:
    explicit MenuPopupAnimator( QMenu* menu, QObject* parent = nullptr );
    ~MenuPopupAnimator() override;

    void stop();
    void setDuration( int duration );

private:
    MenuPopupAnimatorImpl* m_impl;
};
