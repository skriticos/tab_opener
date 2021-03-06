#include "presetbutton.h"

PresetButton::PresetButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(_slotClicked(bool)));
}

void PresetButton::setPreset(QString presetPath)
{
    auto presetLabel = [](QString presetPath) {
        if(presetPath.size() > 35) {
            return "..." + presetPath.right(35);
        }

        return presetPath;
    };

    this->presetPath = presetPath;
    this->setText(presetLabel(presetPath));

    this->blockSignals(true);
    this->setChecked(false);
    this->blockSignals(false);
}

void PresetButton::slotPresetSelected(QString presetPath)
{
    if(this->presetPath.isEmpty()) {
        return;
    }

    if(this->presetPath == presetPath && this->isChecked()) {
        return;
    }

    this->blockSignals(true);

    if(presetPath == this->presetPath) {
        this->setChecked(true);
    } else {
        this->setChecked(false);
    }

    this->blockSignals(false);
}

void PresetButton::_slotClicked(bool checked)
{
    if(!checked) {
        this->setChecked(false);
        return;
    }

    if(!this->text().isEmpty()) {
        emit sigClicked(this->presetPath);
    }
}

