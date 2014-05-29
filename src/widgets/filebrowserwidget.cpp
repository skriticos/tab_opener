#include "filebrowserwidget.h"
#include "ui_filebrowser.h"

FileBrowserWidget::FileBrowserWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FileBrowser)
{
    ui->setupUi(this);

    ui->viewFolders->blockSignals(true);

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

    ui->viewFolders->blockSignals(false);

    QItemSelectionModel *folderSelectionModel = ui->viewFolders->selectionModel();
    connect(folderSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(_slotOnFolderSeleced()));

    QItemSelectionModel *fileSelectionModel = ui->viewFiles->selectionModel();
    connect(fileSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(_slotOnFileSelected()));

    connect(ui->btnActPrimary, SIGNAL(clicked()), this, SLOT(_slotOpenFilePrimary()));
    connect(ui->btnActSecondary, SIGNAL(clicked()), this, SLOT(_slotOpenFileSeconday()));
}

FileBrowserWidget::~FileBrowserWidget()
{
    delete ui;
}

void FileBrowserWidget::slotInitLocation(QString folderPath, QString filePath)
{
    if(!filePath.isEmpty()){
        // init file path
        this->slotSelectFile(filePath);
    } else if(!folderPath.isEmpty()) {
        // init folder path
        this->slotSelectFolder(folderPath);
    } else {
        this->slotSelectFolder(QDir::homePath());
    }
}

void FileBrowserWidget::slotSelectFolder(QString folderPath)
{
    if(!folderPath.isEmpty())
        ui->viewFolders->setCurrentIndex(this->dirmodel->index(folderPath));
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

void FileBrowserWidget::_slotOpenFilePrimary()
{
    emit this->sigRequestOpenFile(FileOpen::PRIMARY, _getSelectedFile());
}

void FileBrowserWidget::_slotOpenFileSeconday()
{
    emit this->sigRequestOpenFile(FileOpen::SECONDARY, _getSelectedFile());
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

void FileBrowserWidget::_slotOnFileSelected()
{
    emit this->sigFileSelected(_getSelectedFile());
}

void FileBrowserWidget::_slotOnFolderSeleced()
{
    CharmButton *charmButton;
    QLabel      *charmLabel;

    QString selectedFolderPath = this->_getSelectedFolder();

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
    emit this->sigFolderSelected(selectedFolderPath);
}

QString FileBrowserWidget::_getSelectedFolder()
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

QString FileBrowserWidget::_getSelectedFile()
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
    emit this->sigRequestOpenExtApp(FileOpen::TERMINAL, _getSelectedFolder());
}

void FileBrowserWidget::on_btnExtFileBrowser_clicked()
{
    emit this->sigRequestOpenExtApp(FileOpen::FILEBROWSER, _getSelectedFolder());
}

void FileBrowserWidget::on_btnPreferences_clicked()
{
    emit this->sigConfigClicked();
}

void FileBrowserWidget::on_btnScmPull_clicked()
{
    emit this->sigRequestScmPull();
}

void FileBrowserWidget::on_btnScmCommit_clicked()
{
    bool ok;
    QString commitMsg = QInputDialog::getText(
                this, "Commit message", "Commit msg:", QLineEdit::Normal, "", &ok);
    if(ok && !commitMsg.isEmpty()){
        emit this->sigRequestScmCommit(commitMsg);
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
    emit this->sigRequestScmPush();
}

void FileBrowserWidget::on_btnHelp_clicked()
{
    qDebug() << "not yet implemented";
}
