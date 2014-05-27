#include "presetwidget.h"
#include "ui_presetwidget.h"

PresetWidget::PresetWidget(QWidget *parent) : QWidget(parent), ui(new Ui::PresetWidget)
{
    ui->setupUi(this);
    PresetButton *button;
    for(int i=0; i<10; i++){
        button = _getPresetButtonAt(i);
        connect(button, SIGNAL(sigClicked(QString)), this, SLOT(_onPresetButtonClicked(QString)));
        connect(this, SIGNAL(sigPresetClicked(QString)), button, SLOT(slotPresetSelected(QString)));
    }
}

PresetWidget::~PresetWidget()
{
    delete ui;
}

void PresetWidget::slotUpdatePresets(QStringList pathList)
{
    Q_ASSERT(pathList.size() <= 10);

    for(int i=0; i<pathList.size(); i++){
        this->_getPresetButtonAt(i)->setPreset(pathList.at(i));
    }
}

void PresetWidget::_onPresetButtonClicked(QString presetPath)
{
    emit this->sigPresetClicked(presetPath);
}

PresetButton *PresetWidget::_getPresetButtonAt(int pos)
{
    return ui->presetContainer->findChild<PresetButton*>("preset" + QString::number(pos));
}
