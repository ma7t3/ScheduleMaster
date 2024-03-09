
#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include "Dialogs\DlgTimeprofileeditor.h"
#include "ui_DlgTimeprofileeditor.h"

TimeProfileEditor::TimeProfileEditor(QWidget *parent, bool createMode, QString n, float d, Route* r,  QList<TimeProfileItem *> l, QList<Route *> m) :
    QDialog(parent),
    ui(new Ui::TimeProfileEditor),
    route(r),
    itemList(l),
    matchingRoutes(m)
{
    ui->setupUi(this);

    ui->twProfile->verticalHeader()->setVisible(false);
    ui->twProfile->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twProfile->setColumnHidden(0, true);
    ui->twProfile->setColumnWidth(1, 350);
    ui->twProfile->setColumnWidth(2, 50);
    ui->twProfile->setColumnWidth(3, 75);
    ui->twProfile->setColumnWidth(4, 75);
    ui->twProfile->setColumnWidth(5, 175);

    if(createMode)
        this->setWindowTitle(tr("Create Time Profile"));

    ui->leName->setText(n);
    ui->dsbDuration->setValue(d);

    // load busstops

    QStringList busstopIDList;
    
    int busstopCount = r->busstopCount();
    ui->twProfile->setRowCount(busstopCount);
    
    for(int i = 0; i < r->busstopCount(); i++) {
        Busstop *b = r->busstopAt(i);

        QString id = b->id();
        QString name = b->name();

        busstopIDList << id;

        QTableWidgetItem *itmID = new QTableWidgetItem;
        QTableWidgetItem *itmName = new QTableWidgetItem;

        itmID->setText(id);
        itmName->setText(name);

        ui->twProfile->setItem(i, 0, itmID);
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

        TimeProfileItem *itm = findItemForBusstop(id);

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

        QObject::connect(sepBox, SIGNAL(stateChanged(int)), this, SLOT(refreshTable(int)));
    }

    QList<Route *> matchingRoutesFiltered;
    for(int i = 0; i < matchingRoutes.count(); i++)
        if(matchingRoutes[i]->timeProfileCount() > 0)
            matchingRoutesFiltered << matchingRoutes[i];

    matchingRoutes = matchingRoutesFiltered;

    // copy-buttons
    for(int i = 0; i < matchingRoutesFiltered.count(); i++) {
        ui->twProfile->insertColumn(ui->twProfile->columnCount());
        int targetColumn = ui->twProfile->columnCount() - 1;
        Route *r = matchingRoutesFiltered[i];
        ui->twProfile->setHorizontalHeaderItem(targetColumn, new QTableWidgetItem(r->name()));

        for(int j = 0; j < ui->twProfile->rowCount(); j++) {
            QString currentBusstopId = ui->twProfile->item(j, 0)->text();

            if(r->hasBusstop(currentBusstopId)) {
                QPushButton* pb = new QPushButton;
                pb->setText(r->name());
                ui->twProfile->setCellWidget(j, targetColumn, pb);

                QObject::connect(pb, SIGNAL(clicked()), this, SLOT(executeCopy()));
            }
        }
    }

    // row hight
    for(int i = 0; i < ui->twProfile->rowCount(); i++) {
        ui->twProfile->setRowHeight(i, 25);
    }

    ui->leName->setFocus();
}

TimeProfileEditor::~TimeProfileEditor()
{
    delete ui;
}

void TimeProfileEditor::setName(QString n)
{
    ui->leName->setText(n);
}

void TimeProfileEditor::setDuration(float d)
{
    ui->dsbDuration->setValue(d);
}

void TimeProfileEditor::setRoute(Route *r)
{
    route = r;
}

void TimeProfileEditor::setTimeProfileItemList(QList<TimeProfileItem *> l)
{
    itemList = l;
}


QString TimeProfileEditor::name()
{
    return ui->leName->text();
}

float TimeProfileEditor::getDuration()
{
    return ui->dsbDuration->value();
}

Route * TimeProfileEditor::getRoute()
{
    return route;
}

