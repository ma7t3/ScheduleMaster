#ifndef LANGUAGESMODEL_H
#define LANGUAGESMODEL_H

#include <QAbstractListModel>
#include "Global/GlobalConfig.h"
#include "Global/LocalConfig.h"

class LanguagesModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LanguagesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QLocale language(const QModelIndex &index);
    QLocale language(const int &index);
    int indexOfLanguage(const QLocale &locale);

    void reload();

private:
    QList<QLocale> _languages;
};

#endif // LANGUAGESMODEL_H
