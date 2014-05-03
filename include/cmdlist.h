#ifndef CMDLIST_H
#define CMDLIST_H

#include <QWidget>
#include <QClipboard>
#include <QDebug>

#include "commandbutton.h"
#include "datastore.h"

namespace Ui {
class CmdList;
}

class CmdList : public QWidget
{
    Q_OBJECT

public:
    explicit CmdList(QWidget *parent = 0);
    void init(DataStore *ds);
    void update();
    ~CmdList();

signals:
    void commandSelected(QString command, QString path);

public slots:
    void selected(QString cmd, QString wd, CommandButton *cb);

private slots:
    void on_wclab_copy_clicked();

private:
    Ui::CmdList *ui;

    QList<CommandButton*> recentCommandButtons;
    QList<CommandButton*> popularCommandButtons;
    CommandButton *selectedCommandButton;

    DataStore *ds;
};

#endif // CMDLIST_H
