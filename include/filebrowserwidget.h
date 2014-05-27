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

class FileBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowserWidget(QWidget *parent = 0);
    ~FileBrowserWidget();

    void initFileBrowser(DataStore *ds);

    QString getSelectedFolder();
    QString getSelectedFile();

signals:
    void sigFolderSelected(QString folderPath);
    void sigFileSelected(QString filePath);
    void sigConfigChanged();
    void sigCloseAction();
    void sigExecMultiCommand(QStringList);
    void sigExecCommand(QString);

public slots:
    void slotSelectFolder(QString folderPath);
    void slotSelectFile(QString filePath);

    void slotScmOff();
    void slotScmOn();

private slots:
    void _slotOnFileSelected();
    void _slotOnFolderSeleced();
    void _slotOnConfigAccepted();

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
