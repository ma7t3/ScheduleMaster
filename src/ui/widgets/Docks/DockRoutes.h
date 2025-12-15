#ifndef DOCKROUTES_H
#define DOCKROUTES_H

#include <QWidget>
#include <QPersistentModelIndex>
#include "DockAbstract.h"

#include "src/ui/widgets/WdgTableColumnVisibilitySelector.h"
#include "ProjectData/Route.h"

class ProjectData;
class Line;
class RouteTableModel;
class RouteTableProxyModel;

namespace Ui {
class DockRoutes;
}

class DockRoutes : public DockAbstract {
    Q_OBJECT

public:
    explicit DockRoutes(QWidget *parent = nullptr);
    ~DockRoutes();

    Line *currentLine() const;
    Route *currentRoute() const;
    PDISet<Route> selectedRoutes() const;

public slots:
    void setLine(Line *line);

protected slots:
    void onRouteNew();
    void onRouteEdit();
    void onRouteDuplicate();
    void onRouteDelete();

signals:
    void currentRouteChanged(Route *);
    void selectedRoutesChaned(PDISet<Route>);

private:
    Ui::DockRoutes *ui;
    QAction *_actionNew, *_actionEdit, *_actionDuplicate, *_actionDelete, *_actionSearch, *_actionFilter, *_actionClearFilter;

    WdgTableColumnVisibilitySelector *_columnVisibilitySelector;
    ProjectData *_projectData;

    Line *_line;
    RouteTableModel *_model;
    RouteTableProxyModel *_proxyModel;
};

#endif // DOCKROUTES_H
