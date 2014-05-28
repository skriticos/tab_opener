#ifndef PRESETWIDGET_H
#define PRESETWIDGET_H

#include <QtCore>
#include <QtWidgets>

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
    void sigFolderChanged(QString);

public slots:
    void slotUpdatePresets(QStringList pathList);
    void slotFolderChanged(QString folderPath);

private slots:
    void _slotPresetButtonClicked(QString presetPath);

private:
    Ui::PresetWidget *ui;
    PresetButton *_getPresetButtonAt(int pos);
};

#endif // PRESETWIDGET_H
