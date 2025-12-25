#ifndef LANGUAGESMODEL_H
#define LANGUAGESMODEL_H

#include "src/namespace.h"
#include "src/core/LanguageServiceImpl.h"

#include <QAbstractListModel>

class LanguagesModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LanguagesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    SMA::LanguageConfig language(const QModelIndex &index);
    SMA::LanguageConfig language(const int &index);
    int indexOfLanguage(const SMA::LanguageConfig &language);
    int indexOfLanguage(const QLocale::Language &language);

    void reload();

private:
    QList<SMA::LanguageConfig> _languages;
};

#endif // LANGUAGESMODEL_H
