#include "dlgprojectsettings.h"
#include "App/global.h"
#include "ui_dlgprojectsettings.h"


#include <QMessageBox>
#include <QFileDialog>

DlgProjectSettings::DlgProjectSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgProjectSettings),
    m_icon("")
{
    ui->setupUi(this);

    QObject::connect(ui->pbSelectIcon, SIGNAL(clicked()), this, SLOT(actionSelectIcon()));

    QObject::connect(ui->cbMonday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbTuesday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbWednesday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbThursday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbFriday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbSaturday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbSunday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbHoliday, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbSchool, SIGNAL(clicked()), this, SLOT(saveDays()));
    QObject::connect(ui->cbNoSchool, SIGNAL(clicked()), this, SLOT(saveDays()));
}

DlgProjectSettings::~DlgProjectSettings()
{
    delete ui;
}

void DlgProjectSettings::setDisplayName(QString name) { ui->leDisplayName->setText(name); }
void DlgProjectSettings::setShortName(QString name)   { ui->leShortName->setText(name); }
void DlgProjectSettings::setNames(QString displayName, QString shortName) {
    setDisplayName(displayName);
    setShortName(shortName);
}
void DlgProjectSettings::setIcon(QString fileName) {
    m_icon.setFileName(fileName);
    ui->leIconPath->setText(fileName);
    reloadIconPreview();
}

void DlgProjectSettings::setDayTypes(QList<DayType> dayTypes) {
    for(int i = 0; i < dayTypes.count(); i++)
        tableReference << new DayType(dayTypes[i]);

    refreshDayTypesTable();
}

QString DlgProjectSettings::displayName()       { return ui->leDisplayName->text(); }
QString DlgProjectSettings::shortName()         { return ui->leShortName->text(); }
QString DlgProjectSettings::icon()              { return m_icon.fileName(); }
QList<DayType *> DlgProjectSettings::dayTypes() { return tableReference; }

void DlgProjectSettings::actionSelectIcon() {
    QString filename = QFileDialog::getOpenFileName(this, "", "", tr("Images (*.png *.jpg *.bmp *.ico)"));
    if(filename.isEmpty())
        return;

    QFile f(filename);

    if(!f.exists()) {
        QMessageBox::critical(this, tr("File not found"), tr("The requested file was not found!"));
        return;
    }

    if(!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Couldn't open file"), tr("<p><b>The requested file couldn't be opened:</b></p><p>%1</p>").arg(f.errorString()));
        return;
    }
    f.close();

    ui->leIconPath->setText(f.fileName());

    m_icon.setFileName(f.fileName());
    reloadIconPreview();
}

void DlgProjectSettings::reloadIconPreview() {
    QPixmap pixmap(m_icon.fileName());
    pixmap = pixmap.scaled(92, 92, Qt::KeepAspectRatio);
    ui->lIcon->setGeometry(ui->lIcon->x(), ui->lIcon->y(), pixmap.width(), pixmap.height());
    ui->lIcon->setPixmap(pixmap);
}

void DlgProjectSettings::refreshDayTypesTable() {
    refreshingDayTypes = true;
    ui->lwDays->clear();
    for(int i = 0; i < tableReference.count(); i++) {
        DayType *d = tableReference[i];
        ui->lwDays->insertItem(i, new QListWidgetItem(d->name()));

        if(d == m_currentDayType)
            ui->lwDays->setCurrentRow(i);
    }

    refreshingDayTypes = false;
}

