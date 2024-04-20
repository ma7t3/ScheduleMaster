#include "dlgprojectsettings.h"
#include "App/global.h"
#include "ui_dlgprojectsettings.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

DlgProjectSettings::DlgProjectSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgProjectSettings),
    _icon("")
{
    ui->setupUi(this);

    QObject::connect(ui->pbSelectIcon, SIGNAL(clicked()), this, SLOT(actionSelectIcon()));

    ui->tabWidget->setCurrentIndex(0);

    ui->leDayTypesName->setEnabled(false);
    ui->daySelector->setEnabled(false);
    ui->daySelector->setWeekDays(WeekDays(this, 0));
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
    _icon.setFileName(fileName);
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
QString DlgProjectSettings::icon()              { return _icon.fileName(); }
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

    _icon.setFileName(f.fileName());
    reloadIconPreview();
}

void DlgProjectSettings::reloadIconPreview() {
    QPixmap pixmap(_icon.fileName());
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

        if(d == _currentDayType)
            ui->lwDays->setCurrentRow(i);
    }

    refreshingDayTypes = false;
}

void DlgProjectSettings::refreshTayTypesDetails() {
    if(!_currentDayType) {
        ui->leDayTypesName->setText("");
        ui->daySelector->setWeekDays(WeekDays(this, 0));

        ui->leDayTypesName->setEnabled(false);
        ui->daySelector->setEnabled(false);

        return;
    }

    ui->leDayTypesName->setText(_currentDayType->name());
    ui->daySelector->setWeekDays(WeekDays(*_currentDayType));

    ui->leDayTypesName->setEnabled(true);
    ui->daySelector->setEnabled(true);
}

void DlgProjectSettings::on_lwDays_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);
    if(refreshingDayTypes)
        return;

    if(!current)
        _currentDayType = nullptr;
    else
        _currentDayType = tableReference[ui->lwDays->row(current)];

    refreshTayTypesDetails();
}

void DlgProjectSettings::on_leDayTypesName_textEdited(const QString &arg1) {
    if(_currentDayType) {
        _currentDayType->setName(arg1);
        refreshDayTypesTable();
    }
}

void DlgProjectSettings::on_pbDayNew_clicked() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Create New Day Type"), tr("Enter day type name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    DayType *d = new DayType(nullptr, global::getNewID(), name, 0);
    tableReference << d;
    _currentDayType = d;
    refreshDayTypesTable();
    refreshTayTypesDetails();
}


void DlgProjectSettings::on_pbDaysDelete_clicked() {
    if(!_currentDayType)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete day type"), tr("<p><b>Do you really want to delete this dayType?</b></p><p>%1</p>").arg(_currentDayType->name()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == _currentDayType) {
            tableReference.remove(i);
            _currentDayType = nullptr;
            refreshDayTypesTable();
            refreshTayTypesDetails();
            return;
        }
    }
}


void DlgProjectSettings::on_pbDaysUp_clicked() {
    int index = -1;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == _currentDayType) {
            index = i - 1;
        }
    }

    if(index < 0 || index >= tableReference.count())
        return;

    tableReference.remove(index + 1);
    tableReference.insert(index, _currentDayType);

    refreshDayTypesTable();
}


void DlgProjectSettings::on_pbDaysDown_clicked() {
    int index = -1;

    for(int i = 0; i < tableReference.count(); i++) {
        if(tableReference[i] == _currentDayType) {
            index = i + 1;
        }
    }

    if(index < 0 || index >= tableReference.count())
        return;

    tableReference.remove(index - 1);
    tableReference.insert(index, _currentDayType);

    refreshDayTypesTable();
}

void DlgProjectSettings::on_daySelector_weekDaysChanged() {
    if(!_currentDayType)
        return;

    *_currentDayType = ui->daySelector->weekDays();

    refreshDayTypesTable();
}
