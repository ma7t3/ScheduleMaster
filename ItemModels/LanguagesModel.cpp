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
        case Qt::DisplayRole: return _languages[index.row()].locale.nativeLanguageName();
        case Qt::UserRole:    return _languages[index.row()].locale.name();
    }

    return QVariant();
}

Language LanguagesModel::language(const QModelIndex &index) {
    return index.isValid() ? _languages[index.row()] : Language();
}

Language LanguagesModel::language(const int &index) {
    return index < 0 || index >= _languages.count() ? Language() : _languages[index];
}

int LanguagesModel::indexOfLanguage(const Language &language) {
    return _languages.indexOf(language);
}

int LanguagesModel::indexOfLanguage(const QLocale::Language &language) {
    for(int i = 0; i < _languages.size(); i++) {
        if(_languages[i].locale.language() == language)
            return i;
    }
    return -1;
}

void LanguagesModel::reload() {
    beginResetModel();
    _languages = LanguagesHandler::supportedLanguages();
    endResetModel();
}
