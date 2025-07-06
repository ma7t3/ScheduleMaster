#ifndef DOCKROUTES_H
#define DOCKROUTES_H

#include <QWidget>
#include "DockAbstract.h"

#include "Widgets/WdgTableColumnVisibilitySelector.h"

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

public slots:
    void setLine(Line *line);

protected slots:
    void onRouteNew();
    void onRouteEdit();
    void onRouteDuplicate();
    void onRouteDelete();

private:
    Ui::DockRoutes *ui;
    QAction *_actionNew, *_actionEdit, *_actionDuplicate, *_actionDelete;

    WdgTableColumnVisibilitySelector *_columnVisibilitySelector;

    Line *_line;
    RouteTableModel *_model;
    RouteTableProxyModel *_proxyModel;
};

#endif // DOCKROUTES_H
