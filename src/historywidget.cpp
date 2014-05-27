#include "historywidget.h"
#include "ui_historywidget.h"

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent), ui(new Ui::HistoryWidget)
{
    ui->setupUi(this);

    this->type = History::UNDEFINED;

    HistoryButton *button;
    for(int i=0; i<10; i++){
        button = _getRecentBtnAt(i);
        connect(button, SIGNAL(selected(QString)), this, SLOT(_buttonSelected(QString)));
        button = _getPopularBtnAt(i);
        connect(button, SIGNAL(selected(QString)), this, SLOT(_buttonSelected(QString)));
    }

    connect(ui->control0, SIGNAL(clicked()), this, SLOT(_onControl0Clicked()));
    connect(ui->control1, SIGNAL(clicked()), this, SLOT(_onControl1Clicked()));
    connect(ui->control2, SIGNAL(clicked()), this, SLOT(_onControl2Clicked()));
}

HistoryWidget::~HistoryWidget()
{
    delete ui;
}

void HistoryWidget::iSetType(History::WidgetType type)
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

void HistoryWidget::iUpdateWidget(QList<History::Entry> recentHistory,
                                 QList<History::Entry> popularHistory)
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    // reset
    for(int i=0; i<10; i++){
        _getRecentBtnAt(i)->reset();
        _getPopularBtnAt(i)->reset();
    }

    if(this->type == History::FILEHISTORY){
        this->_updateFileHistory(recentHistory, popularHistory);
    } else if(this->type == History::COMMANDHISTORY) {
        this->_updateCommandHistory(recentHistory, popularHistory);
    }
}

void HistoryWidget::iFileSelected(QString filePath)
{
    if(filePath != this->selectedFile){
        this->selectedFile = filePath;
        emit this->sigIdSelected(filePath); // select matching history buttons
    }
}

void HistoryWidget::iCommandSelected(QString commandString)
{
    if(commandString != this->selectedCommand){
        this->selectedCommand = commandString;
        emit this->sigIdSelected(commandString + this->selectedWorkingDirectory);
    }
}

void HistoryWidget::iWorkingDirectorySelected(QString workingDirectory)
{
    this->selectedWorkingDirectory = workingDirectory;
}

void HistoryWidget::_buttonSelected(QString id)
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    emit this->sigIdSelected(id); // if there is a button with same .. on in the other list, select it too, unselect others

    if(this->type == History::FILEHISTORY){
        this->selectedFile = id;
        emit this->sigSelectedFileChanged(id);

    } else if(this->type == History::COMMANDHISTORY) {
        QString cmd = this->cmdIds.value(id).first();
        QString wd  = this->cmdIds.value(id).last();

        if(cmd != this->selectedCommand){
            this->selectedCommand = cmd;
            emit this->sigSelectedCommandChanged(cmd);
        }
        if(wd != this->selectedWorkingDirectory){
            this->selectedWorkingDirectory = wd;
            emit this->sigSelectedFolderChanged(wd);
        }
    }

}

void HistoryWidget::_onControl0Clicked()
{
    Q_ASSERT(this->type != History::UNDEFINED); // triggers if we forget to set type

    if(this->type == History::FILEHISTORY){ // execute primary action
        emit this->sigFilePriActRequested(this->selectedFile);

    } else if(this->type == History::COMMANDHISTORY) { // copy to clipboard
        QApplication::clipboard()->setText(this->selectedCommand);
    }

}

void HistoryWidget::_onControl1Clicked()
{
    Q_ASSERT(this->type == History::FILEHISTORY);

    emit this->sigFileSecActRequested(this->selectedFile);
}

void HistoryWidget::_onControl2Clicked()
{
    Q_ASSERT(this->type == History::FILEHISTORY);

    QApplication::clipboard()->setText(this->selectedFile);
}

HistoryButton *HistoryWidget::_getRecentBtnAt(int pos)
{
    return ui->recentHistoryContainer->findChild<HistoryButton*>("recent" + QString::number(pos));
}

HistoryButton *HistoryWidget::_getPopularBtnAt(int pos)
{
    return ui->popularHistoryContainer->findChild<HistoryButton*>("popular" + QString::number(pos));
}

void HistoryWidget::_updateFileHistory(QList<History::Entry> recentHistory,
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
        button = _getRecentBtnAt(i);
        button->setId(entryId);
        button->setText(fileLabel(historyEntry.filePath));
    }
    for(int i=0; i<popularHistory.size(); i++){
        historyEntry = popularHistory.at(i);
        entryId = historyEntry.filePath;
        button = _getPopularBtnAt(i);
        button->setId(entryId);
        button->setText(fileLabel(historyEntry.filePath));
    }
}

void HistoryWidget::_updateCommandHistory(QList<History::Entry> recentHistory,
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
        button = _getRecentBtnAt(i);
        button->setId(entryId);
        button->setText(cmdLabel(historyEntry.commandString));
    }
    for(int i=0; i<popularHistory.size(); i++){
        historyEntry = popularHistory.at(i);
        entryId = historyEntry.commandString + historyEntry.workingDirecotry;
        this->cmdIds.value(entryId, QStringList() << historyEntry.commandString << historyEntry.workingDirecotry);
        button = _getPopularBtnAt(i);
        button->setId(entryId);
        button->setText(cmdLabel(historyEntry.commandString));
    }
}
