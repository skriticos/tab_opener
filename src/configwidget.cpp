#include "configwidget.h"
#include "ui_configwidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}
