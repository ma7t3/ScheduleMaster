#ifndef DLGOMSIIMPORT_H
#define DLGOMSIIMPORT_H

#include <QDialog>

namespace Ui {
class DlgOmsiImport;
}

class DlgOmsiImport : public QDialog {
    Q_OBJECT

public:
    explicit DlgOmsiImport(QWidget *parent = nullptr);
    ~DlgOmsiImport();

private slots:
    void on_pbDirBrowse_clicked();
    void refresh();
    void refreshCount();

    void on_phTripsSelectAll_clicked();

    void on_pbTripsDeselectAll_clicked();

    void on_pbTripsToggleSelected_clicked();

    void on_pbLinesSelectAll_clicked();

    void on_pbLinesDeselectAll_clicked();

    void on_pbLinesToggleSelected_clicked();

private:
    Ui::DlgOmsiImport *ui;

    QString countStr = "Count: %1 (%2 selected)";
};

#endif // DLGOMSIIMPORT_H
