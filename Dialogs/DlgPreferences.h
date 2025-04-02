#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLocale>

#include "Global/LocalConfig.h"

namespace Ui {
class DlgPreferences;
}

class DlgPreferences : public QDialog {
    Q_OBJECT

public:
    explicit DlgPreferences(QWidget *parent);
    ~DlgPreferences();

private:
    void loadPreferences();
    void savePreferences();

private slots:
    void on_lwList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pbReset_clicked();

    void accept() override;

    void on_cbLogfileMode_currentIndexChanged(int index);

private:
    Ui::DlgPreferences *ui;
};

#endif // DLGPREFERENCES_H
