#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QDialog>
#include <QShowEvent>

#include "datastore.h"

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWidget(DataStore *ds, QWidget *parent = 0);
    ~ConfigWidget();

private slots:
    void on_ConfigWidget_accepted();

private:
    void showEvent(QShowEvent *event);

    Ui::ConfigWidget *ui;
    DataStore *ds;
};

#endif // CONFIGWIDGET_H
