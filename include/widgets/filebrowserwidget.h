#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QtWidgets>

#include "charmbutton.h"
#include "defines.h"
#include "util.h"

namespace Ui
{
    class FileBrowser;
}

class FileBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowserWidget(QWidget *parent = 0);
    ~FileBrowserWidget();

signals:
    void sigFolderSelected(QString folderPath);
    void sigFileSelected(QString filePath);
    void sigRequestOpenExtApp(FileOpen::ExtApp extApp, QString folderPath);
    void sigRequestOpenFile(FileOpen::OpenType openType, QString filePath);

    void sigRequestScmPull();
    void sigRequestScmCommit(QString commitMsg);
    void sigRequestScmPush();

    void sigConfigClicked();

public slots:
    void slotInitLocation(QString folderPath, QString filePath);

    void slotSelectFolder(QString folderPath);
    void slotSelectFile(QString filePath);

    void slotScmOff();
    void slotScmOn();

private slots:
    void _slotOpenFilePrimary();
    void _slotOpenFileSeconday();
    void _slotOnFileSelected();
    void _slotOnFolderSeleced();

    void on_btnHome_clicked();
    void on_btnTerminal_clicked();
    void on_btnExtFileBrowser_clicked();
    void on_btnPreferences_clicked();
    void on_btnScmPull_clicked();
    void on_btnScmCommit_clicked();
    void on_btnScmPush_clicked();
    void on_btnHelp_clicked();

private:
    QString _getSelectedFolder();
    QString _getSelectedFile();

    Ui::FileBrowser *ui;

    QFileSystemModel *dirmodel, *filemodel;
    QList<QPushButton> charmButtons;
    QList<CharmButton *> charmButtonList;
    QList<QLabel *> charmLabelList;
};

#endif // FILEBROWSER_H
