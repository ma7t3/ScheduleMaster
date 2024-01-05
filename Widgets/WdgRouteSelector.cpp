#include "WdgRouteSelector.h"
#include "ui_WdgRouteSelector.h"

WdgRouteSelector::WdgRouteSelector(QWidget *parent, ProjectData *projectData) :
    QWidget(parent),
    ui(new Ui::WdgRouteSelector),
    _projectData(projectData) {

    ui->setupUi(this);

    QObject::connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(refreshCheckBoxRelations(QTreeWidgetItem*)));
    QObject::connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(emitRoutesChangedSignal()));
}

WdgRouteSelector::~WdgRouteSelector() {
    delete ui;
}

void WdgRouteSelector::setProjectData(ProjectData *projectData) {
    _projectData = projectData;
}

QList<Route *> WdgRouteSelector::routes() {
    QList<Route *> list;

    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *itm = ui->treeWidget->topLevelItem(i);
        for(int j = 0; j < itm->childCount(); j++) {
            QTreeWidgetItem *subItm = itm->child(j);
            for(int k = 0; k < subItm->childCount(); k++) {
                QTreeWidgetItem *subSubItm = subItm->child(k);
                if(subSubItm->checkState(0) == Qt::Checked)
                    list << _routesReference[i][j][k];
            }
        }
    }
    return list;
}

void WdgRouteSelector::refresh() {
    ui->treeWidget->clear();

    _routesReference.clear();
    _routesDirectionsReference.clear();
    _routesLinesReference.clear();

    for(int i = 0; i < _projectData->lineCount(); i++) {
        Line *l = _projectData->lineAt(i);
        QTreeWidgetItem *lineItm = new QTreeWidgetItem({l->name()});
        lineItm->setCheckState(0, Qt::Unchecked);

        QList<LineDirection *> directions = l->directions();
        QList<LineDirection *> resultDirections;
        QList<QList<Route *>> resultRoutesParent;

        for(int j = 0; j < directions.count(); j++) {
            LineDirection *ld = directions[j];
            QTreeWidgetItem *directionItm = new QTreeWidgetItem({tr("to ") + ld->description()});
            directionItm->setCheckState(0, Qt::Unchecked);

            QList<Route *> routes = l->routesToDirection(ld);
            QList<Route *> resultRoutes;

            for(int k = 0; k < routes.count(); k++) {
                Route *r = routes[k];
                if(_filterBusstop && !r->hasBusstop(_filterBusstop))
                    continue;

                QTreeWidgetItem *routeItm = new QTreeWidgetItem(directionItm, {r->name()});
                routeItm->setCheckState(0, Qt::Unchecked);
                resultRoutes << r;
            }

            if(directionItm->childCount() != 0) {
                lineItm->addChild(directionItm);

                resultDirections << ld;
                resultRoutesParent << resultRoutes;
            }
        }

        if(lineItm->childCount() != 0) {
            ui->treeWidget->addTopLevelItem(lineItm);
            _routesLinesReference << l;
            _routesDirectionsReference << resultDirections;
            _routesReference << resultRoutesParent;
        }
    }
}

void WdgRouteSelector::setSelectedRoutes(QList<Route *> routeList) {
    // clear
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++) {
            for(int k = 0; k < ui->treeWidget->topLevelItem(i)->child(j)->childCount(); k++) {
                ui->treeWidget->topLevelItem(i)->child(j)->child(k)->setCheckState(0, Qt::Unchecked);
            }
        }
    }

    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            for(int k = 0; k < ui->treeWidget->topLevelItem(i)->child(j)->childCount(); k++)
                foreach(Route *r, routeList)
                    if(_routesReference[i][j][k] == r) {
                        ui->treeWidget->topLevelItem(i)->child(j)->child(k)->setCheckState(0, Qt::Checked);
                        break;
                    }
}

void WdgRouteSelector::setFilterBusstop(Busstop *b) {
    _filterBusstop = b;
}

void WdgRouteSelector::expandAll() {
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        ui->treeWidget->topLevelItem(i)->setExpanded(true);
        for(int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); j++)
            ui->treeWidget->topLevelItem(i)->child(j)->setExpanded(true);
    }
}

void WdgRouteSelector::refreshCheckBoxRelations(QTreeWidgetItem *changedItm) {
    if(refreshingRouteCheckBoxes)
        return;

    refreshingRouteCheckBoxes = true;

    // for each line
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *lineItm = ui->treeWidget->topLevelItem(i);
        bool lineChanged = lineItm == changedItm;

        bool checkedDirectionFound = false;
        bool partiallyCheckedDirectionFound = false;
        bool uncheckedDirectionFound = false;

        // for each direction
        for(int j = 0; j < lineItm->childCount(); j++) {
            QTreeWidgetItem *directionItm = lineItm->child(j);
            bool directionChanged = directionItm == changedItm;

            if(lineChanged)
                directionItm->setCheckState(0, lineItm->checkState(0));

            bool checkedRouteFound = false;
            bool uncheckedRouteFound = false;

            // for each route
            for(int k = 0; k < directionItm->childCount(); k++) {
                QTreeWidgetItem *routeItm = directionItm->child(k);
                if(lineChanged)
                    routeItm->setCheckState(0, lineItm->checkState(0));
                else if(directionChanged)
                    routeItm->setCheckState(0, directionItm->checkState(0));
                else {
                    if(routeItm->checkState(0) == Qt::Checked)
                        checkedRouteFound = true;
                    if(routeItm->checkState(0) == Qt::Unchecked)
                        uncheckedRouteFound = true;
                }
            }

            if(!directionChanged && !lineChanged) {
                if(checkedRouteFound && uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::PartiallyChecked);
                else if(checkedRouteFound && !uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::Checked);
                else if(!checkedRouteFound && uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::Unchecked);
            }

            if(!lineChanged) {
                if(directionItm->checkState(0) == Qt::Checked)
                    checkedDirectionFound = true;
                if(directionItm->checkState(0) == Qt::PartiallyChecked)
                    partiallyCheckedDirectionFound = true;
                if(directionItm->checkState(0) == Qt::Unchecked)
                    uncheckedDirectionFound = true;
            }
        }

        if(!lineChanged) {
            if((checkedDirectionFound && uncheckedDirectionFound) || partiallyCheckedDirectionFound)
                lineItm->setCheckState(0, Qt::PartiallyChecked);
            else if(checkedDirectionFound && !uncheckedDirectionFound)
                lineItm->setCheckState(0, Qt::Checked);
            else if(!checkedDirectionFound && uncheckedDirectionFound)
                lineItm->setCheckState(0, Qt::Unchecked);
        }
    }

    refreshingRouteCheckBoxes = false;
}

void WdgRouteSelector::emitRoutesChangedSignal() {
    emit routesChanged(routes());
}
