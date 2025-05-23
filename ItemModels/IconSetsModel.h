#ifndef ICONSETSMODEL_H
#define ICONSETSMODEL_H

#include <QAbstractListModel>

#include "Global/IconSetManager.h"

class IconSetsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IconSetsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString iconSet(const QModelIndex &index);
    QString iconSet(const int &row);
    int indexOficonSet(const QString &iconSetID);

    void reload();

private:
    QList<IconSetConfig> _iconSets;
};

#endif // ICONSETSMODEL_H
