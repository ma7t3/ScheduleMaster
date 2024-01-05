#ifndef WDGROUTESELECTOR_H
#define WDGROUTESELECTOR_H

#include <QWidget>

#include <QTreeWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgRouteSelector;
}

class WdgRouteSelector : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRouteSelector(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~WdgRouteSelector();

    void setProjectData(ProjectData *projectData);

public slots:
    void refresh();
    void setSelectedRoutes(QList<Route *>);

private slots:
    void refreshCheckBoxRelations(QTreeWidgetItem *);
    void emitRoutesChangedSignal();

signals:
    void routesChanged(QList<Route *>);

private:
    Ui::WdgRouteSelector *ui;

    ProjectData *_projectData;

    QList<QList<QList<Route *>>> _routesReference;
    QList<QList<LineDirection *>> _routesDirectionsReference;
    QList<Line *> _routesLinesReference;
    bool refreshingRouteCheckBoxes = false;
};

#endif // WDGROUTESELECTOR_H
