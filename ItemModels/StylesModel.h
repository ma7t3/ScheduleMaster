#ifndef STYLESMODEL_H
#define STYLESMODEL_H

#include <QAbstractListModel>

#include "Global/StyleManager.h"

class StylesModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit StylesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString style(const QModelIndex &index);
    QString style(const int &row);
    int indexOfStyle(const QString &styleID);

    void reload();

private:
    QList<StyleConfig> _styles;
};

#endif // STYLESMODEL_H
