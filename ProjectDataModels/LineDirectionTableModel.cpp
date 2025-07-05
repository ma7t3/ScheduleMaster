#include "LineDirectionTableModel.h"

LineDirectionTableModel::LineDirectionTableModel(QObject *parent) :
    OrderedProjectDataRowModel(parent), _line(nullptr) {}

void LineDirectionTableModel::setLine(Line *line) {
    if(_line)
        _line->disconnect(this);

    _line = line;
    reset();
    connect(_line, &Line::directionAdded,   this, &LineDirectionTableModel::onItemInserted);
    connect(_line, &Line::directionRemoved, this, &LineDirectionTableModel::onItemRemoved);
    connect(_line, &Line::directionChanged, this, &LineDirectionTableModel::onItemUpdated);
    connect(_line, &Line::directionMoved,   this, &LineDirectionTableModel::onItemMoved);
}

int LineDirectionTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant LineDirectionTableModel::headerData(int section, Qt::Orientation orientation,
                                             int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    }

    return QVariant();
}

QVariant LineDirectionTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();

    LineDirection *ld = itemAt(index.row());

    return ld->description();
}

PDIList<LineDirection> LineDirectionTableModel::fetch() const {
    return _line->directions();
}
