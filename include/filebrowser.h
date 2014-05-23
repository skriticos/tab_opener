#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QtWidgets>

#include "util.h"
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

    void commandProcessStarted();
    void commandProcessStopped();

private slots:
    void onFileSelected();
    void onFolderSeleced();
    void onConfigAccepted();

    void on_btnActPrimary_clicked();
    void on_btnActSecondary_clicked();
    void on_btnHome_clicked();
    void on_btnTerminal_clicked();
    void on_btnExtFileBrowser_clicked();
    void on_btnPreferences_clicked();
    void on_btnScmPull_clicked();
    void on_btnScmCommit_clicked();
    void on_btnScmPush_clicked();
    void on_btnHelp_clicked();

signals:
    void folderSelected(QString folderPath);
    void fileSelected(QString filePath);
    void configChanged();
    void closeAction();
    void execMultiCommand(QStringList);
    void execCommand(QString);

private:
    Ui::FileBrowser *ui;
    DataStore *ds;
    bool isInit;

    QFileSystemModel *dirmodel, *filemodel;
    ConfigWidget *configWidget;
    QList<QPushButton> charmButtons;
    QList<CharmButton *> charmButtonList;
    QList<QLabel *> charmLabelList;
};

#endif // FILEBROWSER_H
