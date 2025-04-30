#include "LanguagesModel.h"

LanguagesModel::LanguagesModel(QObject *parent) :
    QAbstractListModel(parent) {

    reload();
}

int LanguagesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _languages.count();
}

QVariant LanguagesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole))
        return QVariant();

    switch(role) {
        case Qt::DisplayRole: return _languages[index.row()].nativeLanguageName();
        case Qt::UserRole:    return _languages[index.row()].name();
    }

    return QVariant();
}

QLocale LanguagesModel::language(const QModelIndex &index) {
    return index.isValid() ? _languages[index.row()] : QLocale();
}

QLocale LanguagesModel::language(const int &index) {
    return index < 0 || index >= _languages.count() ? QLocale() : _languages[index];
}

int LanguagesModel::indexOfLanguage(const QLocale &locale) {
    return _languages.indexOf(locale);
}

void LanguagesModel::reload() {
    beginResetModel();
    _languages = GlobalConfig::supportedLanguages();
    endResetModel();
}
