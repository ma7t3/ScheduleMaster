#include "BusstopPlatformTableModel.h"

#include "Global/IconController.h"

#include <QFont>
#include <QIcon>

BusstopPlatformTableModel::BusstopPlatformTableModel(QObject *parent) :
    UnorderedProjectDataRowModel(parent), _busstop(nullptr) {
}

void BusstopPlatformTableModel::setBusstop(Busstop *busstop) {
    if(_busstop)
        _busstop->disconnect(this);

    _busstop = busstop;
    reset();
    connect(_busstop, &Busstop::platformAdded,          this, &BusstopPlatformTableModel::onItemAdded);
    connect(_busstop, &Busstop::platformRemoved,        this, &BusstopPlatformTableModel::onItemRemoved);
    connect(_busstop, &Busstop::defaultPlatformChanged, this, &BusstopPlatformTableModel::onDefaultPlatformChanged);
    connect(_busstop, &QObject::destroyed,              this, [this]() { setBusstop(nullptr); });
}

int BusstopPlatformTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 3;
}

QVariant BusstopPlatformTableModel::headerData(int section, Qt::Orientation orientation,
                                               int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Roles");
    case 2:
        return tr("Comment");
    }
    return QVariant();
}

QVariant BusstopPlatformTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole && role != Qt::DecorationRole && role != Qt::FontRole)
        return QVariant();
    BusstopPlatform *platform = itemAt(index.row());

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return platform->name();
        case 1:
            return platformFlagsToString(platform->flags());
        case 2:
            return platform->comment().replace("\n", " ");
        }
        break;
    case Qt::DecorationRole:
        switch(index.column()) {
        case 0:
            return IconController::icon(_busstop->isDefaultPlatform(platform) ? "circle-check"
                                                                              : "busstop");
        case 1:
            return QVariant();
        }
        break;
    case Qt::FontRole:
        if(_busstop->isDefaultPlatform(platform)) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    return QVariant();
}

QString BusstopPlatformTableModel::platformFlagsToString(const BusstopPlatformFlags &flags) {
    if(flags == BusstopPlatformFlag::StandardBusstopPlatform)
        return "";

    QStringList strings;
    if(flags.testFlag(BusstopPlatformFlag::ArrivalBusstopPlatform))
        strings << tr("Arrival");

    if(flags.testFlag(BusstopPlatformFlag::BreakBusstopPlatform))
        strings << tr("Break");

    if(flags.testFlag(BusstopPlatformFlag::DepartureBusstopPlatform))
        strings << tr("Departure");

    if(flags.testFlag(BusstopPlatformFlag::WaitBusstopPlatform))
        strings << tr("Waiting");

    if(flags.testFlag(BusstopPlatformFlag::InternalBusstopPlatform))
        strings << tr("Internal");

    return strings.join(", ");
}

PDISet<BusstopPlatform> BusstopPlatformTableModel::fetch() const {
    if(_busstop)
        return _busstop->platforms();
    return PDISet<BusstopPlatform>();
}

void BusstopPlatformTableModel::onDefaultPlatformChanged(BusstopPlatform *before,
                                                         BusstopPlatform *after) {
    if(before) onItemUpdated(before);
    if(after)  onItemUpdated(after);
}
