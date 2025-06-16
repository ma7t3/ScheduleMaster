#include "BusstopTableModel.h"

#include "ApplicationInterface.h"

BusstopTableModel::BusstopTableModel(QObject *parent) :
    UnorderedProjectDataRowModel(parent), _projectData(ApplicationInterface::projectData()) {
    connect(_projectData, &ProjectData::cleared,  this, &BusstopTableModel::reset);

    connect(_projectData, &ProjectData::busstopAdded, this, &BusstopTableModel::onItemAdded);
    connect(_projectData, &ProjectData::busstopRemoved, this, &BusstopTableModel::onItemRemoved);
}

QVariant BusstopTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Vertical && role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Lines");
    }

    return QVariant();
}

int BusstopTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2;
}

QVariant BusstopTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();

    Busstop *b = itemAt(index.row());

    switch(index.column()) {
        case 0: return b->name();
        case 1: return QVariant(); //TODO
    }

    return QVariant();
}

PDISet<Busstop> BusstopTableModel::fetch() const {
    return _projectData->busstops();
}