void DlgProjectSettings::refreshTayTypesDetails() {
    if(!m_currentDayType) {
        ui->leDayTypesName->setText("");
        ui->cbMonday->setChecked(false);
        ui->cbTuesday->setChecked(false);
        ui->cbWednesday->setChecked(false);
        ui->cbThursday->setChecked(false);
        ui->cbFriday->setChecked(false);
        ui->cbSaturday->setChecked(false);
        ui->cbSunday->setChecked(false);
        ui->cbHoliday->setChecked(false);
        ui->cbSchool->setChecked(false);
        ui->cbNoSchool->setChecked(false);

        ui->leDayTypesName->setEnabled(false);
        ui->cbMonday->setEnabled(false);
        ui->cbTuesday->setEnabled(false);
        ui->cbWednesday->setEnabled(false);
        ui->cbThursday->setEnabled(false);
        ui->cbFriday->setEnabled(false);
        ui->cbSaturday->setEnabled(false);
        ui->cbSunday->setEnabled(false);
        ui->cbHoliday->setEnabled(false);
        ui->cbSchool->setEnabled(false);
        ui->cbNoSchool->setEnabled(false);
        return;
    }

    ui->leDayTypesName->setText(m_currentDayType->name());
    ui->cbMonday->setChecked(m_currentDayType->monday());
    ui->cbTuesday->setChecked(m_currentDayType->tuesday());
    ui->cbWednesday->setChecked(m_currentDayType->wednesday());
    ui->cbThursday->setChecked(m_currentDayType->thursday());
    ui->cbFriday->setChecked(m_currentDayType->friday());
    ui->cbSaturday->setChecked(m_currentDayType->saturday());
    ui->cbSunday->setChecked(m_currentDayType->sunday());
    ui->cbHoliday->setChecked(m_currentDayType->holiday());
    ui->cbSchool->setChecked(m_currentDayType->school());
    ui->cbNoSchool->setChecked(m_currentDayType->noSchool());

    ui->leDayTypesName->setEnabled(true);
    ui->cbMonday->setEnabled(true);
    ui->cbTuesday->setEnabled(true);
    ui->cbWednesday->setEnabled(true);
    ui->cbThursday->setEnabled(true);
    ui->cbFriday->setEnabled(true);
    ui->cbSaturday->setEnabled(true);
    ui->cbSunday->setEnabled(true);
    ui->cbHoliday->setEnabled(true);
    ui->cbSchool->setEnabled(true);
    ui->cbNoSchool->setEnabled(true);
}

void DlgProjectSettings::on_lwDays_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);
    if(refreshingDayTypes)
        return;

    if(!current)
        m_currentDayType = nullptr;
    else
        m_currentDayType = tableReference[ui->lwDays->row(current)];

    refreshTayTypesDetails();
}

void DlgProjectSettings::on_leDayTypesName_textEdited(const QString &arg1) {
    if(m_currentDayType) {
        m_currentDayType->setName(arg1);
        refreshDayTypesTable();
    }
}


void DlgProjectSettings::saveDays() {
    if(!m_currentDayType)
        return;

    m_currentDayType->setMonday(ui->cbMonday->isChecked());
    m_currentDayType->setTuesday(ui->cbTuesday->isChecked());
    m_currentDayType->setWednesday(ui->cbWednesday->isChecked());
    m_currentDayType->setThursday(ui->cbThursday->isChecked());
    m_currentDayType->setFriday(ui->cbFriday->isChecked());
    m_currentDayType->setSaturday(ui->cbSaturday->isChecked());
    m_currentDayType->setSunday(ui->cbSunday->isChecked());
    m_currentDayType->setHoliday(ui->cbHoliday->isChecked());
    m_currentDayType->setSchool(ui->cbSchool->isChecked());
    m_currentDayType->setNoSchool(ui->cbNoSchool->isChecked());

    refreshDayTypesTable();
}


void DlgProjectSettings::on_pbDayNew_clicked() {
    DayType *d = new DayType(global::getNewID(), "", 0);
    tableReference << d;
    m_currentDayType = d;
    refreshDayTypesTable();
    refreshTayTypesDetails();
}


void DlgProjectSettings::on_pbDaysDelete_clicked() {
    if(!m_currentDayType)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete day type"), tr("<p><b>Do you really want to delete this dayType?</b></p><p>%1</p>").arg(m_currentDayType->name()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == m_currentDayType) {
            tableReference.remove(i);
            m_currentDayType = nullptr;
            refreshDayTypesTable();
            refreshTayTypesDetails();
            return;
        }
    }
}


void DlgProjectSettings::on_pbDaysUp_clicked() {
    int index = -1;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == m_currentDayType) {
            index = i - 1;
        }
    }

    if(index < 0 || index >= tableReference.count())
        return;

    tableReference.remove(index + 1);
    tableReference.insert(index, m_currentDayType);

    refreshDayTypesTable();
}


void DlgProjectSettings::on_pbDaysDown_clicked() {
    int index = -1;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == m_currentDayType) {
            index = i + 1;
        }
    }

    if(index < 0 || index >= tableReference.count())
        return;

    tableReference.remove(index - 1);
    tableReference.insert(index, m_currentDayType);

    refreshDayTypesTable();
}

