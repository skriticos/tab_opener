#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QtWidgets>

#include "defines.h"

namespace Ui {
    class ConfigWidget;
}

class ConfigWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = 0);
    ~ConfigWidget();

signals:
    void sigPresetsChanged(QStringList presetList);
    void sigExtensionChanged(Config::ExtensionEntry extEntry);
    void sigExtensionDeleted(QString extStr);
    void sigTerminalChanged(QString terminalCmd);
    void sigExtFileBrowserChanged(QString fbrowserCmd);

public slots:
    void slotInitConfig(QStringList presetList,
                    QList<Config::ExtensionEntry> extList,
                    QString terminalCmd,
                    QString fbrowserCmd);

private slots:
    void on_btnCommit_clicked();
    void on_btnDelete_clicked();
    void on_extensionlist_itemSelectionChanged();
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_extension_textChanged(const QString &arg1);

private:
    void _reloadSettings();
    QLineEdit *_getPresetInput(int pos);

    Ui::ConfigWidget *ui;

    // local config cache
    QMap<QString, QListWidgetItem*> extWidgetMap;
    QMap<QString, Config::ExtensionEntry> extIndex;
    QString terminalCmd;
    QString fbrowserCmd;
    QStringList presetList;
};

#endif // CONFIGWIDGET_H
