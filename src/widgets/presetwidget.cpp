#include "presetwidget.h"
#include "ui_presetwidget.h"

PresetWidget::PresetWidget(QWidget *parent) : QWidget(parent), ui(new Ui::PresetWidget)
{
    ui->setupUi(this);
    PresetButton *button;
    for(int i=0; i<10; i++){
        button = _getPresetButtonAt(i);
        connect(button, SIGNAL(sigClicked(QString)), this, SLOT(_slotPresetButtonClicked(QString)));
        connect(this, SIGNAL(sigFolderChanged(QString)), button, SLOT(slotPresetSelected(QString)));
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

void PresetWidget::slotFolderChanged(QString folderPath)
{
    emit this->sigFolderChanged(folderPath);
}

void PresetWidget::_slotPresetButtonClicked(QString presetPath)
{
    emit this->sigFolderChanged(presetPath);
}

PresetButton *PresetWidget::_getPresetButtonAt(int pos)
{
    return ui->presetContainer->findChild<PresetButton*>("preset" + QString::number(pos));
}
