#ifndef WDGROUTESELECTOR_H
#define WDGROUTESELECTOR_H

#include <QWidget>

#include <QTreeWidgetItem>

#include "ProjectData/projectdata.h"

class WdgRouteSelector : public QTreeWidget
{
    Q_OBJECT

public:
    explicit WdgRouteSelector(QWidget *parent);
    void setProjectData(ProjectData *projectData);
    QList<Route *> routes();

public slots:
    void refresh();
    void setSelectedRoutes(QList<Route *>);
    void setFilterBusstop(Busstop *b);
    void expandAll();

private slots:
    void refreshCheckBoxRelations(QTreeWidgetItem *);

signals:
    void routesChanged(QList<Route *>);

private:
    ProjectData *_projectData;
    Busstop *_filterBusstop;

    QList<QList<QList<Route *>>> _routesReference;
    QList<QList<LineDirection *>> _routesDirectionsReference;
    QList<Line *> _routesLinesReference;

    bool _userClicked = false;
};

#endif // WDGROUTESELECTOR_H
