#include "wdgroutes.h"
#include "ui_wdgroutes.h"

#include <QUndoStack>
#include <QMessageBox>

#include <QFileDialog>

#include "App/global.h"
#include "Dialogs/DlgRouteeditor.h"
#include "Dialogs/DlgDataexporter.h"
#include "Commands/CmdRoutes.h"

WdgRoutes::WdgRoutes(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgRoutes),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QAction *actionEdit = new QAction(ui->twRoutes);
    actionEdit->setShortcuts({QKeySequence(Qt::Key_Space), QKeySequence(Qt::Key_Return), QKeySequence(Qt::Key_Enter)});
    actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->twRoutes->addAction(actionEdit);

    QAction *actionDuplicate = new QAction(ui->twRoutes);
    actionDuplicate->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_D));
    actionDuplicate->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->twRoutes->addAction(actionDuplicate);

    QAction *actionDelete = new QAction(ui->twRoutes);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->twRoutes->addAction(actionDelete);

    QObject::connect(actionEdit, SIGNAL(triggered()), this, SLOT(actionEdit()));
    QObject::connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(actionDuplicate()));
    QObject::connect(actionDelete, SIGNAL(triggered()), this, SLOT(actionDelete()));

    QObject::connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbEdit, SIGNAL(clicked()), this, SLOT(actionEdit()));
    QObject::connect(ui->pbDuplicate, SIGNAL(clicked()), this, SLOT(actionDuplicate()));
    QObject::connect(ui->twRoutes, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(actionEdit()));
    QObject::connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));
    QObject::connect(ui->pbExportProfiles, SIGNAL(clicked()), this, SLOT(actionExportProfiles()));
    QObject::connect(ui->leSearch, SIGNAL(textChanged(QString)), this, SLOT(refresh()));

    QObject::connect(ui->pbExportProfilesOMSITrips, SIGNAL(clicked()), this, SLOT(omsiExport()));

    _actionNew->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_R));
    _actionEdit->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_R));

    ui->twRoutes->verticalHeader()->setVisible(false);
    ui->twRoutes->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twRoutes->setColumnWidth(0, 50);
    ui->twRoutes->setColumnWidth(1, 50);
    ui->twRoutes->setColumnWidth(2, 400);
}

WdgRoutes::~WdgRoutes()
{
    delete ui;
}

void WdgRoutes::actionNew() {
    if(!_currentLine)
        return;

    Route *r = _currentLine->newRoute();
    r->setDirection(_currentLine->directionAt(0));
    routeEditor dlg(this, true, r, _currentLine->directions(), projectData->busstops());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    r->setName(dlg.name());
    r->setCode(dlg.getCode());
    r->setDirection(dlg.getDirection());

    QStringList busstopList = dlg.getBusstopList();

    for(int i = 0; i < busstopList.count(); i++) {
        Busstop * b = projectData->busstop(busstopList[i]);
        r->addBusstop(b);
    }

    undoStack->push(new CmdRouteNew(_currentLine, r));
    emit refreshRequested();
}

void WdgRoutes::actionEdit() {
    if(!_currentLine || !_currentRoute)
        return;
    
    QList<Route *> matchingRoutes = projectData->matchingRoutes(_currentRoute);
    
    routeEditor dlg(this, false, _currentRoute, _currentLine->directions(), projectData->busstops(), matchingRoutes);

    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Route newR = *_currentRoute;

    newR.setName(dlg.name());
    newR.setCode(dlg.getCode());
    newR.setDirection(dlg.getDirection());
    newR.clearBusstops();

    QStringList busstopList = dlg.getBusstopList();

    for(int i = 0; i < busstopList.count(); i++) {
        Busstop *b = projectData->busstop(busstopList[i]);
        newR.addBusstop(b);
    }

    undoStack->push(new CmdRouteEdit(_currentRoute, newR));
    emit refreshRequested();
}

void WdgRoutes::actionDuplicate() {
    if(!_currentLine || !_currentRoute)
        return;
    
    QList<Route *> matchingRoutes = projectData->matchingRoutes(_currentRoute);
    
    routeEditor dlg(this, false, _currentRoute, _currentLine->directions(), projectData->busstops(), matchingRoutes);
    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Route *n = _currentLine->newRoute();
    n->setCode(dlg.getCode());
    n->setName(dlg.name());
    n->setDirection(dlg.getDirection());
    QStringList busstopList = dlg.getBusstopList();

    for(int i = 0; i < busstopList.count(); i++) {
        Busstop * b = projectData->busstop(busstopList[i]);
        n->addBusstop(b);
    }

    undoStack->push(new CmdRouteNew(_currentLine, n));
    emit refreshRequested();
}

