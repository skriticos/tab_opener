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
    void charmClicked(QString path);

public slots:
    void onClicked();

private:
    QString path;

};

#endif // CHARMBUTTON_H
