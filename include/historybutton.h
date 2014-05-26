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
    void selected(QString id);

public slots:
    void idSelected(QString id);

private slots:
    void onClicked(bool checked);

private:
    QString id;
};

#endif // HISTORYBUTTON_H
