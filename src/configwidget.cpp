#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(DataStore *ds, QWidget *parent) : QDialog(parent), ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    this->ds = ds;
    this->deleting_item = false;

    for(QString lKey : ds->tblExtensions->getRecordKeys()){
        QListWidgetItem *w = new QListWidgetItem(lKey, ui->extensionlist);
        this->extWidgetMap.insert(lKey, w);
    }
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::showEvent(QShowEvent *event){
    if (!event->spontaneous()){ // we don't want to load the widget config on wm events (hide)
        this->ui->preset0->setText(ds->getPreset(0));
        this->ui->preset1->setText(ds->getPreset(1));
        this->ui->preset2->setText(ds->getPreset(2));
        this->ui->preset3->setText(ds->getPreset(3));
        this->ui->preset4->setText(ds->getPreset(4));
        this->ui->preset5->setText(ds->getPreset(5));
        this->ui->preset6->setText(ds->getPreset(6));
        this->ui->preset7->setText(ds->getPreset(7));
        this->ui->preset8->setText(ds->getPreset(8));
        this->ui->preset9->setText(ds->getPreset(9));

        this->ui->filemanager->setText(ds->getGeneralValue("file_browser"));
        this->ui->terminal->setText(ds->getGeneralValue("terminal_emulator"));
    }
}

void ConfigWidget::on_ConfigWidget_accepted()
{
    ds->setPreset(0, ui->preset0->text());
    ds->setPreset(1, ui->preset1->text());
    ds->setPreset(2, ui->preset2->text());
    ds->setPreset(3, ui->preset3->text());
    ds->setPreset(4, ui->preset4->text());
    ds->setPreset(5, ui->preset5->text());
    ds->setPreset(6, ui->preset6->text());
    ds->setPreset(7, ui->preset7->text());
    ds->setPreset(8, ui->preset8->text());
    ds->setPreset(9, ui->preset9->text());

    ds->setGeneralValue("file_browser", this->ui->filemanager->text());
    ds->setGeneralValue("terminal_emulator", this->ui->terminal->text());
}

void ConfigWidget::on_btnCommit_clicked()
{
    // commit new extension mapping
    QString extStr = ui->extension->text();
    QString extActPri = ui->viewerpath->text();
    QString extActSec = ui->editorPath->text();

    QListWidgetItem *w;
    if(this->extWidgetMap.contains(extStr)){
        w = this->extWidgetMap.value(extStr);
    } else {
        w = new QListWidgetItem(extStr, ui->extensionlist);
        this->extWidgetMap.insert(extStr, w);
    }
    ds->setExtensionValues(extStr, extActPri, extActSec);
    ui->extensionlist->setCurrentItem(w);
}

void ConfigWidget::on_extensionlist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (this->deleting_item)
        return;
    previous = previous; // I'm not unused, ha!
    QString extStr = current->text();
    ui->extension->setText(extStr);
    DsTable::Record record = ds->tblExtensions->getRecord(extStr);
    ui->viewerpath->setText(record.value("ext_act_pri").toString());
    ui->editorPath->setText(record.value("ext_act_sec").toString());
}

void ConfigWidget::on_btnDelete_clicked()
{
    if(ui->extensionlist->selectedItems().size() != 1)
        return;

    this->deleting_item = true;

    QListWidgetItem *w = ui->extensionlist->selectedItems().at(0);
    QString extStr = w->text();

    delete w;
    ds->tblExtensions->deleteRecord(extStr);

    ui->extension->clear();
    ui->viewerpath->clear();
    ui->editorPath->clear();

    this->deleting_item = false;

    if(ui->extensionlist->count() > 0){
        extStr = ui->extensionlist->selectedItems().at(0)->text();
        ui->extension->setText(extStr);

        DsTable::Record record = ds->tblExtensions->getRecord(extStr);

        ui->viewerpath->setText(record.value("ext_act_pri").toString());
        ui->editorPath->setText(record.value("ext_act_sec").toString());
    }
}






