
#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>

#include "ProjectData/ProjectData.h"

#include "Dialogs\DlgTimeprofileeditor.h"
#include "ui_DlgTimeprofileeditor.h"

DlgTimeProfileEditor::DlgTimeProfileEditor(QWidget *parent, TimeProfile *timeProfile, Route *route, const bool &createMode) :
    QDialog(parent),
    ui(new Ui::DlgTimeProfileEditor),
    _timeProfile(*timeProfile),
    _timeProfilePtr(timeProfile)
{
    ui->setupUi(this);

    setCreateMode(createMode);

    ui->twProfile->setColumnHidden(0, true);
    ui->twProfile->setColumnWidth(1, 350);
    ui->twProfile->setColumnWidth(2, 50);
    ui->twProfile->setColumnWidth(3, 75);
    ui->twProfile->setColumnWidth(4, 75);
    ui->twProfile->setColumnWidth(5, 175);


    loadTimeProfile();
    loadBusstopList(route);

    ui->leName->setFocus();
}

DlgTimeProfileEditor::~DlgTimeProfileEditor()
{
    delete ui;
}

void DlgTimeProfileEditor::setCreateMode(const bool &newCreateMode) {
    if(newCreateMode)
        setWindowTitle(tr("Create Time Profile"));
}

TimeProfile DlgTimeProfileEditor::timeProfile() const {
    TimeProfile timeProfile = _timeProfile;

    Route *r = dynamic_cast<Route *>(timeProfile.parent());

    timeProfile.setName(ui->leName->text());
    timeProfile.setDuration(ui->dsbDuration->value());


    QList<TimeProfileItem *> itemList;

    for(int i = 0; i < r->busstopCount(); i++) {
        QWidget *sepTimesW = ui->twProfile->cellWidget(i, 2);
        QWidget *arrTimeW = ui->twProfile->cellWidget(i, 3);
        QWidget *depTimeW = ui->twProfile->cellWidget(i, 4);
        QWidget *busstopModeW = ui->twProfile->cellWidget(i, 5);

        bool sepTimes = qobject_cast<QCheckBox *>(sepTimesW)->isChecked();
        float arrTime = qobject_cast<QDoubleSpinBox *>(arrTimeW)->value();
        float depTime = qobject_cast<QDoubleSpinBox *>(depTimeW)->value();
        int busstopMode = qobject_cast<QComboBox *>(busstopModeW)->currentIndex();

        TimeProfileItem *itm = timeProfile.busstop(r->busstopAt(i));
        if(!itm)
            itm = new TimeProfileItem(_timeProfilePtr, r->busstopAt(i));
        itm->setDepValue(depTime);
        if(sepTimes)
            itm->setArrValue(arrTime);
        itm->setBusstopMode(busstopMode);

        itemList << itm;
    }

    timeProfile.setBusstops(itemList);
    return timeProfile;
}

void DlgTimeProfileEditor::loadTimeProfile() {
    ui->leName->setText(_timeProfile.name());
    ui->dsbDuration->setValue(_timeProfile.duration());
}

void DlgTimeProfileEditor::loadBusstopList(Route *r) {
    int busstopCount = r->busstopCount();
    ui->twProfile->setRowCount(busstopCount);

    for(int i = 0; i < r->busstopCount(); i++) {
        Busstop *b = r->busstopAt(i);
        QTableWidgetItem *itmName = new QTableWidgetItem(b->name());

        ui->twProfile->setItem(i, 1, itmName);

        QCheckBox *sepBox = new QCheckBox;
        sepBox->setChecked(false);

        ui->twProfile->setCellWidget(i, 2, sepBox);

        QDoubleSpinBox *dsbArr = new QDoubleSpinBox;
        QDoubleSpinBox *dsbDep = new QDoubleSpinBox;

        dsbArr->setSuffix(tr(" min."));
        dsbDep->setSuffix(tr(" min."));

        dsbArr->setEnabled(false);

        ui->twProfile->setCellWidget(i, 3, dsbArr);
        ui->twProfile->setCellWidget(i, 4, dsbDep);

        QComboBox *cb = new QComboBox;
        QStringList items = {tr("never"), tr("normal"), tr("always"), tr("wait for dep. time"), tr("always + wait for dep. time")};
        cb->addItems(items);
        cb->setCurrentIndex(1);

        ui->twProfile->setCellWidget(i, 5, cb);

        //load data
        TimeProfileItem *itm = _timeProfile.busstop(b);

        if(itm) {
            sepBox->setChecked(itm->hasSeperateTimes());
            if(itm->hasSeperateTimes()) {
                dsbArr->setValue(itm->arrValue());
                dsbArr->setEnabled(true);
            } else {
                dsbArr->setValue(0);
                dsbArr->setEnabled(false);
            }

            dsbDep->setValue(itm->depValue());
            cb->setCurrentIndex(itm->busstopMode());
        }

        connect(sepBox, &QCheckBox::stateChanged, this, &DlgTimeProfileEditor::refreshTable);
    }

    // load busstops
    QList<Route *> allMatchingRoutes = dynamic_cast<ProjectData *>(r->parent()->parent())->matchingRoutes(r);
    for(int i = 0; i < allMatchingRoutes.count(); i++)
        if(allMatchingRoutes[i]->timeProfileCount() > 0)
            _matchingRoutes << allMatchingRoutes[i];

    // copy-buttons
    for(int i = 0; i < _matchingRoutes.count(); i++) {
        ui->twProfile->insertColumn(ui->twProfile->columnCount());
        int targetColumn = ui->twProfile->columnCount() - 1;
        Route *compareRoute = _matchingRoutes[i];
        ui->twProfile->setHorizontalHeaderItem(targetColumn, new QTableWidgetItem(compareRoute->name()));

        for(int j = 0; j < ui->twProfile->rowCount(); j++) {
            if(compareRoute->hasBusstop(r->busstopAt(j))) {
                QPushButton* pb = new QPushButton;
                pb->setText(compareRoute->name());
                ui->twProfile->setCellWidget(j, targetColumn, pb);

                connect(pb, &QPushButton::clicked, this, &DlgTimeProfileEditor::executeCopy);
            }
        }
    }

    // row hight
    for(int i = 0; i < ui->twProfile->rowCount(); i++) {
        ui->twProfile->setRowHeight(i, 25);
    }
}

