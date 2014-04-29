#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(DataStore *ds, QWidget *parent) : QDialog(parent), ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    this->ds = ds;
    this->deleting_item = false;
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

        this->ui->filemanager->setText(ds->getFileBrowser());
        this->ui->terminal->setText(ds->getTerminalEmulator());
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

    ds->setFileBrowser(ui->filemanager->text());
    ds->setTerminalEmulator(ui->terminal->text());
}

void ConfigWidget::on_btnCommit_clicked()
{
    // commit new extension mapping
    QString ext = ui->extension->text();
    QString open = ui->viewerpath->text();
    QString edit = ui->editorPath->text();

    QListWidgetItem *w;
    if(!ds->extMapContains(ext)) {
        w = new QListWidgetItem(ext, ui->extensionlist);
        ds->setExtMapItem(ext, w);

    } else { // existing extension
        w = ds->getExtMapItem(ext);
    }
    ds->setOpenAppsItem(ext, open);
    ds->setEditMapItem(ext, edit);
    ui->extensionlist->setCurrentItem(w);
}

void ConfigWidget::on_extensionlist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (this->deleting_item)
        return;
    previous = previous; // I'm not unused, ha!
    QString ext = current->text();
    ui->extension->setText(ext);
    ui->viewerpath->setText(ds->getOpenAppsItem(ext));
    ui->editorPath->setText(ds->getEditMapItem(ext));
}

void ConfigWidget::on_btnDelete_clicked()
{
    if(ds->getExtMapSize() == 0)
        return;
    this->deleting_item = true;
    QListWidgetItem *w = ui->extensionlist->selectedItems().at(0);
    QString ext = w->text();
    delete w;
    ds->deleteOpenAppsItem(ext);
    ds->deleteEditMapItem(ext);
    ds->deleteExtMapItem(ext);
    ui->extension->clear();
    ui->viewerpath->clear();
    ui->editorPath->clear();
    this->deleting_item = false;
    if(ds->getExtMapSize() > 0) {
        ext = ui->extensionlist->selectedItems().at(0)->text();
        ui->extension->setText(ext);
        ui->viewerpath->setText(ds->getOpenAppsItem(ext));
        ui->editorPath->setText(ds->getEditMapItem(ext));
    }
}






