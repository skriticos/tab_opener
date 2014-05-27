#include "noteswidget.h"
#include "ui_noteswidget.h"

NotesWidget::NotesWidget(QWidget *parent) : QWidget(parent), ui(new Ui::NotesWidget)
{
    ui->setupUi(this);
}

// can't put this in the constructor, as it breaks the GUI builder
void NotesWidget::initWidget(DataStore *ds)
{
    this->ds = ds;

    QString state = ds->getGeneralValue("last_note_state");
    if(state == "file"){
        ui->btnFileNotes->setChecked(true);
        this->_setNotesText(ds->getFileNote(ds->getGeneralValue("selected_file")));
    } else if(state == "command"){
        ui->btnCmdNotes->setChecked(true);
        this->_setNotesText(ds->getCommandNote(ds->getGeneralValue("current_command")));
    } else {
        ui->btnGlobalNotes->setChecked(true);
        this->_setNotesText(ds->getGeneralValue("notes"));
    }
}

NotesWidget::~NotesWidget()
{
    delete ui;
}

void NotesWidget::slotUpdateFile(QString filePath)
{
    if(ui->btnFileNotes->isChecked()){
        if(filePath.isEmpty()){
            ui->notesView->blockSignals(true);
            ui->notesView->clear();
            ui->notesView->blockSignals(false);
        } else
            this->_setNotesText(ds->getFileNote(filePath));
    }
    this->filePath = filePath;
}

void NotesWidget::slotUpdateCmd(QString cmdStr)
{
    if(ui->btnCmdNotes->isChecked()){
        if(cmdStr.isEmpty()){
            ui->notesView->blockSignals(true);
            ui->notesView->clear();
            ui->notesView->blockSignals(false);
        } else
            this->_setNotesText(ds->getCommandNote(cmdStr));
    }
    this->commandStr = cmdStr;
}

void NotesWidget::_setNotesText(QString notesText)
{
    ui->notesView->blockSignals(true);
    ui->notesView->setPlainText(notesText);
    ui->notesView->blockSignals(false);
}

void NotesWidget::on_btnGlobalNotes_clicked(bool checked)
{
    if(!checked){
        ui->btnGlobalNotes->setChecked(true);
        return;
    }
    ds->setGeneralValue("last_note_state", "global");
    ui->btnFileNotes->setChecked(false);
    ui->btnCmdNotes->setChecked(false);
    this->_setNotesText(ds->getGeneralValue("notes"));
}

void NotesWidget::on_btnFileNotes_clicked(bool checked)
{
    if(!checked){
        ui->btnFileNotes->setChecked(true);
        return;
    }
    ds->setGeneralValue("last_note_state", "file");
    ui->btnGlobalNotes->setChecked(false);
    ui->btnCmdNotes->setChecked(false);
    this->_setNotesText(ds->getFileNote(ds->getGeneralValue("selected_file")));
}

void NotesWidget::on_btnCmdNotes_clicked(bool checked)
{
    if(!checked){
        ui->btnCmdNotes->setChecked(true);
        return;
    }
    ds->setGeneralValue("last_note_state", "command");
    ui->btnGlobalNotes->setChecked(false);
    ui->btnFileNotes->setChecked(false);
    this->_setNotesText(ds->getCommandNote(this->commandStr));
}

void NotesWidget::on_notesView_textChanged()
{
    if(ui->btnGlobalNotes->isChecked()){
        ds->setGeneralValue("notes", ui->notesView->toPlainText());
    } else if(ui->btnFileNotes->isChecked()){
        if(!this->filePath.isEmpty())
            ds->setFileNote(this->filePath, ui->notesView->toPlainText());
    } else if(ui->btnCmdNotes->isChecked()){
        ds->setCommandNote(this->commandStr, ui->notesView->toPlainText());
    }
}
