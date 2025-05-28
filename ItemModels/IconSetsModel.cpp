#include "IconSetsModel.h"

IconSetsModel::IconSetsModel(QObject *parent) : QAbstractListModel(parent) {
    reload();
}

int IconSetsModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _iconSets.size();
}

QVariant IconSetsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    return _iconSets.at(index.row()).name;
}

QString IconSetsModel::iconSet(const QModelIndex &index) {
    if(!index.isValid())
        return "";

    return iconSet(index.row());
}

QString IconSetsModel::iconSet(const int &row) {
    if(row < 0 | row >= _iconSets.size())
        return "";
    return _iconSets.at(row).id();
}

int IconSetsModel::indexOficonSet(const QString &iconSetID) {
    for(int i = 0; i < _iconSets.size(); i++)
        if(_iconSets.at(i).id() == iconSetID)
            return i;

    return -1;
}

void IconSetsModel::reload() {
    beginResetModel();
    _iconSets = IconSetManager::items();
    endResetModel();
}
