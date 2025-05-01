#ifndef LANGUAGESMODEL_H
#define LANGUAGESMODEL_H

#include <QAbstractListModel>
#include "Global/GlobalConfig.h"
#include "Global/LocalConfig.h"

#include "Global/LanguagesHandler.h"

class LanguagesModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LanguagesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Language language(const QModelIndex &index);
    Language language(const int &index);
    int indexOfLanguage(const Language &language);
    int indexOfLanguage(const QLocale::Language &language);

    void reload();

private:
    QList<Language> _languages;
};

#endif // LANGUAGESMODEL_H
