#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>

namespace Ui {
class FileBrowser;
}

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = 0);
    ~FileBrowser();

private:
    Ui::FileBrowser *ui;
};

#endif // FILEBROWSER_H
