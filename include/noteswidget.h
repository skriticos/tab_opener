#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QtWidgets>
#include "datastore.h"
#include "hoverbutton.h"

namespace Ui {
    class NotesWidget;
}

class NotesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesWidget(QWidget *parent = 0);
    ~NotesWidget();

    void initWidget(DataStore *ds);

public slots:
    void slotUpdateFile(QString filePath);
    void slotUpdateCmd(QString cmdStr);

private slots:
    void on_btnGlobalNotes_clicked(bool checked);
    void on_btnFileNotes_clicked(bool checked);
    void on_btnCmdNotes_clicked(bool checked);
    void on_notesView_textChanged();

private:
    DataStore *ds;
    void _setNotesText(QString notesText);
    void _resetButtons();

    Ui::NotesWidget *ui;
    QString filePath;
    QString commandStr;
};

#endif // NOTESWIDGET_H
