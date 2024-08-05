#include "LineTableModel.h"

#include <QFont>
#include <QBrush>
#include "App/global.h"

LineTableModel::LineTableModel(QObject *parent) : UnorderedProjectDataRowModel(parent), _refreshSorting(false) {
    _dataFieldsCount = 2;
    connect(this, &QAbstractTableModel::rowsInserted, this, &LineTableModel::refreshDefaultSortIndexes);
    connect(this, &QAbstractTableModel::dataChanged,  this, &LineTableModel::refreshDefaultSortIndexes);
    connect(this, &QAbstractTableModel::rowsRemoved,  this, &LineTableModel::refreshDefaultSortIndexes);
    connect(this, &QAbstractTableModel::modelReset,   this, &LineTableModel::refreshDefaultSortIndexes);
}

void LineTableModel::setProjectData(ProjectData *newProjectData) {
    projectData = newProjectData;
    connect(projectData, &ProjectData::wasReset,     this, &LineTableModel::reset);
    connect(projectData, &ProjectData::linesAdded,   this, &LineTableModel::addItems);
    connect(projectData, &ProjectData::linesChanged, this, &LineTableModel::changeItems);
    connect(projectData, &ProjectData::linesRemoved, this, &LineTableModel::removeItems);
    reset();
}

QVariant LineTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Description");
    }

    return QVariant();
}

int LineTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _dataFieldsCount;
}

QVariant LineTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Line *l = _items[index.row()];

    switch(index.column()) {
    case 0:
        switch(role) {
            case Qt::DisplayRole:
                return l->name();

            case Qt::ForegroundRole:
                return QBrush(global::getContrastColor(l->color()));

            case Qt::BackgroundRole:
                return QBrush(l->color());

            case 0x0100:
                return _defaultSortIndexes[l];

            case Qt::FontRole:
                QFont f;
                f.setBold(true);
                return f;
        } break;
    case 1:
        switch(role) {
            case 0x0100:
            case Qt::DisplayRole: return l->description();
        }
    }

    return QVariant();
}

QList<Line *> LineTableModel::fetchData() const {
    return ProjectData::sortItems(projectData->lines());
}

void LineTableModel::refreshDefaultSortIndexes() {
    if(_refreshSorting)
        return;

    _refreshSorting = true;
    _defaultSortIndexes.clear();

    for(int i = 0; i < _items.count(); i++)
        _defaultSortIndexes.insert(_items[i], i);

    _refreshSorting = false;
}



