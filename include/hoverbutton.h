#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>

class HoverButton : public QPushButton
{
    Q_OBJECT
public:
    explicit HoverButton(QWidget *parent = 0);

signals:

public slots:

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
};

#endif // HOVERBUTTON_H
