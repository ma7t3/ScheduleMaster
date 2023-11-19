#include "wdgroutes.h"
#include "ui_wdgroutes.h"

#include <QUndoStack>
#include <QMessageBox>

#include "App/global.h"
#include "Dialogs/DlgRouteeditor.h"
#include "Dialogs/DlgDataexporter.h"
#include "Commands/cmdRoutes.h"

WdgRoutes::WdgRoutes(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgRoutes),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QObject::connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbEdit, SIGNAL(clicked()), this, SLOT(actionEdit()));
    QObject::connect(ui->pbDuplicate, SIGNAL(clicked()), this, SLOT(actionDuplicate()));
    QObject::connect(ui->twRoutes, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(actionEdit()));
    QObject::connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));
    QObject::connect(ui->pbExportProfiles, SIGNAL(clicked()), this, SLOT(actionExportProfiles()));

    m_actionNew->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_R));
    m_actionEdit->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_R));

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
    if(!m_currentLine)
        return;

    Route * r = new Route(global::getNewID(), 1, "", m_currentLine->directionAt(0));
    routeEditor dlg(this, true, r, m_currentLine->directions(), projectData->busstops());
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

    undoStack->push(new cmdRouteNew(m_currentLine, r));
    refreshRouteTable();
}

void WdgRoutes::actionEdit() {
    if(!m_currentLine || !m_currentRoute)
        return;
    
    QList<Route *> matchingRoutes = projectData->matchingRoutes(m_currentRoute);
    
    routeEditor dlg(this, false, m_currentRoute, m_currentLine->directions(), projectData->busstops(), matchingRoutes);

    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Route newR = *m_currentRoute;

    newR.setName(dlg.name());
    newR.setCode(dlg.getCode());
    newR.setDirection(dlg.getDirection());
    newR.clearBusstopList();

    QStringList busstopList = dlg.getBusstopList();

    for(int i = 0; i < busstopList.count(); i++) {
        Busstop * b = projectData->busstop(busstopList[i]);
        newR.addBusstop(b);
    }

    undoStack->push(new cmdRouteEdit(m_currentRoute, newR));
    refreshRouteTable();
}

void WdgRoutes::actionDuplicate() {
    if(!m_currentLine || !m_currentRoute)
        return;
    
    QList<Route *> matchingRoutes = projectData->matchingRoutes(m_currentRoute);
    
    routeEditor dlg(this, false, m_currentRoute, m_currentLine->directions(), projectData->busstops(), matchingRoutes);
    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Route * n = new Route(global::getNewID(), dlg.getCode(), dlg.name(), dlg.getDirection());
    QStringList busstopList = dlg.getBusstopList();

    for(int i = 0; i < busstopList.count(); i++) {
        Busstop * b = projectData->busstop(busstopList[i]);
        n->addBusstop(b);
    }

    undoStack->push(new cmdRouteNew(m_currentLine, n));
    refreshRouteTable();
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

    undoStack->push(new cmdRoutesDelete(m_currentLine, routes));
    refreshRouteTable();
}

void WdgRoutes::actionExportProfiles() {
    if(!m_currentRoute)
        return;

    Route *r = m_currentRoute;

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
            if(itm->busstopMode() != TimeProfileItem::busstopModeNormal) {
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
    m_actionNew = actionNew;
    m_actionEdit = actionEdit;
    m_actionDuplicate = actionDuplicate;
    m_actionDelete = actionDelete;

    refreshUI();
}

void WdgRoutes::refreshUI() {
    int selectionCount = ui->twRoutes->selectionModel()->selectedRows(0).count();

    if(selectionCount == 0) {
        ui->twRoutes->setCurrentItem(nullptr);
        ui->pbEdit->setEnabled(false);
        ui->pbDuplicate->setEnabled(false);
        ui->pbDelete->setEnabled(false);
        m_actionEdit->setEnabled(false);
        m_actionDuplicate->setEnabled(false);
        m_actionDelete->setEnabled(false);
    } else if(selectionCount == 1) {
        ui->pbEdit->setEnabled(true);
        ui->pbDuplicate->setEnabled(true);
        ui->pbDelete->setEnabled(true);
        m_actionEdit->setEnabled(true);
        m_actionDuplicate->setEnabled(true);
        m_actionDelete->setEnabled(true);
    } else {
        ui->pbEdit->setEnabled(false);
        ui->pbDuplicate->setEnabled(false);
        ui->pbDelete->setEnabled(true);
        m_actionEdit->setEnabled(false);
        m_actionDuplicate->setEnabled(false);
        m_actionDelete->setEnabled(true);
    }
}

void WdgRoutes::setCurrentLine(Line *l) {
    if(!l) {
        ui->pbNew->setEnabled(false);
        m_actionNew->setEnabled(false);
        m_currentLine = nullptr;
    }

    ui->pbNew->setEnabled(true);
    m_actionNew->setEnabled(true);

    m_currentLine = l;
    refreshRouteTable();
}

Route * WdgRoutes::currentRoute() {
    return m_currentRoute;
}

QAction *WdgRoutes::menubarActionNew() {
    return m_actionNew;
}

QAction *WdgRoutes::menubarActionEdit() {
    return m_actionEdit;
}

QAction *WdgRoutes::menubarActionDuplicate() {
    return m_actionDuplicate;
}

QAction *WdgRoutes::menubarActionDelete() {
    return m_actionDelete;
}

QAction *WdgRoutes::menubarActionExportListCurrent() {
    return m_actionExportListCurrent;
}

QAction *WdgRoutes::menubarActionExportListAll() {
    return m_actionExportListCurrent;
}


void WdgRoutes::refreshRouteTable() {
    refreshing = true;

    ui->twRoutes->setRowCount(0);
    tableReference.clear();

    if(!m_currentLine)
        return;

    QList<Route *> routes = m_currentLine->routes();
    routes = ProjectData::sortRoutes(routes);
    ui->twRoutes->setRowCount(routes.count());

    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];

        tableReference << r;

        QString direction = r->direction() ? "+" : "-";
        QString code = QString::number(r->code());
        QString name = r->name();

        if(code.length() == 1)
            code = "0" + code;

        ui->twRoutes->setItem(i, 0, new QTableWidgetItem(direction));
        ui->twRoutes->setItem(i, 1, new QTableWidgetItem(code));
        ui->twRoutes->setItem(i, 2, new QTableWidgetItem(name));

        if(m_currentRoute == r)
            ui->twRoutes->setCurrentCell(i, 2);
    }

    for(int i = 0; i < ui->twRoutes->rowCount(); i++) {
        ui->twRoutes->setRowHeight(i, 15);
    }

    refreshing = false;
}

void WdgRoutes::on_twRoutes_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twRoutes->currentItem();

    int selectionCount = ui->twRoutes->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        m_currentRoute = nullptr;
    else
        m_currentRoute = tableReference[current->row()];

    refreshUI();

    emit currentRouteChanged(m_currentRoute);
}

