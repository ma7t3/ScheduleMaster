#include "Dialogs/DlgRouteeditor.h"
#include "ui_DlgRouteeditor.h"

#include "Dialogs/DlgTimeprofileeditor.h"

#include "App/global.h"

#include <QMessageBox>

routeEditor::routeEditor(QWidget *parent, bool createMode, Route * r, QList<LineDirection *> ld, QList<Busstop*> l, QList<Route *> m) :
    QDialog(parent),
    matchingRoutes(m),
    ui(new Ui::routeEditor),
    directions(ld),
    routeData(r),
    allBusstops(l)
{
    ui->setupUi(this);

    ui->twAllBusstops->hideColumn(0);
    ui->twRouteBusstops->hideColumn(0);
    ui->twProfiles->hideColumn(0);

    ui->twAllBusstops->header()->hide();
    ui->twRouteBusstops->header()->hide();

    if(createMode)
        QDialog::setWindowTitle(tr("Create route"));

    for(int i = 0; i < ld.count(); i++) {
        ui->cbDirection->addItem(ld[i]->description());
        if(ld[i] == r->direction())
            ui->cbDirection->setCurrentIndex(i);
    }

    refreshAllBusstops("");

    ui->twAllBusstops->sortItems(1, Qt::AscendingOrder);

    QString name = r->name();
    int code = r->code();

    ui->leName->setText(name);
    ui->sbCode->setValue(code);
    
    for(int i = 0; i < r->busstopCount(); i++) {
        Busstop *b = r->busstopAt(i);

        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twRouteBusstops);

        itm->setText(0, b->id());
        itm->setText(1, b->name());

        ui->twRouteBusstops->addTopLevelItem(itm);
    }

    // load profiles

    refreshProfiles();


}

routeEditor::~routeEditor()
{
    delete ui;
}

void routeEditor::refreshAllBusstops(QString filter)
{
    ui->twAllBusstops->clear();

    for(int i = 0; i < allBusstops.count(); i++) {
        QTreeWidgetItem *itm = new QTreeWidgetItem;

        if(!allBusstops[i]->name().contains(filter, Qt::CaseInsensitive))
            continue;

        itm->setText(0, allBusstops[i]->id());
        itm->setText(1, allBusstops[i]->name());

        ui->twAllBusstops->addTopLevelItem(itm);
    }

    ui->twAllBusstops->sortItems(1, Qt::AscendingOrder);
}

void routeEditor::refreshProfiles()
{
    ui->twProfiles->clear();
    for(int i = 0; i < routeData->profileCount(); i++) {
        TimeProfile *p = routeData->timeProfileAt(i);

        QString id = p->id();
        QString name = p->name();
        float duration = p->duration();

        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twProfiles);

        itm->setText(0, id);
        itm->setText(1, name);
        itm->setText(2, QString::number(duration) + " min.");

        ui->twProfiles->addTopLevelItem(itm);
    }
}

QString routeEditor::name()
{
    return ui->leName->text();
}

int routeEditor::getCode()
{
    return ui->sbCode->value();
}

LineDirection *routeEditor::getDirection()
{
    return directions[ui->cbDirection->currentIndex()];
}

QStringList routeEditor::getBusstopList()
{
    QStringList busstopIDs;
    for(int i = 0; i < ui->twRouteBusstops->topLevelItemCount(); i++) {
        busstopIDs << ui->twRouteBusstops->topLevelItem(i)->text(0);
    }
    return busstopIDs;
}

QStringList routeEditor::getTimeProfileList()
{
    return {};
}

void routeEditor::on_leBusstopsSearch_textChanged(const QString &arg1)
{
    refreshAllBusstops(ui->leBusstopsSearch->text());
}

void routeEditor::on_twAllBusstops_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    on_pbBusstopAdd_clicked();
}

void routeEditor::on_pbBusstopAdd_clicked()
{
    if(!ui->twAllBusstops->currentItem())
        return;

    QString id = ui->twAllBusstops->currentItem()->text(0);
    QString name = ui->twAllBusstops->currentItem()->text(1);

    QTreeWidgetItem * itm = new QTreeWidgetItem;

    itm->setText(0, id);
    itm->setText(1, name);

    QTreeWidgetItem * currentTargetItem = ui->twRouteBusstops->currentItem();

    int i;
    if(!currentTargetItem)
        i = 0;
    else
        i = ui->twRouteBusstops->indexOfTopLevelItem(currentTargetItem) + 1;

    ui->twRouteBusstops->insertTopLevelItem(i, itm);
    ui->twRouteBusstops->setCurrentItem(itm);
}



