#include "LineDirectionListModel.h"

LineDirectionListModel::LineDirectionListModel(QObject *parent) : OrderedProjectDataRowModel<LineDirection>(parent) {}

void LineDirectionListModel::setLine(Line *newLine) {
    _line = newLine;
    connect(_line, &Line::directionsAdded,   this, &LineDirectionListModel::addItems);
    connect(_line, &Line::directionsChanged, this, &LineDirectionListModel::changeItems);
    connect(_line, &Line::directionsRemoved, this, &LineDirectionListModel::removeItems);
    reset();
}

QVariant LineDirectionListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);
    return QVariant();
}

int LineDirectionListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant LineDirectionListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    LineDirection *ld = _items[index.row()];

    switch(index.column()) {
    case 0:
        switch(role) {
        case Qt::DisplayRole:
            return ld->description();
        }
    }

    return QVariant();
}

QList<LineDirection *> LineDirectionListModel::fetchData() const {
    return _line->directions();
}
