#include "presetbutton.h"

PresetButton::PresetButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void PresetButton::onClicked()
{
    if (!this->text().isEmpty())
        emit presetClicked(this->text());
}

