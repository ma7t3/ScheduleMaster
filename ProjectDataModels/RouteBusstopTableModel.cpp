#include "RouteBusstopTableModel.h"

#include "ProjectData/Route.h"
#include <QFont>
#include <QComboBox>

RouteBusstopTableDelegate::RouteBusstopTableDelegate(QObject *parent) :
    QStyledItemDelegate(parent) {}

QWidget *RouteBusstopTableDelegate::createEditor(QWidget *parent,
                                                                const QStyleOptionViewItem &,
                                                                const QModelIndex &index) const {
    RouteBusstopItem *b = static_cast<RouteBusstopItem *>(index.internalPointer());
    if(!b)
        return nullptr;

    if(b->busstop()->platformCount() == 0)
        return nullptr;

    QComboBox *cbx = new QComboBox(parent);
    QList<BusstopPlatform *> platforms = b->busstop()->platforms().values();
    std::sort(platforms.begin(), platforms.end(), [](BusstopPlatform *a, BusstopPlatform *b) {
        return a->name() < b->name();
    });
    for(BusstopPlatform *bp : platforms) {
        cbx->addItem(bp->name());
        cbx->setItemData(cbx->count() - 1, bp->id());
    }

    return cbx;
}

void RouteBusstopTableDelegate::setEditorData(QWidget *editor,
                                                             const QModelIndex &index) const {
    QComboBox *cbx = static_cast<QComboBox *>(editor);
    RouteBusstopItem *b = static_cast<RouteBusstopItem *>(index.internalPointer());
    BusstopPlatform *bp = b->defaultPlatform();
    if(!bp)
        return;

    const int row = cbx->findData(bp->id());
    cbx->setCurrentIndex(row);
}

void RouteBusstopTableDelegate::setModelData(QWidget *editor,
                                                            QAbstractItemModel *model,
                                                            const QModelIndex &index) const {
    QComboBox *cbx = static_cast<QComboBox *>(editor);
    RouteBusstopItem *b = static_cast<RouteBusstopItem *>(index.internalPointer());
    b->setDefaultPlatform(b->busstop()->platform(cbx->currentData().toUuid()));
}

RouteBusstopTableModel::RouteBusstopTableModel(QObject *parent) :
    OrderedProjectDataRowModel<RouteBusstopItem>(parent), _route(nullptr) {}

void RouteBusstopTableModel::setRoute(Route *route) {
    if(_route)
        _route->disconnect(this);

    _route = route;
    reset();

    connect(_route, &Route::busstopAdded,   this, &RouteBusstopTableModel::onItemInserted);
    connect(_route, &Route::busstopRemoved, this, &RouteBusstopTableModel::onItemRemoved);
    connect(_route, &Route::busstopChanged, this, &RouteBusstopTableModel::onItemUpdated);
    connect(_route, &Route::busstopMoved,   this, &RouteBusstopTableModel::onItemMoved);
}

int RouteBusstopTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 3;
}

QVariant RouteBusstopTableModel::headerData(int section, Qt::Orientation orientation,
                                            int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("No.");

    case 1:
        return tr("Name");

    case 2:
        return tr("Platform");
    }

    return QVariant();
}

QVariant RouteBusstopTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole && role != Qt::FontRole)
        return QVariant();

    RouteBusstopItem *b = itemAt(index.row());
    BusstopPlatform *defaultPlatform = b->defaultPlatform();

    if(role == Qt::FontRole
       && b->busstop()->flags().testAnyFlags(BusstopFlag::FinalBusstop | BusstopFlag::TransferBusstop
                                  | BusstopFlag::CentralTransferBusstop)) {
        QFont f;
        f.setBold(true);
        return f;
    } else if(role == Qt::FontRole)
        return QVariant();

    switch(index.column()) {
    case 0:
        return index.row();

    case 1:
        return b->busstop()->name();

    case 2:
        return defaultPlatform ? defaultPlatform->name() : "";
    }

    return QVariant();
}

Qt::ItemFlags RouteBusstopTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (index.column() == 2)
        f |= Qt::ItemIsEditable;
    return f;
}

PDIList<RouteBusstopItem> RouteBusstopTableModel::fetch() const {
    return _route->busstops();
}
