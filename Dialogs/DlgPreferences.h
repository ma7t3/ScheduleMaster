#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>

namespace Ui {
class DlgPreferences;
}

class DlgPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPreferences(QWidget *parent = nullptr);
    ~DlgPreferences();

private slots:
    void on_DlgPreferences_accepted();
    void on_DlgPreferences_rejected();

    void on_pbDefaultProjectLocationBrowse_clicked();

    void refreshStylePreview(int index);

private:
    Ui::DlgPreferences *ui;
};

#endif // DLGPREFERENCES_H
