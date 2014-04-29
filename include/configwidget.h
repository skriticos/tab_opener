#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QDialog>
#include <QShowEvent>
#include <QListWidgetItem>

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
    void on_btnCommit_clicked();
    void on_extensionlist_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_btnDelete_clicked();

private:
    void showEvent(QShowEvent *event);

    Ui::ConfigWidget *ui;
    DataStore *ds;

    bool deleting_item;
};

#endif // CONFIGWIDGET_H
