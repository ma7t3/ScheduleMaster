#include "LineTableModel.h"

#include "ApplicationInterface.h"

#include <QFont>

LineTableModel::LineTableModel(QObject *parent) : UnorderedProjectDataRowModel<Line>(parent), _projectData(ApplicationInterface::projectData()) {
    connect(_projectData, &ProjectData::cleared,     this, &LineTableModel::reset);

    connect(_projectData, &ProjectData::lineAdded,   this, &LineTableModel::onItemAdded);
    connect(_projectData, &ProjectData::lineRemoved, this, &LineTableModel::onItemRemoved);

    reset();
}

int LineTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2;
}

QVariant LineTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal
       || (role != Qt::DisplayRole
           && role != ColumnVisibleByDefaultRole
           && role != ColumnMandatoryRole))
        return QVariant();

    switch(role) {
    case ColumnVisibleByDefaultRole:
        return true;

    case ColumnMandatoryRole:
        return section == 0;

    case Qt::DisplayRole:
        switch(section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Description");
        }
    }

    return QVariant();
}

QVariant LineTableModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()
       || (role != Qt::DisplayRole && role != Qt::ForegroundRole && role != Qt::BackgroundRole
           && role != Qt::FontRole && role != Qt::TextAlignmentRole))
        return QVariant();

    Line *l = itemAt(index.row());

    switch(role) {
    case Qt::TextAlignmentRole:
        if(index.column() == 0) {
            return Qt::AlignCenter;
        }
        break;

    case Qt::FontRole:
        if(index.column() == 0) {
            QFont f;
            f.setBold(true);
            return f;
        }
        break;

    case Qt::BackgroundRole:
        if(index.column() == 0) {
            return l->color();
        }
        break;

    case Qt::ForegroundRole:
        if(index.column() == 0) {
            return Global::contrastColor(l->color());
        }
        break;

    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return l->name();

        case 1:
            return l->description();
        }
    }

    return QVariant();
}

PDISet<Line> LineTableModel::fetch() const {
    return _projectData->lines();
}
