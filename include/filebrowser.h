#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QtWidgets>

#include "configwidget.h"
#include "charmbutton.h"
#include "datastore.h"

namespace Ui {
    class FileBrowser;
}

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = 0);
    ~FileBrowser();

    void initFileBrowser(DataStore *ds);

    QString getSelectedFolder();
    QString getSelectedFile();

public slots:
    void setSelectedFolder(QString folderPath);
    void setSelectedFile(QString filePath);

private slots:
    void onFileSelected();
    void onFolderSeleced();

signals:
    void folderSelected(QString folderPath);
    void fileSelected(QString filePath);

private:
    Ui::FileBrowser *ui;
    DataStore *ds;
    bool isInit;

    QFileSystemModel *dirmodel, *filemodel;
    ConfigWidget *wconfig;
    QList<QPushButton> charmButtons;
    QList<CharmButton*> cblist;
    QList<QLabel*> cllist;
};

#endif // FILEBROWSER_H
