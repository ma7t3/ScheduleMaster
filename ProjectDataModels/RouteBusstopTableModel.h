#ifndef ROUTEBUSSTOPTABLEMODEL_H
#define ROUTEBUSSTOPTABLEMODEL_H

#include "OrderedProjectDataRowModel.h"
#include "src/projectdata/RouteBusstopItem.h"
#include <QStyledItemDelegate>

class Route;

class RouteBusstopTableDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    RouteBusstopTableDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
};

class RouteBusstopTableModel : public OrderedProjectDataRowModel<RouteBusstopItem> {
    Q_OBJECT

public:
    explicit RouteBusstopTableModel(QObject *parent = nullptr);

    void setRoute(Route *route);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

protected:
    virtual PDIList<RouteBusstopItem> fetch() const override;

private:
    Route *_route;
};

#endif // ROUTEBUSSTOPTABLEMODEL_H
