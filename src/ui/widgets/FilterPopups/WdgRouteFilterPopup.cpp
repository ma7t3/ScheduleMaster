#include "WdgRouteFilterPopup.h"
#include "ui_WdgRouteFilterPopup.h"

#include <QSortFilterProxyModel>

#include "Global/ActionController.h"
#include "ProjectDataModels/LineDirectionTableModel.h"
#include "ProjectDataModels/BusstopTableModel.h"
#include "ProjectData/Line.h"

RouteFilterPopupBusstopProxyModel::RouteFilterPopupBusstopProxyModel(QObject *parent) : QSortFilterProxyModel(parent), _referenceLine(nullptr) {}

void RouteFilterPopupBusstopProxyModel::setReferenceLine(Line *line) {
    if(_referenceLine)
        _referenceLine->disconnect(this);

    for(Route *r : std::as_const(_relevantRoutes))
        r->disconnect(this);

    _relevantRoutes.clear();

    _referenceLine = line;

    if(_referenceLine) {
        connect(_referenceLine, &Line::routeAdded, this, [this](Route *route) {
            _relevantRoutes.add(route);
            connect(route, &Route::changed, this, [this]() { invalidateFilter(); });
            invalidateFilter();
        });

        connect(_referenceLine, &Line::routeRemoved, this, [this](Route *route) {
            _relevantRoutes.remove(route);
            route->disconnect(this);
            invalidateFilter();
        });

        _relevantRoutes = _referenceLine->routes();
        for(Route *r : std::as_const(_relevantRoutes))
            connect(r, &Route::changed, this, [this]() { invalidateFilter(); });

        connect(_referenceLine, &QObject::destroyed, this, [this]() { setReferenceLine(nullptr); });
    }

    invalidateFilter();
}

bool RouteFilterPopupBusstopProxyModel::filterAcceptsRow(int sourceRow,
                                                         const QModelIndex &sourceParent) const {
    Q_UNUSED(sourceParent);

    Busstop *b = static_cast<Busstop *>(sourceModel()->index(sourceRow, 0).internalPointer());

    bool ok = false;
    for(Route *r : _relevantRoutes)
        if(r->busstopCount() > 0)
            ok |= (r->firstBusstop()->busstop() == b) || (r->lastBusstop()->busstop() == b);

    return ok;
}

WdgRouteFilterPopup::WdgRouteFilterPopup(QWidget *parent) :
    WdgFilterPopupContent(parent), ui(new Ui::WdgRouteFilterPopup),
    _directionModel(new LineDirectionTableModel(this)),
    _busstopModel(new BusstopTableModel(this)), _busstopProxyModel(new RouteFilterPopupBusstopProxyModel(this)) {
    ui->setupUi(this);

    _busstopProxyModel->setSourceModel(_busstopModel);
    _busstopProxyModel->setSortLocaleAware(true);
    _busstopProxyModel->sort(0, Qt::AscendingOrder);

    ui->cbDirection->setModel(_directionModel);
    ui->cbFirstBusstop->setModel(_busstopProxyModel);
    ui->cbLastBusstop->setModel(_busstopProxyModel);

    connect(ui->cbDirection,    &QComboBox::currentIndexChanged, this, &WdgFilterPopupContent::filterChanged);
    connect(ui->cbFirstBusstop, &QComboBox::currentIndexChanged, this, &WdgFilterPopupContent::filterChanged);
    connect(ui->cbLastBusstop,  &QComboBox::currentIndexChanged, this, &WdgFilterPopupContent::filterChanged);

    ActionController::add(ui->tbDirectionClear,    "projectDataTable.filter.clear", ActionController::IconComponent);
    ActionController::add(ui->tbFirstBusstopClear, "projectDataTable.filter.clear", ActionController::IconComponent);
    ActionController::add(ui->tbLastBusstopClear,  "projectDataTable.filter.clear", ActionController::IconComponent);

    connect(ui->tbDirectionClear, &QAbstractButton::clicked, this, [this]() {
        ui->cbDirection->setCurrentIndex(-1);
    });

    connect(ui->tbFirstBusstopClear, &QAbstractButton::clicked, this, [this]() {
        ui->cbFirstBusstop->setCurrentIndex(-1);
    });

    connect(ui->tbLastBusstopClear, &QAbstractButton::clicked, this, [this]() {
        ui->cbLastBusstop->setCurrentIndex(-1);
    });

    reset();
}

WdgRouteFilterPopup::~WdgRouteFilterPopup() {
    delete ui;
}

bool WdgRouteFilterPopup::filterIsEnabled() const {
    return ui->cbDirection->currentIndex() != -1 || ui->cbFirstBusstop->currentIndex() != -1
           || ui->cbLastBusstop->currentIndex() != -1;
}

LineDirection *WdgRouteFilterPopup::filterDirection() const {
    const int index = ui->cbDirection->currentIndex();
    return index == -1 ? nullptr : _directionModel->itemAt(index);
}

Busstop *WdgRouteFilterPopup::filterFirstBusstop() const {
    const int index = ui->cbFirstBusstop->currentIndex();
    return index == -1 ? nullptr : _busstopModel->itemAt(_busstopProxyModel->mapToSource(_busstopProxyModel->index(index, 0)).row());
}

Busstop *WdgRouteFilterPopup::filterLastBusstop() const {
    const int index = ui->cbLastBusstop->currentIndex();
    return index == -1 ? nullptr : _busstopModel->itemAt(_busstopProxyModel->mapToSource(_busstopProxyModel->index(index, 0)).row());
}

void WdgRouteFilterPopup::setLine(Line *line) {
    if(!_line)
        reset();

    _line = line;
    _directionModel->setLine(line);
    _busstopProxyModel->setReferenceLine(line);
    ui->cbDirection->setCurrentIndex(-1);
    ui->cbFirstBusstop->setCurrentIndex(-1);
    ui->cbLastBusstop->setCurrentIndex(-1);
}

void WdgRouteFilterPopup::reset() {
    ui->cbDirection->setCurrentIndex(-1);
    ui->cbFirstBusstop->setCurrentIndex(-1);
    ui->cbLastBusstop->setCurrentIndex(-1);
}
