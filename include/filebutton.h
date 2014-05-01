#ifndef FILEBUTTON_H
#define FILEBUTTON_H

#include <QDebug>

#include "hoverbutton.h"

class FileButton : public HoverButton
{
    Q_OBJECT
public:
    explicit FileButton(QWidget *parent = 0);
    void setPath(QString path);

signals:
    void myToggled(QString path, FileButton *butt);

public slots:
    void onToggled(bool t);

private:
    QString path;  // note: face text is only the last 20 characters of path

};

#endif // FILEBUTTON_H
