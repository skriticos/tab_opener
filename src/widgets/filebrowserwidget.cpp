#include "filebrowserwidget.h"
#include "ui_filebrowser.h"

FileBrowserWidget::FileBrowserWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FileBrowser)
{
    this->isInit = true;

    ui->setupUi(this);

    this->dirmodel = new QFileSystemModel(this);
    this->filemodel = new QFileSystemModel(this);

    this->dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    this->filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);

    ui->viewFolders->setModel(dirmodel);
    ui->viewFiles->setModel(filemodel);

    ui->viewFolders->setRootIndex(dirmodel->setRootPath("/"));
    ui->viewFolders->hideColumn(3);
    ui->viewFolders->hideColumn(2);
    ui->viewFolders->hideColumn(1);

    QItemSelectionModel *folderSelectionModel = ui->viewFolders->selectionModel();
    connect(folderSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(_slotOnFolderSeleced()));

    QItemSelectionModel *fileSelectionModel = ui->viewFiles->selectionModel();
    connect(fileSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(_slotOnFileSelected()));

    connect(ui->btnActPrimary, SIGNAL(clicked()), this, SLOT(slotOpenFilePrimary()));
    connect(ui->btnActSecondary, SIGNAL(clicked()), this, SLOT(slotOpenFileSeconday()));

    this->isInit = false;
}

FileBrowserWidget::~FileBrowserWidget()
{
    delete ui;
}

void FileBrowserWidget::initFileBrowser(DataStore *ds)
{
    this->isInit = true;

    this->ds = ds;

    if(ds->tblGeneral->contains("selected_file")
              && !ds->_getGeneralValue("selected_file").isEmpty()){
        this->slotSelectFile(ds->_getGeneralValue("selected_file"));
    } else if(ds->tblGeneral->contains("navigator_path")
              && !ds->_getGeneralValue("navigator_path").isEmpty()){
        this->slotSelectFolder(ds->_getGeneralValue("navigator_path"));
    } else {
        this->slotSelectFolder(QDir::homePath());
    }

    this->isInit = false;
}

void FileBrowserWidget::slotSelectFolder(QString folderPath)
{
    if(!folderPath.isEmpty()){
        ui->viewFolders->setCurrentIndex(this->dirmodel->index(folderPath));
    }
}

void FileBrowserWidget::slotSelectFile(QString filePath)
{
    QString baseFolder = QFileInfo(filePath).absolutePath();
    this->slotSelectFolder(baseFolder);

    if(!filePath.isEmpty()){
        ui->viewFiles->setCurrentIndex(this->filemodel->index(filePath));
    }

    emit this->sigFileSelected(filePath);
}

void FileBrowserWidget::slotOpenFilePrimary()
{
    QString selectedFile, extension, command;

    selectedFile = this->getSelectedFile();
    extension = selectedFile.split(".").last();

    command = ds->getExtActPri(extension) + " " + selectedFile;
    Util::execDetachedCommand(command);

    ds->setFile(selectedFile);

    emit this->sigFileOpened();
}

void FileBrowserWidget::slotOpenFileSeconday()
{
    QString selectedFile, extension, command;

    selectedFile = this->getSelectedFile();
    extension = selectedFile.split(".").last();

    command = ds->getExtActSec(extension) + " " + selectedFile;
    Util::execDetachedCommand(command);

    ds->setFile(selectedFile);

    emit this->sigFileOpened();
}

void FileBrowserWidget::slotScmOff()
{
    ui->btnScmCommit->setEnabled(false);
    ui->btnScmPull->setEnabled(false);
    ui->btnScmPush->setEnabled(false);
}

void FileBrowserWidget::slotScmOn()
{
    ui->btnScmCommit->setEnabled(true);
    ui->btnScmPull->setEnabled(true);
    ui->btnScmPush->setEnabled(true);
}

void FileBrowserWidget::slotTerminalEmulatorChanged(QString newTermEmulator)
{

}

void FileBrowserWidget::slotExtFileBrowserChanged(QString newExtFBrowser)
{

}

void FileBrowserWidget::_slotOnFileSelected()
{
    QString selectedFilePath = this->getSelectedFile();

    if(!this->isInit)
        ds->_setGeneralValue("selected_file", selectedFilePath);
    emit this->sigFileSelected(selectedFilePath);
}

