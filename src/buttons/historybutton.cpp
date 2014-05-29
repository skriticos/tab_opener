#include "historybutton.h"

HistoryButton::HistoryButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(_slotOnClicked(bool)));
}

void HistoryButton::setId(QString id)
{
    this->id = id;
}

void HistoryButton::reset()
{
    this->blockSignals(true);

    this->setChecked(false);
    this->id = "";
    this->setText("");

    this->blockSignals(false);
}

void HistoryButton::slotIdSelected(QString id)
{
    if(this->id.isEmpty()) {
        return;
    }

    this->blockSignals(true);

    if(this->id == id) {
        this->setChecked(true);
    } else {
        this->setChecked(false);
    }

    this->blockSignals(false);
}

void HistoryButton::_slotOnClicked(bool checked)
{
    if(this->id.isEmpty()) {
        this->setChecked(false);
        return;
    }

    if(!checked) {
        this->setChecked(true);
        return;
    } else {
        emit this->sigSelected(this->id);
    }
}
