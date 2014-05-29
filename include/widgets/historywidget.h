#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QtCore>
#include <QtWidgets>
#include <QClipboard>

#include "history.h"
#include "historybutton.h"
#include "defines.h"

namespace Ui
{
    class HistoryWidget;
}

/**
 * @brief File and Command history widgets
 *
 * Note: This is a dual purpose class: file and command history.
 *       I wanted to avoid mucking around with even more classes and abstraction
 *       for this relatively trivial case, so we just check the History::WidgetType flag
 *       for specific operations and use specific types.
 * Note: The id that is used in the history buttons is:
 *       - for file history the full file path
 *       - for command history the full command plus the working directory
 *         - there is a revese lookup map that is used to interpret the return signals (in buttonSelected)
 */
class HistoryWidget : public QWidget
{
    Q_OBJECT

public:

    explicit HistoryWidget(QWidget *parent = 0);
    ~HistoryWidget();

    void setType(History::WidgetType type);

signals:
    void sigSelectedFileChanged(QString path);
    void sigSelectedCommandChanged(QString command);
    void sigSelectedFolderChanged(QString workingDirectory);
    void sigRequestOpenFile(FileOpen::OpenType openType, QString filePath);
    void sigIdSelected(QString id); // gui update signal for child widgets

public slots:
    void slotUpdateWidget(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void slotFileSelected(QString filePath);
    void slotCommandSelected(QString commandString);
    void slotWorkingDirectorySelected(QString workingDirectory);

private slots:
    void _slotButtonSelected(QString id);

    void _slotControl0Clicked();
    void _slotControl1Clicked();
    void _slotControl2Clicked();

private:
    HistoryButton *_getRecentBtnAt(int pos);
    HistoryButton *_getPopularBtnAt(int pos);

    void _updateFileHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);
    void _updateCommandHistory(QList<History::Entry> recentHistory, QList<History::Entry> popularHistory);

    Ui::HistoryWidget *ui;

    History::WidgetType type;

    QMap<QString, QStringList> cmdIds;
    QStringList commandList;
    QStringList historyFilePathList;

    QString selectedFile;
    QString selectedCommand;
    QString selectedWorkingDirectory;
};

#endif // HISTORYWIDGET_H
