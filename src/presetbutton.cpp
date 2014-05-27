#include "presetbutton.h"

PresetButton::PresetButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(_slotClicked()));
}

void PresetButton::_slotClicked()
{
    if (!this->text().isEmpty())
        emit sigClicked(this->text());
}

