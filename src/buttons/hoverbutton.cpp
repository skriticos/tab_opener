#include "hoverbutton.h"

HoverButton::HoverButton(QWidget *parent) : QPushButton(parent)
{
    setFlat(true);
}

void HoverButton::leaveEvent(QEvent * e)
{
    setFlat(true);
    QPushButton::leaveEvent(e);
}

void HoverButton::enterEvent(QEvent * e)
{
    setFlat(false);
    QPushButton::enterEvent(e);
}