void DlgTimeProfileEditor::refreshTable(int state)
{
    int row = ui->twProfile->currentRow();
    QWidget *w = ui->twProfile->cellWidget(row, 3);
    QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox *>(w);

    if(state == Qt::Checked)
        dsb->setEnabled(true);
    else {
        dsb->setEnabled(false);
        dsb->setValue(0);
    }
}

void DlgTimeProfileEditor::executeCopy()
{
    int routeIndex = ui->twProfile->currentColumn() - 6;
    Route *r = _matchingRoutes[routeIndex];
    QStringList itemList;

    int presetProfileIndex = 0;
    
    for(int i = 0; i < r->timeProfileCount(); i++) {
        itemList << r->timeProfileAt(i)->name();
        if(r->timeProfileAt(i)->name() == ui->leName->text())
            presetProfileIndex = i;
    }

    if(itemList.count() == 0) {
        QMessageBox::warning(this, tr("No profiles"), tr("This trip has no profiles.<br>You can't use it now!"));
        return;
    }

    bool ok;
    QString result = QInputDialog::getItem(this, tr("select source profile"), tr("Please select the source profile:"), itemList, presetProfileIndex, false, &ok);
    if(!ok)
        return;

    TimeProfile *p = nullptr;
    
    for(int i = 0; i < r->timeProfileCount(); i++)
        if(r->timeProfileAt(i)->name() == result) {
            p = r->timeProfileAt(i);
            break;
        }

    if(!p)
        return;

    int currentRow = ui->twProfile->currentRow();
    Busstop *currentBusstop = dynamic_cast<Route *>(_timeProfile.parent())->busstopAt(currentRow);
    QWidget *depTimeW = ui->twProfile->cellWidget(currentRow, 4);
    float targetOffset = qobject_cast<QDoubleSpinBox *>(depTimeW)->value();
    float sourceOffset = p->busstop(currentBusstop)->depValue();

    int sourceIndex = -1;
    int targetIndex = currentRow;
    for(int i =0; i < r->busstopCount(); i++)
        if(r->busstopAt(i) == currentBusstop)
            sourceIndex = i;

    if(sourceIndex == -1)
        return;

    while(true) {
        sourceIndex++;
        targetIndex++;

        if(targetIndex >= ui->twProfile->rowCount())
            break;
        
        if(sourceIndex >= r->busstopCount())
            break;

        Busstop *currentBusstop = dynamic_cast<Route *>(_timeProfile.parent())->busstopAt(targetIndex);
        if(r->busstopAt(sourceIndex) != currentBusstop)
            break;

        TimeProfileItem *sourceItm = p->busstop(currentBusstop);
        float arr = sourceItm->arrValue() - sourceOffset + targetOffset;
        float dep = sourceItm->depValue() - sourceOffset + targetOffset;
        int busstopMode = sourceItm->busstopMode();
        bool sepTimes = sourceItm->hasSeperateTimes();

        QCheckBox *sepTimesBox    = qobject_cast<QCheckBox *>(ui->twProfile->cellWidget(targetIndex, 2));
        QDoubleSpinBox *arrBox    = qobject_cast<QDoubleSpinBox *>(ui->twProfile->cellWidget(targetIndex, 3));
        QDoubleSpinBox *depBox    = qobject_cast<QDoubleSpinBox *>(ui->twProfile->cellWidget(targetIndex, 4));
        QComboBox *busstopModeBox = qobject_cast<QComboBox *>(ui->twProfile->cellWidget(targetIndex, 5));

        sepTimesBox->setChecked(sepTimes);
        arrBox->setValue(arr);
        depBox->setValue(dep);
        busstopModeBox->setCurrentIndex(busstopMode);

        if(sepTimes)
            arrBox->setEnabled(true);
        else {
            arrBox->setEnabled(false);
            arrBox->setValue(0);
        }

        if(targetIndex + 1 == ui->twProfile->rowCount())
            ui->dsbDuration->setValue(arr);
    }
}
