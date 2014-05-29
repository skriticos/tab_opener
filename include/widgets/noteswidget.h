#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QtWidgets>

#include "hoverbutton.h"
#include "defines.h"

namespace Ui
{
    class NotesWidget;
}

class NotesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesWidget(QWidget *parent = 0);
    ~NotesWidget();

signals:
    void sigGlobalNoteChanged(QString noteText);
    void sigFileNoteChanged(QString filePath, QString noteText);
    void sigCmdNoteChanged(QString cmdStr, QString noteText);
    void sigSelectionChanged(QString newSelection);

public slots:
    void slotInitSelection(QString selection);
    void slotInitGlobalNote(QString noteText);
    void slotUpdateFileNote(QString filePath, QString noteText);
    void slotUpdateCommandNote(QString cmdStr, QString noteText);

private slots:
    void on_btnGlobalNotes_clicked(bool checked);
    void on_btnFileNotes_clicked(bool checked);
    void on_btnCmdNotes_clicked(bool checked);
    void on_notesView_textChanged();

private:
    void _setNotesText(QString notesText);

    Ui::NotesWidget *ui;

    // cache for current selection
    QString filePath;
    QString commandStr;

    QString globalNote;
    QString fileNote;
    QString commandNote;
};

#endif // NOTESWIDGET_H
