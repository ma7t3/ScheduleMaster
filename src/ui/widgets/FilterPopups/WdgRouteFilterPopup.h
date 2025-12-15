#ifndef WDGROUTEFILTERPOPUP_H
#define WDGROUTEFILTERPOPUP_H

#include "WdgFilterPopupContent.h"

#include "ProjectData/Route.h"

#include <QSortFilterProxyModel>

class LineDirectionTableModel;
class BusstopTableModel;
class Busstop;
class Line;
class LineDirection;

namespace Ui {
class WdgRouteFilterPopup;
}

class RouteFilterPopupBusstopProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    RouteFilterPopupBusstopProxyModel(QObject *parent);

    void setReferenceLine(Line *line);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    Line *_referenceLine;
    PDISet<Route> _relevantRoutes;
};

class WdgRouteFilterPopup : public WdgFilterPopupContent {
    Q_OBJECT

public:
    explicit WdgRouteFilterPopup(QWidget *parent = nullptr);
    ~WdgRouteFilterPopup();

    bool filterIsEnabled() const override;
    LineDirection *filterDirection() const;
    Busstop *filterFirstBusstop() const;
    Busstop *filterLastBusstop() const;

public slots:
    void setLine(Line *line);
    void reset() override;

private:
    Ui::WdgRouteFilterPopup *ui;

    LineDirectionTableModel *_directionModel;
    BusstopTableModel *_busstopModel;
    RouteFilterPopupBusstopProxyModel *_busstopProxyModel;

    Line *_line;
};

#endif // WDGROUTEFILTERPOPUP_H
