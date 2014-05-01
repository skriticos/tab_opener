#ifndef FILELIST_H
#define FILELIST_H

#include <QWidget>
#include "filebutton.h"
#include "datastore.h"

namespace Ui {
class FileList;
}

class FileList : public QWidget
{
    Q_OBJECT

public:
    explicit FileList(QWidget *parent = 0);
    void init(DataStore *ds);
    void update();
    ~FileList();

public slots:
    void buttonSelected(QString path, FileButton *butt);

private slots:
    void on_wflab_file_view_clicked();
    void on_wflab_file_edit_clicked();

private:
    Ui::FileList *ui;

    QList<FileButton*> recentButtons;
    QList<FileButton*> popularButtons;
    FileButton *selectedButton;

    QString selectedFilePath;

    DataStore *ds;
};

#endif // FILELIST_H
