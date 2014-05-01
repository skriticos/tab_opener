#include "filebutton.h"

FileButton::FileButton(QWidget *parent) : HoverButton(parent)
{
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}

void FileButton::setPath(QString path)
{
    this->path = path;
    QString label;
    if (path.size() > 35)
        label = "..." + path.right(35);
    else
        label = path;
    this->setText(label);
}

void FileButton::onToggled(bool t)
{
    if (t) {
        if(!this->text().isEmpty())
            emit myToggled(this->path, this);
        else {
            this->toggle();
        }
    }

}
