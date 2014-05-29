#include "noteswidget.h"
#include "ui_noteswidget.h"

NotesWidget::NotesWidget(QWidget *parent) : QWidget(parent), ui(new Ui::NotesWidget)
{
    ui->setupUi(this);
}

NotesWidget::~NotesWidget()
{
    delete ui;
}

void NotesWidget::slotInitSelection(QString selection)
{
    if(selection == FILE) {
        ui->btnFileNotes->setChecked(true);
    } else if(selection == COMMAND) {
        ui->btnCmdNotes->setChecked(true);
    } else if(selection == GLOBAL) {
        ui->btnGlobalNotes->setChecked(true);
    } else {
        Q_ASSERT(false);
    }
}

void NotesWidget::slotInitGlobalNote(QString noteText)
{
    this->globalNote = noteText;

    if(ui->btnGlobalNotes->isChecked()) {
        _setNotesText(noteText);
    }
}

void NotesWidget::slotUpdateFileNote(QString filePath, QString noteText)
{
    this->filePath = filePath;
    this->fileNote = noteText;

    if(ui->btnFileNotes->isChecked()) {
        if(filePath.isEmpty()) {
            ui->notesView->setPlainText("No file selected.");
            ui->notesView->setEnabled(false);
        } else {
            ui->notesView->setEnabled(true);
            _setNotesText(noteText);
        }
    }
}

void NotesWidget::slotUpdateCommandNote(QString cmdStr, QString noteText)
{
    this->commandStr = cmdStr;
    this->commandNote = noteText;

    if(ui->btnCmdNotes->isChecked()) {
        if(cmdStr.isEmpty()) {
            ui->notesView->setPlainText("No command selected.");
            ui->notesView->setEnabled(false);
        } else {
            ui->notesView->setEnabled(true);
            _setNotesText(noteText);
        }
    }
}

void NotesWidget::_setNotesText(QString notesText)
{
    ui->notesView->blockSignals(true);
    ui->notesView->setPlainText(notesText);
    ui->notesView->blockSignals(false);
}

void NotesWidget::on_btnGlobalNotes_clicked(bool checked)
{
    if(checked) {
        ui->btnFileNotes->setChecked(false);
        ui->btnCmdNotes->setChecked(false);
        ui->notesView->setEnabled(true);
        _setNotesText(this->globalNote);
        emit this->sigSelectionChanged(GLOBAL);
    } else {
        ui->btnGlobalNotes->setChecked(true);
    }
}

void NotesWidget::on_btnFileNotes_clicked(bool checked)
{
    if(checked) {
        ui->btnGlobalNotes->setChecked(false);
        ui->btnCmdNotes->setChecked(false);

        if(this->filePath.isEmpty()) {
            ui->notesView->setPlainText("No file selected.");
            ui->notesView->setEnabled(false);
        } else {
            ui->notesView->setEnabled(true);
            _setNotesText(this->fileNote);
        }

        emit this->sigSelectionChanged(FILE);
    } else {
        ui->btnFileNotes->setChecked(true);
    }
}

void NotesWidget::on_btnCmdNotes_clicked(bool checked)
{
    if(checked) {
        ui->btnGlobalNotes->setChecked(false);
        ui->btnFileNotes->setChecked(false);

        if(this->commandStr.isEmpty()) {
            ui->notesView->setPlainText("No command selected.");
            ui->notesView->setEnabled(false);
        } else {
            ui->notesView->setEnabled(true);
            _setNotesText(this->commandNote);
        }

        emit this->sigSelectionChanged(COMMAND);
    } else {
        ui->btnCmdNotes->setChecked(true);
    }
}

void NotesWidget::on_notesView_textChanged()
{
    if(ui->btnGlobalNotes->isChecked()) {
        this->globalNote = ui->notesView->toPlainText();
        emit this->sigGlobalNoteChanged(ui->notesView->toPlainText());
    } else if(ui->btnFileNotes->isChecked()) {
        if(!this->filePath.isEmpty()) {
            this->fileNote = ui->notesView->toPlainText();
            emit this->sigFileNoteChanged(this->filePath, ui->notesView->toPlainText());
        }
    } else if(ui->btnCmdNotes->isChecked()) {
        if(!this->commandStr.isEmpty()) {
            this->commandNote = ui->notesView->toPlainText();
            emit this->sigCmdNoteChanged(this->commandStr, ui->notesView->toPlainText());
        }
    }
}
