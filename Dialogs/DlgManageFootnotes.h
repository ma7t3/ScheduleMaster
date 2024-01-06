#ifndef DLGMANAGEFOOTNOTES_H
#define DLGMANAGEFOOTNOTES_H

#include <QDialog>

#include "ProjectData/projectdata.h"

namespace Ui {
class DlgManageFootnotes;
}

class DlgManageFootnotes : public QDialog {
    Q_OBJECT

public:
    explicit DlgManageFootnotes(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~DlgManageFootnotes();

private slots:
    void on_pbNew_clicked();

private:
    Ui::DlgManageFootnotes *ui;

    ProjectData *projectData;
};

#endif // DLGMANAGEFOOTNOTES_H
