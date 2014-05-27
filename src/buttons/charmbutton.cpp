#include "charmbutton.h"

CharmButton::CharmButton(QString label, QString path, QWidget *parent) : HoverButton(parent)
{
    this->setStyleSheet("min-width: 0px; padding: 2px;");
    this->setText(label);
    this->path = path;

    connect(this, SIGNAL(clicked()), this, SLOT(_slotClicked()));
}

void CharmButton::_slotClicked()
{
    emit sigCharmClicked(this->path);
}
