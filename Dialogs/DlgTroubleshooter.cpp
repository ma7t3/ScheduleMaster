#include "Dialogs\DlgTroubleshooter.h"
#include "ui_DlgTroubleshooter.h"

Troubleshooter::Troubleshooter(QWidget *parent, ProjectData *data) :
    QDialog(parent),
    ui(new Ui::Troubleshooter),
    projectData(data)
{
    ui->setupUi(this);
}

Troubleshooter::~Troubleshooter()
{
    delete ui;
}