void FileBrowserWidget::_slotOnFolderSeleced()
{
    CharmButton *charmButton;
    QLabel      *charmLabel;

    QString selectedFolderPath = this->getSelectedFolder();

    // set folder index and file view root
    ui->viewFolders->setCurrentIndex(dirmodel->index(selectedFolderPath));
    ui->viewFolders->setExpanded(dirmodel->index(selectedFolderPath), true);
    ui->viewFiles->setRootIndex(filemodel->setRootPath(selectedFolderPath));
    ui->viewFiles->selectionModel()->clearSelection();
    emit this->sigFileSelected("");

    // remove all charms
    for(int i=0; i<ui->charmLayout->count(); i++)
        ui->charmLayout->removeItem(ui->charmLayout->itemAt(i));

    for(int i=0; i<this->charmButtonList.size(); i++)
        charmButtonList.at(i)->deleteLater();
    for(int i=0; i<this->charmLabelList.size(); i++)
        charmLabelList.at(i)->deleteLater();

    this->charmButtonList.clear();
    this->charmLabelList.clear();

    // add root charm
    charmButton = new CharmButton(QDir::rootPath(), QDir::rootPath(), ui->charmContainer);
    ui->charmLayout->addWidget(charmButton);
    charmButtonList.append(charmButton);
    connect(charmButton, SIGNAL(sigCharmClicked(QString)), this, SLOT(slotSelectFolder(QString)));

    // add folder charms and separator labels
    QStringList charmParts = selectedFolderPath.split(QDir::separator());
    if (selectedFolderPath != QDir::rootPath()) {

        for (int i=1; i<charmParts.size(); i++){

            QStringList charmPartSL = charmParts.mid(1, i);
            QString charmPath = QDir::rootPath() + charmPartSL.join(QDir::separator());

            charmButton = new CharmButton(charmParts.at(i), charmPath, ui->charmContainer);
            this->charmButtonList.append(charmButton);
            connect(charmButton, SIGNAL(sigCharmClicked(QString)), this, SLOT(slotSelectFolder(QString)));

            if(i>1){
                charmLabel = new QLabel("/", ui->charmContainer);
                ui->charmLayout->addWidget(charmLabel);
                charmLabelList.append(charmLabel);
            }

            ui->charmLayout->addWidget(charmButton);
            charmButtonList.append(charmButton);
        }
    }

    // external interface
    if(!this->isInit)
        ds->_setGeneralValue("navigator_path", selectedFolderPath);
    emit this->sigFolderSelected(selectedFolderPath);
}

QString FileBrowserWidget::getSelectedFolder()
{
    QModelIndex index;
    QItemSelectionModel *selectionModel;

    selectionModel = ui->viewFolders->selectionModel();

    if(!selectionModel->selectedIndexes().isEmpty()){
        index = selectionModel->selectedIndexes().first();
        return dirmodel->fileInfo(index).absoluteFilePath();
    } else {
        return QString();
    }
}

QString FileBrowserWidget::getSelectedFile()
{
    QModelIndex index;
    QItemSelectionModel *selectionModel;

    selectionModel = ui->viewFiles->selectionModel();

    if(!selectionModel->selectedIndexes().isEmpty()) {
        index = selectionModel->selectedIndexes().first();
        return filemodel->fileInfo(index).absoluteFilePath();
    } else {
        return QString();
    }
}

void FileBrowserWidget::on_btnHome_clicked()
{
    this->slotSelectFolder(QDir::homePath());
}

void FileBrowserWidget::on_btnTerminal_clicked()
{
    QString selectedFolder, command;

    selectedFolder = this->getSelectedFolder();
    command = ds->_getGeneralValue("terminal_emulator") + " " + "\"" + selectedFolder + "\"";
    Util::execDetachedCommand(command);
    emit this->sigFileOpened();
}

void FileBrowserWidget::on_btnExtFileBrowser_clicked()
{
    QString selectedFolder, command;

    selectedFolder = this->getSelectedFolder();
    command = ds->_getGeneralValue("file_browser") + " " + "\"" + selectedFolder + "\"";
    Util::execDetachedCommand(command);
    emit this->sigFileOpened();
}

void FileBrowserWidget::on_btnPreferences_clicked()
{
    emit this->sigConfigClicked();
}

void FileBrowserWidget::on_btnScmPull_clicked()
{
    emit this->sigExecCommand(QString("git pull --all"));
}

void FileBrowserWidget::on_btnScmCommit_clicked()
{
    bool ok;
    QString commitMsg = QInputDialog::getText(
                this,
                "Commit message",
                "Commit msg:",
                QLineEdit::Normal,
                "",
                &ok);
    if(ok && !commitMsg.isEmpty()){
        QStringList cmdList;
        cmdList << "git add --all";
        cmdList << "git commit -am \"" + commitMsg + "\"";
        emit this->sigExecMultiCommand(cmdList);
    } else {
        if(ok)
            QMessageBox::warning(
                        this,
                        "Commit error",
                        "Can't commit with empty commit message. Aborting.",
                        QMessageBox::Ok
                        );
    }
}

void FileBrowserWidget::on_btnScmPush_clicked()
{
    emit this->sigExecCommand("git push --all");
}

void FileBrowserWidget::on_btnHelp_clicked()
{
    qDebug() << "not yet implemented";
}
