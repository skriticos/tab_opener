#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(DataStore *ds, QWidget *parent) : QDialog(parent), ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    this->ds = ds;
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
