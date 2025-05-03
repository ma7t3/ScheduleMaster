#include "StylesModel.h"

StylesModel::StylesModel(QObject *parent) :
    QAbstractListModel(parent) {
    reload();
}

int StylesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _styles.size();
}

QVariant StylesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    Style style = _styles.at(index.row());
    return style.name;
}

QString StylesModel::style(const QModelIndex &index) {
    if(!index.isValid())
        return "";

    return style(index.row());
}

QString StylesModel::style(const int &row) {
    if(row < 0 || row >= _styles.size())
        return "";

    return _styles.at(row).id();
}

int StylesModel::indexOfStyle(const QString &styleID) {
    for(int i = 0; i < _styles.size(); i++)
        if(_styles.at(i).id() == styleID)
            return i;

    return -1;
}

void StylesModel::reload() {
    beginResetModel();
    _styles = StyleManager::items();
    endResetModel();
}
