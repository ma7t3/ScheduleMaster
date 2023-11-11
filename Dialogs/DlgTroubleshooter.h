#ifndef TROUBLESHOOTER_H
#define TROUBLESHOOTER_H

#include <QDialog>
#include "ProjectData\projectdata.h"

namespace Ui {
class Troubleshooter;
}

class Troubleshooter : public QDialog
{
    Q_OBJECT

public:
    explicit Troubleshooter(QWidget *parent = nullptr, ProjectData* = new ProjectData);
    ~Troubleshooter();

private:
    Ui::Troubleshooter *ui;
    ProjectData *projectData;
};

#endif // TROUBLESHOOTER_H
