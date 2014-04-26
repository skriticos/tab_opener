#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QDialog>

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = 0);
    ~ConfigWidget();

private:
    Ui::ConfigWidget *ui;
};

#endif // CONFIGWIDGET_H
