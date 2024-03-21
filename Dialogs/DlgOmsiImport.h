#ifndef DLGOMSIIMPORT_H
#define DLGOMSIIMPORT_H

#include <QDialog>
#include <QFuture>

#include "Plugins/PlgOmsiImport.h"
#include "ProjectData/projectdata.h"

namespace Ui {
class DlgOmsiImport;
}

class DlgOmsiImport : public QDialog {
    Q_OBJECT

public:
    explicit DlgOmsiImport(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~DlgOmsiImport();

private slots:
    void closeEvent(QCloseEvent *) override;

    void on_pbDirBrowse_clicked();
    void refresh();
    void refreshCount();

    void on_phTripsSelectAll_clicked();
    void on_pbTripsDeselectAll_clicked();
    void on_pbTripsToggleSelected_clicked();
    void on_pbLinesSelectAll_clicked();
    void on_pbLinesDeselectAll_clicked();
    void on_pbLinesToggleSelected_clicked();
    void on_pbStart_clicked();
    void refreshProgress(int, QString);
    void finish();

private:
    Ui::DlgOmsiImport *ui;
    PlgOmsiImport *_importer;
    ProjectData *projectData;
};

#endif // DLGOMSIIMPORT_H
