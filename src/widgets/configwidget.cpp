#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) : QDialog(parent), ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    ui->btnCommit->setEnabled(false);
    ui->btnDelete->setEnabled(false);
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::slotInitConfig(QStringList presetList,
                                  QList<Config::ExtensionEntry> extList,
                                  QString terminalCmd, QString fbrowserCmd)
{
    this->presetList = presetList;
    this->terminalCmd = terminalCmd;
    this->fbrowserCmd = fbrowserCmd;

    for(int i = 0; i < extList.size(); i++) {
        QString extName = extList.at(i).extName;
        this->extIndex.insert(extName, extList.at(i));
        new QListWidgetItem(extName, ui->extensionlist);
    }

    _reloadSettings();
}

void ConfigWidget::_reloadSettings()
{
    Q_ASSERT(this->presetList.size() <= 10);

    for(int i = 0; i < presetList.size(); i++) {
        _getPresetInput(i)->setText(this->presetList.at(i));
    }

    this->ui->filemanager->setText(this->fbrowserCmd);
    this->ui->terminal->setText(this->terminalCmd);
}

QLineEdit *ConfigWidget::_getPresetInput(int pos)
{
    return ui->presetContainer->findChild<QLineEdit *>("preset" + QString::number(pos));
}

void ConfigWidget::on_btnCommit_clicked()
{
    // commit new extension mapping
    QString extStr = ui->extension->text();
    QString extActPri = ui->viewerpath->text();
    QString extActSec = ui->editorPath->text();

    QListWidgetItem *w;

    if(this->extWidgetMap.contains(extStr)) {
        w = this->extWidgetMap.value(extStr);
    } else {
        w = new QListWidgetItem(extStr, ui->extensionlist);
        this->extWidgetMap.insert(extStr, w);
    }

    if(!this->extIndex.contains(extStr) ||
            this->extIndex.value(extStr).extActPri != extActPri ||
            this->extIndex.value(extStr).extActSec != extActSec) {

        Config::ExtensionEntry eEntry = {extStr, extActPri, extActSec};
        this->extIndex.insert(extStr, eEntry);
        emit this->sigExtensionChanged(eEntry);
    }

    ui->extensionlist->setCurrentItem(w);
}

void ConfigWidget::on_btnDelete_clicked()
{
    QString extStr;

    ui->extensionlist->blockSignals(true);

    QListWidgetItem *w = ui->extensionlist->selectedItems().first();
    extStr = w->text();
    delete w;

    ui->extension->clear();
    ui->viewerpath->clear();
    ui->editorPath->clear();

    ui->extensionlist->blockSignals(false);

    if(ui->extensionlist->count() > 0) {
        ui->extensionlist->item(0)->setSelected(true);
    } else {
        ui->btnDelete->setEnabled(false);
    }

    emit this->sigExtensionDeleted(extStr);
}

void ConfigWidget::on_extensionlist_itemSelectionChanged()
{
    QList<QListWidgetItem *> sel = ui->extensionlist->selectedItems();

    if(sel.size() == 1) {
        QString extStr = sel.first()->text();
        ui->extension->setText(extStr);
        ui->viewerpath->setText(this->extIndex.value(extStr).extActPri);
        ui->editorPath->setText(this->extIndex.value(extStr).extActSec);
        ui->btnDelete->setEnabled(true);
    } else if(sel.size() == 0) {
        ui->btnDelete->setEnabled(false);
    } else {
        Q_ASSERT(false);
    }
}

void ConfigWidget::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Save) == button) {
        if(this->presetList.size() == 10) {
            bool isChanged = false;

            for(int i = 0; i < 10; i++) {
                if(_getPresetInput(i)->text() != this->presetList.at(i)) {
                    isChanged = true;
                    break;
                }
            }

            if(isChanged) {
                this->presetList.clear();

                for(int i = 0; i < 10; i++) {
                    this->presetList.append(_getPresetInput(i)->text());
                }

                emit this->sigPresetsChanged(this->presetList);
            }
        }

        if(this->fbrowserCmd != ui->filemanager->text()) {
            this->fbrowserCmd = ui->filemanager->text();
            emit this->sigExtFileBrowserChanged(this->fbrowserCmd);
        }

        if(this->terminalCmd != ui->terminal->text()) {
            this->terminalCmd = ui->terminal->text();
            emit this->sigTerminalChanged(this->terminalCmd);
        }
    } else if(ui->buttonBox->button(QDialogButtonBox::Discard) == button) {
        _reloadSettings();
        this->hide();
    } else if(ui->buttonBox->button(QDialogButtonBox::Reset) == button) {
        _reloadSettings();
    }
}

void ConfigWidget::on_extension_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()) {
        ui->btnCommit->setEnabled(false);
    } else {
        ui->btnCommit->setEnabled(true);
    }
}