void routeEditor::on_pbBusstopRemove_clicked()
{
    if(!ui->twRouteBusstops->currentItem())
        return;

    delete ui->twRouteBusstops->currentItem();
}


void routeEditor::on_pbBusstopUp_clicked()
{
    moveCurrentRouteBusstop(false);
}


void routeEditor::on_pbBusstopDown_clicked()
{
    moveCurrentRouteBusstop(true);
}

void routeEditor::moveCurrentRouteBusstop(bool direction)
{
    if(!ui->twRouteBusstops->currentItem())
        return;

    int i = ui->twRouteBusstops->currentIndex().row();
    int z = i + (direction ? 1 : -1);

    if(z < 0 || z >= ui->twRouteBusstops->topLevelItemCount())
        return;

    QTreeWidgetItem *itm = ui->twRouteBusstops->takeTopLevelItem(i);
    ui->twRouteBusstops->insertTopLevelItem(z, itm);
    ui->twRouteBusstops->setCurrentItem(itm);
}


void routeEditor::on_pbBusstopReverse_clicked()
{
    //QMessageBox::warning(this, "Not available", "This feature is currently not available");

    QList<QPair<QString, QString>> l;

    for(int i = 0; i < ui->twRouteBusstops->topLevelItemCount(); i++) {
        QPair<QString, QString> p;
        p.first = ui->twRouteBusstops->topLevelItem(i)->text(0);
        p.second = ui->twRouteBusstops->topLevelItem(i)->text(1);
        l << p;
    }

    ui->twRouteBusstops->clear();

    for(int i = l.count() - 1; i >= 0; i--) {
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twRouteBusstops);
        itm->setText(0, l[i].first);
        itm->setText(1, l[i].second);
        ui->twRouteBusstops->addTopLevelItem(itm);
    }

}



void routeEditor::on_pbProfileNew_clicked()
{
    TimeProfileEditor dlg(this, true, "", 0, routeData, {});
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    QString name = dlg.name();
    float duration = dlg.getDuration();
    QList<TimeProfileItem *> itemList = dlg.getTimeProfileItemList();

    TimeProfile *t = new TimeProfile(global::getNewID(), name);
    t->setDuration(duration);
    t->addBusstopList(itemList);

    routeData->addTimeProfile(t);

    refreshProfiles();
}



void routeEditor::on_twProfiles_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    on_pbProfileEdit_clicked();
}


void routeEditor::on_pbProfileEdit_clicked()
{
    if(!ui->twProfiles->currentItem())
        return;

    QString id = ui->twProfiles->currentItem()->text(0);
    TimeProfile *p = routeData->timeProfile(id);
    if(!p)
        return;

    QString name = p->name();
    float duration = p->duration();
    QList<TimeProfileItem *> itemList = p->busstops();


    TimeProfileEditor dlg(this, false, name, duration, routeData, itemList, matchingRoutes);

    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    QString newName = dlg.name();
    float newDuration = dlg.getDuration();
    QList<TimeProfileItem *> newItemList = dlg.getTimeProfileItemList();

    p->setName(newName);
    p->setDuration(newDuration);
    p->setBusstopList(newItemList);

    refreshProfiles();
}




void routeEditor::on_pbProfileDelete_clicked()
{
    if(!ui->twProfiles->currentItem())
        return;

    QString id = ui->twProfiles->currentItem()->text(0);
    TimeProfile *p = routeData->timeProfile(id);
    if(!p)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete profile"), tr("<p>Do you really want to delete this profile?</p><table><tr><td><b>Name:</b></td><td>%1</td></tr><tr><td><b>Duration:</b></td><td>%2 min.</td></tr></table><p><b>You cannot undo this action!</b></p>").arg(p->name(), QString::number(p->duration())), QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    routeData->removeTimeProfile(p);

    refreshProfiles();
}


















