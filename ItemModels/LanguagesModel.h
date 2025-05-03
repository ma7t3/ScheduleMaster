#ifndef LANGUAGESMODEL_H
#define LANGUAGESMODEL_H

#include <QAbstractListModel>

#include "Global/LanguageManager.h"

class LanguagesModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LanguagesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    LanguageConfig language(const QModelIndex &index);
    LanguageConfig language(const int &index);
    int indexOfLanguage(const LanguageConfig &language);
    int indexOfLanguage(const QLocale::Language &language);

    void reload();

private:
    QList<LanguageConfig> _languages;
};

#endif // LANGUAGESMODEL_H
