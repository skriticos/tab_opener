#ifndef PRESETWIDGET_H
#define PRESETWIDGET_H

#include <QWidget>

#include "presetbutton.h"

namespace Ui {
class PresetWidget;
}

class PresetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PresetWidget(QWidget *parent = 0);
    ~PresetWidget();

signals:
    void sigPresetClicked(QString);

public slots:
    void slotUpdatePresets(QStringList pathList);

private slots:
    void _onPresetButtonClicked(QString presetPath);
    PresetButton *_getPresetButtonAt(int pos);

private:
    Ui::PresetWidget *ui;
};

#endif // PRESETWIDGET_H