void WdgRoutes::actionDelete() {
    QModelIndexList selection = ui->twRoutes->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Route *> routes;
    for(int i = 0; i < selection.count(); i++) {
        Route *r = tableReference[selection[i].row()];
        routes << r;
        showList += QString("<li>%1</li>").arg(r->name());
    }
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("delete route(s)"), tr("<p><b>Do you really want to delete these %n route(s)?</b></p>", "", routes.count()) + showList, QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new CmdRoutesDelete(_currentLine, routes));
    emit refreshRequested();
}

void WdgRoutes::actionExportProfiles() {
    if(!_currentRoute)
        return;

    Route *r = _currentRoute;

    QString result = "";

    QList<TimeProfile *> profiles = r->timeProfiles();
    for(int i = 0; i < profiles.count(); i++) {
        TimeProfile *p = profiles[i];
        result += "[profile]\r\n" + p->name() + "\r\n" + QString::number(p->duration()) + "\r\n\r\n";

        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);
            TimeProfileItem *itm = p->busstop(b);
            if(!itm)
                continue;

            result += "[profile_man_arr_time]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->depValue()) + "\r\n";
            if(itm->busstopMode() != TimeProfileItem::BusstopModeNormal) {
                result += "[profile_otherstopping]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->busstopMode()) + "\r\n";
            }
        }
    }
    DataExporter dlg;
    dlg.setFileName(r->name());
    dlg.setFolderName("profiles");
    dlg.setText(result, "", "");
    dlg.exec();
}

void WdgRoutes::setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDuplicate, QAction *actionDelete) {
    _actionNew = actionNew;
    _actionEdit = actionEdit;
    _actionDuplicate = actionDuplicate;
    _actionDelete = actionDelete;

    refreshUI();
}

void WdgRoutes::refreshUI() {
    int selectionCount = ui->twRoutes->selectionModel()->selectedRows(0).count();

    if(selectionCount == 0) {
        ui->twRoutes->setCurrentItem(nullptr);
        ui->pbEdit->setEnabled(false);
        ui->pbDuplicate->setEnabled(false);
        ui->pbDelete->setEnabled(false);
        _actionEdit->setEnabled(false);
        _actionDuplicate->setEnabled(false);
        _actionDelete->setEnabled(false);
    } else if(selectionCount == 1) {
        ui->pbEdit->setEnabled(true);
        ui->pbDuplicate->setEnabled(true);
        ui->pbDelete->setEnabled(true);
        _actionEdit->setEnabled(true);
        _actionDuplicate->setEnabled(true);
        _actionDelete->setEnabled(true);
    } else {
        ui->pbEdit->setEnabled(false);
        ui->pbDuplicate->setEnabled(false);
        ui->pbDelete->setEnabled(true);
        _actionEdit->setEnabled(false);
        _actionDuplicate->setEnabled(false);
        _actionDelete->setEnabled(true);
    }
}

void WdgRoutes::setCurrentLine(Line *l) {
    if(!l) {
        ui->pbNew->setEnabled(false);
        _actionNew->setEnabled(false);
        _currentLine = nullptr;
    }

    ui->pbNew->setEnabled(true);
    _actionNew->setEnabled(true);

    _currentLine = l;
    refresh();
}

Route * WdgRoutes::currentRoute() {
    return _currentRoute;
}

QAction *WdgRoutes::menubarActionNew() {
    return _actionNew;
}

QAction *WdgRoutes::menubarActionEdit() {
    return _actionEdit;
}

QAction *WdgRoutes::menubarActionDuplicate() {
    return _actionDuplicate;
}

QAction *WdgRoutes::menubarActionDelete() {
    return _actionDelete;
}

QAction *WdgRoutes::menubarActionExportListCurrent() {
    return _actionExportListCurrent;
}

QAction *WdgRoutes::menubarActionExportListAll() {
    return _actionExportListCurrent;
}


