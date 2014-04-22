#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QShortcut>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_wpb_edit_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QShortcut *shortEsc;
};

#endif // MAINWINDOW_H
