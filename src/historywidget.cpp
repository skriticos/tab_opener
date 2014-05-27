#include "historywidget.h"
#include "ui_historywidget.h"

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent), ui(new Ui::HistoryWidget)
{
    ui->setupUi(this);

    this->type = History::UNDEFINED;

    HistoryButton *button;
    for(int i=0; i<10; i++){
        button = getRecentBtnAt(i);
        connect(button, SIGNAL(selected(QString)), this, SLOT(buttonSelected(QString)));
        button = getPopularBtnAt(i);
        connect(button, SIGNAL(selected(QString)), this, SLOT(buttonSelected(QString)));
    }

    connect(ui->control0, SIGNAL(clicked()), this, SLOT(onControl0Clicked()));
    connect(ui->control1, SIGNAL(clicked()), this, SLOT(onControl1Clicked()));
    connect(ui->control2, SIGNAL(clicked()), this, SLOT(onControl2Clicked()));
}

HistoryWidget::~HistoryWidget()
{
    delete ui;
}

void HistoryWidget::setType(History::WidgetType type)
{
    this->type = type;

    if(type == History::FILEHISTORY){
        ui->control0->setIcon(QIcon(":/images/rc/document-preview.png"));
        ui->control1->setIcon(QIcon(":/images/rc/text-editor.png"));
        ui->control2->setIcon(QIcon(":/images/rc/editcopy.png"));

    } else if(type == History::COMMANDHISTORY){
        ui->control0->setIcon(QIcon(":/images/rc/editcopy.png"));
        ui->control1->hide();
        ui->control2->hide();
    }
}

void HistoryWidget::updateWidget(QList<History::Entry> recentHistory,
                                 QList<History::Entry> popularHistory)
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    // reset
    for(int i=0; i<10; i++){
        getRecentBtnAt(i)->reset();
        getPopularBtnAt(i)->reset();
    }

    if(this->type == History::FILEHISTORY){
        this->updateFileHistory(recentHistory, popularHistory);
    } else if(this->type == History::COMMANDHISTORY) {
        this->updateCommandHistory(recentHistory, popularHistory);
    }
}

void HistoryWidget::fileSelected(QString filePath)
{
    if(filePath != this->selectedFile){
        this->selectedFile = filePath;
        emit this->idSelected(filePath); // select matching history buttons
    }
}

void HistoryWidget::commandSelected(QString commandString)
{
    if(commandString != this->selectedCommand){
        this->selectedCommand = commandString;
        emit this->idSelected(commandString + this->selectedWorkingDirectory);
    }
}

void HistoryWidget::workingDirectorySelected(QString workingDirectory)
{
    this->selectedWorkingDirectory = workingDirectory;
}

void HistoryWidget::buttonSelected(QString id)
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    emit this->idSelected(id); // if there is a button with same .. on in the other list, select it too, unselect others

    if(this->type == History::FILEHISTORY){
        this->selectedFile = id;
        emit this->selectedFileChanged(id);

    } else if(this->type == History::COMMANDHISTORY) {
        QString cmd = this->cmdIds.value(id).first();
        QString wd  = this->cmdIds.value(id).last();

        if(cmd != this->selectedCommand){
            this->selectedCommand = cmd;
            emit this->selectedCommandChanged(cmd);
        }
        if(wd != this->selectedWorkingDirectory){
            this->selectedWorkingDirectory = wd;
            emit this->selectedFolderChanged(wd);
        }
    }

}

void HistoryWidget::onControl0Clicked()
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    if(this->type == History::FILEHISTORY){ // execute primary action
        emit this->filePriActRequested(this->selectedFile);

    } else if(this->type == History::COMMANDHISTORY) { // copy to clipboard
        QApplication::clipboard()->setText(this->selectedCommand);
    }

}

void HistoryWidget::onControl1Clicked()
{
    Q_ASSERT(this->type == History::FILEHISTORY);

    emit this->fileSecActRequested(this->selectedFile);
}

void HistoryWidget::onControl2Clicked()
{
    Q_ASSERT(this->type == History::FILEHISTORY);

    QApplication::clipboard()->setText(this->selectedFile);
}

HistoryButton *HistoryWidget::getRecentBtnAt(int pos)
{
    return ui->recentHistoryContainer->findChild<HistoryButton*>("recent" + QString::number(pos));
}

HistoryButton *HistoryWidget::getPopularBtnAt(int pos)
{
    return ui->popularHistoryContainer->findChild<HistoryButton*>("popular" + QString::number(pos));
}

void HistoryWidget::updateFileHistory(QList<History::Entry> recentHistory,
                                      QList<History::Entry> popularHistory)
{
    HistoryButton *button;
    History::Entry historyEntry;
    QString entryId;

    // lambda function to compute file label (see c++11)
    auto fileLabel = [] (QString filePath) {
        if(filePath.size() > 35)
            return "..." + filePath.right(35);
        return filePath;
    };

    for(int i=0; i<recentHistory.size(); i++){
        historyEntry = recentHistory.at(i);
        entryId = historyEntry.filePath;
        button = getRecentBtnAt(i);
        button->setId(entryId);
        button->setText(fileLabel(historyEntry.filePath));
    }
    for(int i=0; i<popularHistory.size(); i++){
        historyEntry = popularHistory.at(i);
        entryId = historyEntry.filePath;
        button = getPopularBtnAt(i);
        button->setId(entryId);
        button->setText(fileLabel(historyEntry.filePath));
    }
}

void HistoryWidget::updateCommandHistory(QList<History::Entry> recentHistory,
                                         QList<History::Entry> popularHistory)
{
    HistoryButton *button;
    History::Entry historyEntry;
    QString entryId;

    auto cmdLabel = [] (QString cmdStr) {
        if(cmdStr.size() > 35)
            return "..." + cmdStr.right(35);
        return cmdStr;
    };

    for(int i=0; i<recentHistory.size(); i++){
        historyEntry = recentHistory.at(i);
        entryId = historyEntry.commandString + historyEntry.workingDirecotry;
        this->cmdIds.value(entryId, QStringList() << historyEntry.commandString << historyEntry.workingDirecotry);
        button = getRecentBtnAt(i);
        button->setId(entryId);
        button->setText(cmdLabel(historyEntry.commandString));
    }
    for(int i=0; i<popularHistory.size(); i++){
        historyEntry = popularHistory.at(i);
        entryId = historyEntry.commandString + historyEntry.workingDirecotry;
        this->cmdIds.value(entryId, QStringList() << historyEntry.commandString << historyEntry.workingDirecotry);
        button = getPopularBtnAt(i);
        button->setId(entryId);
        button->setText(cmdLabel(historyEntry.commandString));
    }
}
