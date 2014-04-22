#ifndef CMDLIST_H
#define CMDLIST_H

#include <QWidget>

namespace Ui {
class CmdList;
}

class CmdList : public QWidget
{
    Q_OBJECT

public:
    explicit CmdList(QWidget *parent = 0);
    ~CmdList();

private:
    Ui::CmdList *ui;
};

#endif // CMDLIST_H
