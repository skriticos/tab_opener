#ifndef HISTORYBUTTON_H
#define HISTORYBUTTON_H

#include "hoverbutton.h"

class HistoryButton : public HoverButton
{
    Q_OBJECT

public:
    explicit HistoryButton(QWidget *parent = 0);

    void setId(QString id);
    void reset();

signals:
    void sigSelected(QString id);

public slots:
    void slotIdSelected(QString id);

private slots:
    void _slotOnClicked(bool checked);

private:
    QString id;
};

#endif // HISTORYBUTTON_H