QList<TimeProfileItem *> TimeProfileEditor::getTimeProfileItemList()
{
    QList<TimeProfileItem *> l;

    for(int i = 0; i < ui->twProfile->rowCount(); i++) {
        QString busstopID = ui->twProfile->item(i, 0)->text();
        QWidget *sepTimesW = ui->twProfile->cellWidget(i, 2);
        QWidget *arrTimeW = ui->twProfile->cellWidget(i, 3);
        QWidget *depTimeW = ui->twProfile->cellWidget(i, 4);
        QWidget *busstopModeW = ui->twProfile->cellWidget(i, 5);

        bool sepTimes = qobject_cast<QCheckBox *>(sepTimesW)->isChecked();
        float arrTime = qobject_cast<QDoubleSpinBox *>(arrTimeW)->value();
        float depTime = qobject_cast<QDoubleSpinBox *>(depTimeW)->value();
        int busstopMode = qobject_cast<QComboBox *>(busstopModeW)->currentIndex();

        TimeProfileItem *itm = new TimeProfileItem(nullptr, busstopID);
        itm->setDepValue(depTime);

        if(sepTimes)
            itm->setArrValue(arrTime);

        itm->setBusstopMode(busstopMode);

        l << itm;
    }

    return l;
}


TimeProfileItem * TimeProfileEditor::findItemForBusstop(QString id)
{
    for(int i = 0; i < itemList.count(); i++) {
        TimeProfileItem *itm = itemList[i];
        
        if(itm->busstopId() == id)
            return itm;
    }

    return nullptr;
}

void TimeProfileEditor::refreshTable(int state)
{
    int row = ui->twProfile->currentRow();
    QWidget *w = ui->twProfile->cellWidget(row, 3);
    QDoubleSpinBox *b = qobject_cast<QDoubleSpinBox *>(w);

    if(state == Qt::Checked)
        b->setEnabled(true);
    else {
        b->setEnabled(false);
        b->setValue(0);
    }
}

void TimeProfileEditor::executeCopy()
{
    int routeIndex = ui->twProfile->currentColumn() - 6;
    Route *r = matchingRoutes[routeIndex];
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

    /*QInputDialog dlg;
    dlg.setWindowTitle("select source profile");
    dlg.setLabelText("Please select the source profile: ");
    dlg.setComboBoxItems(itemList);
    dlg.setComboBoxEditable(false);
    dlg.setOkButtonText("Copy!");
    dlg.setOption(QInputDialog::UseListViewForComboBoxItems);
    dlg.exec();
    QString result = dlg.textValue();*/
    TimeProfile *p = nullptr;
    
    for(int i = 0; i < r->timeProfileCount(); i++)
        if(r->timeProfileAt(i)->name() == result) {
            p = r->timeProfileAt(i);
            break;
        }

    if(!p)
        return;

    int currentRow = ui->twProfile->currentRow();
    QString currentBusstopID = ui->twProfile->item(currentRow, 0)->text();
    QWidget *depTimeW = ui->twProfile->cellWidget(currentRow, 4);
    float targetOffset = qobject_cast<QDoubleSpinBox *>(depTimeW)->value();
    float sourceOffset = p->busstop(currentBusstopID)->depValue();

    int sourceIndex = -1;
    int targetIndex = currentRow;
    for(int i =0; i < r->busstopCount(); i++)
        if(r->busstopAt(i)->id() == currentBusstopID)
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

        QString busstopID = ui->twProfile->item(targetIndex, 0)->text();
        if(r->busstopAt(sourceIndex)->id() != busstopID)
            break;

        TimeProfileItem *sourceItm = p->busstop(busstopID);
        float arr = sourceItm->arrValue() - sourceOffset + targetOffset;
        float dep = sourceItm->depValue() - sourceOffset + targetOffset;
        int busstopMode = sourceItm->busstopMode();
        bool sepTimes = sourceItm->hasSeperateTimes();

        QCheckBox *sepTimesBox = qobject_cast<QCheckBox *>(ui->twProfile->cellWidget(targetIndex, 2));
        QDoubleSpinBox *arrBox = qobject_cast<QDoubleSpinBox *>(ui->twProfile->cellWidget(targetIndex, 3));
        QDoubleSpinBox *depBox = qobject_cast<QDoubleSpinBox *>(ui->twProfile->cellWidget(targetIndex, 4));
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



















