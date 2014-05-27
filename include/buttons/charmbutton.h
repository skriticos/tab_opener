#ifndef CHARMBUTTON_H
#define CHARMBUTTON_H

#include "hoverbutton.h"
#include <QPushButton>

class CharmButton : public HoverButton
{
    Q_OBJECT
public:
    explicit CharmButton(QString label, QString path, QWidget *parent = 0);

signals:
    void sigCharmClicked(QString path);

private slots:
    void _slotClicked();

private:
    QString path;

};

#endif // CHARMBUTTON_H
