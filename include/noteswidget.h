#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QWidget>
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
    void initWidget(DataStore *ds);
    ~NotesWidget();

public slots:
    void selectedFileChanged(QString filePath);
    void commandChanged(QString cmdStr);

protected:
    DataStore *ds;
    void setNotesText(QString notesText);
    void resetButtons();

private slots:
    void on_btnGlobalNotes_clicked(bool checked);
    void on_btnFileNotes_clicked(bool checked);
    void on_btnCmdNotes_clicked(bool checked);
    void on_notesView_textChanged();

private:
    Ui::NotesWidget *ui;
    QString filePath;
    QString commandStr;
};

#endif // NOTESWIDGET_H
