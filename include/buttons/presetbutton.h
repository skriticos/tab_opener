#ifndef PRESETBUTTON_H
#define PRESETBUTTON_H

#include <QtCore>
#include <QPushButton>

#include "hoverbutton.h"

class PresetButton : public HoverButton
{
    Q_OBJECT
public:
    explicit PresetButton(QWidget *parent = 0);
    void setPreset(QString preset);

signals:
    void sigClicked(QString path);

public slots:
    void slotPresetSelected(QString presetPath);

private slots:
    void _slotClicked(bool checked);

private:
    QString presetPath;
};

#endif // PRESETBUTTON_H
