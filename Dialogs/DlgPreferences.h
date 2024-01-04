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

private:
    Ui::DlgPreferences *ui;
};

#endif // DLGPREFERENCES_H
