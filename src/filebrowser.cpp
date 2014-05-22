#include "filebrowser.h"
#include "ui_filebrowser.h"

FileBrowser::FileBrowser(QWidget *parent) : QWidget(parent), ui(new Ui::FileBrowser)
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
            this, SLOT(onFolderSeleced()));

    QItemSelectionModel *fileSelectionModel = ui->viewFiles->selectionModel();
    connect(fileSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onFileSelected()));

    this->isInit = false;
}

FileBrowser::~FileBrowser()
{
    delete ui;
}

void FileBrowser::initFileBrowser(DataStore *ds)
{
    this->isInit = true;

    this->ds = ds;

    if(ds->tblGeneral->contains("navigator_path"))
        this->setSelectedFolder(ds->getGeneralValue("navigator_path"));
    else
        this->setSelectedFolder(QDir::homePath());

    this->isInit = false;
}

void FileBrowser::setSelectedFolder(QString folderPath)
{
    if(!folderPath.isEmpty()){
        ui->viewFolders->setCurrentIndex(this->dirmodel->index(folderPath));
    }
}

void FileBrowser::setSelectedFile(QString filePath)
{
    if(!filePath.isEmpty()){
        ui->viewFiles->setCurrentIndex(this->filemodel->index(filePath));
    }
}

void FileBrowser::onFileSelected()
{
    QString selectedFilePath = this->getSelectedFile();

    if(!this->isInit)
        ds->setGeneralValue("selected_file", selectedFilePath);
    emit this->fileSelected(selectedFilePath);

    qDebug() << "file selected" << selectedFilePath;
}

void FileBrowser::onFolderSeleced()
{
    QString selectedFolderPath = this->getSelectedFolder();

    // set folder index and file view root
    ui->viewFolders->setCurrentIndex(dirmodel->index(selectedFolderPath));
    ui->viewFolders->setExpanded(dirmodel->index(selectedFolderPath), true);
    ui->viewFiles->setRootIndex(filemodel->setRootPath(selectedFolderPath));

    // update charms
    // TBD

    if(!this->isInit)
        ds->setGeneralValue("navigator_path", selectedFolderPath);
    emit this->folderSelected(selectedFolderPath);

    qDebug() << "folder selected" << selectedFolderPath;
}

QString FileBrowser::getSelectedFolder()
{
    QModelIndex index = ui->viewFolders->selectionModel()->selectedIndexes().first();
    return dirmodel->fileInfo(index).absoluteFilePath();
}

QString FileBrowser::getSelectedFile()
{
    QModelIndex index = ui->viewFiles->selectionModel()->selectedIndexes().first();
    return filemodel->fileInfo(index).absoluteFilePath();
}
