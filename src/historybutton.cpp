#include "historybutton.h"

HistoryButton::HistoryButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
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

void HistoryButton::idSelected(QString id)
{
    this->blockSignals(true);

    if(this->id == id){
        if(!this->isChecked()){
            this->setChecked(true);
        }
    } else {
        this->setChecked(false);
    }

    this->blockSignals(false);
}

void HistoryButton::onClicked(bool checked)
{
    if(!checked){
        this->setChecked(true);
        return;
    } else {
        emit this->selected(this->id);
    }
}
