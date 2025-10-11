#include "LineSelectionListModel.h"

#include "ApplicationInterface.h"

#include <QFont>

LineSelectionListModel::LineSelectionListModel(QObject *parent) :
    UnorderedProjectDataRowModel<Line>(parent), _projectData(ApplicationInterface::projectData()),
    _checkboxesEnabled(false) {
    connect(_projectData, &ProjectData::cleared,     this, &LineSelectionListModel::reset);

    connect(_projectData, &ProjectData::lineAdded,   this, &LineSelectionListModel::onItemAdded);
    connect(_projectData, &ProjectData::lineRemoved, this, &LineSelectionListModel::onItemRemoved);

    reset();
}

int LineSelectionListModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 3;
}

QVariant LineSelectionListModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
            return tr("Selected");
        case 1:
            return tr("Name");
        case 2:
            return tr("Description");
        }
    }

    return QVariant();
}

QVariant LineSelectionListModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()
       || (role != Qt::DisplayRole && role != Qt::ForegroundRole && role != Qt::BackgroundRole
           && role != Qt::FontRole && role != Qt::TextAlignmentRole &&  role != Qt::CheckStateRole))
        return QVariant();

    Line *l = itemAt(index.row());

    switch(role) {
    case Qt::TextAlignmentRole:
        if(index.column() == 1) {
            return Qt::AlignCenter;
        }
        break;

    case Qt::FontRole:
        if(index.column() == 1) {
            QFont f;
            f.setBold(true);
            return f;
        }
        break;

    case Qt::BackgroundRole:
        if(index.column() == 1) {
            return l->color();
        }
        break;

    case Qt::ForegroundRole:
        if(index.column() == 1) {
            return Global::contrastColor(l->color());
        }
        break;

    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return QVariant();
        case 1:
            return l->name();

        case 2:
            return l->description();
        }

    case Qt::CheckStateRole:
        return index.column() == 0 ? _checkedLines.contains(l) ? Qt::Checked : Qt::Unchecked
                                   : QVariant();
    }

    return QVariant();
}

bool LineSelectionListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::CheckStateRole)
        return false;

    Line *l = static_cast<Line *>(index.internalPointer());
    if(value.toBool())
        _checkedLines.add(l);
    else
        _checkedLines.remove(l);

    emit checkedLinesChanged(_checkedLines);

    return true;
}

Qt::ItemFlags LineSelectionListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if(_checkboxesEnabled)
        f |= Qt::ItemIsUserCheckable;
    return f;
}

void LineSelectionListModel::setCheckboxesEnabled(const bool &b) {
    emit layoutAboutToBeChanged();
    _checkboxesEnabled = b;
    emit layoutChanged();
}

PDISet<Line> LineSelectionListModel::checkedLines() const {
    return _checkedLines;
}

void LineSelectionListModel::clearCheckedLines() {
    _checkedLines.clear();
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), {Qt::CheckStateRole});
}

PDISet<Line> LineSelectionListModel::fetch() const {
    return _projectData->lines();
}