void WdgRoutes::refresh() {
    qDebug() << "refreshing route list...";

    refreshing = true;

    ui->twRoutes->setRowCount(0);
    tableReference.clear();

    if(!_currentLine)
        return;

    QList<Route *> routes = _currentLine->routes();
    routes = ProjectData::sortItems(routes);

    bool searchCodeEnabled;
    QString search = ui->leSearch->text();
    int searchCode = search.toInt(&searchCodeEnabled);

    int counter = 0;
    QList<LineDirection *> directions = _currentLine->directions();

    foreach(LineDirection *ld, directions) {
        QList<Route *> routes = _currentLine->routesToDirection(ld);
        routes = ProjectData::sortItems(routes);

        foreach(Route *r, routes) {
            if(!r->name().contains(search, Qt::CaseInsensitive))
                if(!searchCodeEnabled || r->code() != searchCode)
                    continue;

            tableReference << r;

            QString code = QString::number(r->code());

            QString firstBusstop, lastBusstop;
            if(r->busstopCount() != 0) {
                firstBusstop = r->firstBusstop()->name();
                lastBusstop = r->lastBusstop()->name();
            }

            if(code.length() == 1)
                code = "0" + code;

            QFont bold;
            bold.setBold(true);

            ui->twRoutes->insertRow(counter);
            ui->twRoutes->setItem(counter, 0, new QTableWidgetItem(code));
            ui->twRoutes->setItem(counter, 1, new QTableWidgetItem(ld->description()));
            ui->twRoutes->setItem(counter, 2, new QTableWidgetItem(r->name()));
            ui->twRoutes->setItem(counter, 3, new QTableWidgetItem(firstBusstop));
            ui->twRoutes->setItem(counter, 4, new QTableWidgetItem(lastBusstop));
            ui->twRoutes->setItem(counter, 5, new QTableWidgetItem(QString::number(r->busstopCount())));

            ui->twRoutes->item(counter, 2)->setFont(bold);

            if(_currentRoute == r)
                ui->twRoutes->setCurrentCell(counter, 2);

            counter++;
        }
    }

    for(int i = 0; i < ui->twRoutes->rowCount(); i++) {
        ui->twRoutes->setRowHeight(i, 15);
    }

    ui->twRoutes->resizeColumnsToContents();

    refreshing = false;
}

void WdgRoutes::on_twRoutes_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twRoutes->currentItem();

    int selectionCount = ui->twRoutes->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        _currentRoute = nullptr;
    else
        _currentRoute = tableReference[current->row()];

    refreshUI();

    emit currentRouteChanged(_currentRoute);
}

void WdgRoutes::omsiExport() {
    QString path = QFileDialog::getExistingDirectory(this, "", "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2/maps/Region Grundorf V5/TTData", QFileDialog::ShowDirsOnly);
    if(path == "")
        return;

    QDir dir(path);
    if(!dir.exists())
        return;

    for(int i = 0; i < tableReference.count(); i++) {
        Route *r = tableReference[i];
        QFile f(dir.path() + "/" + r->name() + ".ttp");
        if(!f.exists()) {
            qDebug() << "not found: " << r->name();
            continue;
        }

        f.open(QIODevice::ReadOnly);
        QTextStream s(&f);
        s.setEncoding(QStringConverter::Latin1);

        QString resultStr;
        while(!s.atEnd()) {
            QString line = s.readLine();
            if(line != "[profile]")
                resultStr += line + "\r\n";
            else
                break;
        }

        // export
        QString result = "";

        QList<TimeProfile *> profiles = r->timeProfiles();
        for(int i = 0; i < profiles.count(); i++) {
            TimeProfile *p = profiles[i];
            result += "[profile]\r\n" + p->name() + "\r\n" + QString::number(p->duration()) + "\r\n\r\n";

            for(int j = 0; j < r->busstopCount(); j++) {
                Busstop *b = r->busstopAt(j);
                TimeProfileItem *itm = p->busstop(b);
                if(!itm)
                    continue;

                result += "[profile_man_arr_time]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->depValue()) + "\r\n";
                if(itm->busstopMode() != TimeProfileItem::BusstopModeNormal) {
                    result += "[profile_otherstopping]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->busstopMode()) + "\r\n";
                }
            }
        }

        resultStr += result;

        QFile f2(dir.path() + "/" + r->name() + ".ttp");
        f2.open(QIODevice::WriteOnly);
        QTextStream s2(&f2);
        s2.setEncoding(QStringConverter::Latin1);
        s2 << resultStr;
        f2.close();
        f.close();
    }
}








