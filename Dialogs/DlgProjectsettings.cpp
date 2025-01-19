#include "dlgprojectsettings.h"
#include "ui_dlgprojectsettings.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

DlgProjectSettings::DlgProjectSettings(QWidget *parent, ProjectSettings *projectSettings) :
    QDialog(parent),
    ui(new Ui::DlgProjectSettings),
    _projectSettingsPtr(projectSettings),
    _projectSettings(*projectSettings),
    _model(new DayTypeListModel(this)),
    _icon("") {
    ui->setupUi(this);

    _projectSettings.setParent(nullptr);

    ui->lwDays->setModel(_model);

    connect(_model,                       &QAbstractItemModel::rowsInserted,       this, &DlgProjectSettings::onDayTypeInserted);
    connect(ui->pbSelectIcon,             &QPushButton::clicked,                   this, &DlgProjectSettings::actionSelectIcon);
    connect(ui->lwDays->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &DlgProjectSettings::onSelectionChanged);


    loadProjectSettings();

    ui->tabWidget->setCurrentIndex(0);

    ui->leDayTypesName->setEnabled(false);
    ui->daySelector->setEnabled(false);
    ui->daySelector->setWeekDays(WeekDays());
}

DlgProjectSettings::~DlgProjectSettings()
{
    delete ui;
}

ProjectSettings DlgProjectSettings::projectSettings() const {
    ProjectSettings p = _projectSettings;
    p.setDisplayName(ui->leDisplayName->text());
    p.setShortName(ui->leShortName->text());
    p.setIcon(ui->leIconPath->text());
    return p;
}

void DlgProjectSettings::loadProjectSettings() {
    ui->leDisplayName->setText(_projectSettings.displayName());
    ui->leShortName->setText(_projectSettings.shortName());
    ui->leIconPath->setText(_projectSettings.icon());

    _projectSettings.setDayTypes(_projectSettingsPtr->cloneDayTypes(), true);
    _model->setProjectSettings(&_projectSettings);

    reloadIconPreview();
}

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

void DlgProjectSettings::refreshTayTypesDetails() {
    QModelIndex index = ui->lwDays->selectionModel()->currentIndex();
    if(!index.isValid()){
        ui->leDayTypesName->setText("");
        ui->daySelector->setWeekDays(WeekDays());

        ui->leDayTypesName->setEnabled(false);
        ui->daySelector->setEnabled(false);

        return;
    }

    DayType *dt = _model->itemAt(index.row());

    ui->leDayTypesName->setText(dt->name());
    ui->daySelector->setWeekDays(WeekDays(*dt));

    ui->leDayTypesName->setEnabled(true);
    ui->daySelector->setEnabled(true);
}

void DlgProjectSettings::onSelectionChanged(QModelIndex current, QModelIndex previous) {
    Q_UNUSED(current);Q_UNUSED(previous);

    refreshTayTypesDetails();
}

void DlgProjectSettings::onDayTypeInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);Q_UNUSED(last);
    ui->lwDays->setCurrentIndex(_model->index(first, 0));
    ui->lwDays->selectionModel()->select(_model->index(first, 0), QItemSelectionModel::ClearAndSelect);
    ui->lwDays->setFocus();
}

void DlgProjectSettings::on_leDayTypesName_textEdited(const QString &arg1) {
    QModelIndex index = ui->lwDays->selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    DayType *dt = _model->itemAt(index.row());
    if(dt)
        dt->setName(arg1);
}

void DlgProjectSettings::on_pbDayNew_clicked() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Create New Day Type"), tr("Enter day type name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    DayType *dt = _projectSettingsPtr->newDayType();
    dt->setName(name);
    dt->setCode(0);
    _projectSettings.addDayType(dt, true);
    refreshTayTypesDetails();
}


void DlgProjectSettings::on_pbDaysDelete_clicked() {
    QModelIndex index = ui->lwDays->selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    DayType *dt = _model->itemAt(index.row());

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete day type"), tr("<p><b>Do you really want to delete this dayType?</b></p><p>%1</p>").arg(dt->name()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _projectSettings.removeDayType(dt);
}


void DlgProjectSettings::on_pbDaysUp_clicked() {
    QModelIndex index = ui->lwDays->selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    DayType *dt = _model->itemAt(index.row());
    if(index.row() < 1)
        return;

    int targetIndex = index.row() - 1;

    _projectSettings.removeDayType(dt);
    qApp->processEvents();
    _projectSettings.insertDayType(dt, targetIndex, true);
}


void DlgProjectSettings::on_pbDaysDown_clicked() {
    QModelIndex index = ui->lwDays->selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    DayType *dt = _model->itemAt(index.row());
    if(index.row() >= _model->itemCount() - 1)
        return;

    int targetIndex = index.row() + 1;

    _projectSettings.removeDayType(dt);
    qApp->processEvents();
    _projectSettings.insertDayType(dt, targetIndex, true);
}

void DlgProjectSettings::on_daySelector_weekDaysChanged() {
    QModelIndex index = ui->lwDays->currentIndex();
    if(!index.isValid())
        return;

    DayType *dt = _model->itemAt(index.row());
    *dt = ui->daySelector->weekDays();
}
