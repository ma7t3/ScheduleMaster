#ifndef BUSSTOPTABLEMODEL_H
#define BUSSTOPTABLEMODEL_H

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QPainter>

#include "src/projectdata/model/UnorderedProjectDataRowModel.h"
#include "src/projectdata/ProjectData.h"
#include "Global/IconController.h"

class BusstopTableModelDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    BusstopTableModelDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

protected:
    void paintRoleIcons(QPainter *painter, const QStyleOptionViewItem &option, Busstop *busstop) const;
    void paintPlatforms(QPainter *painter, const QStyleOptionViewItem &option, Busstop *busstop) const;
    void paintLines(QPainter *painter, const QStyleOptionViewItem &option, Busstop *busstop) const;

    QSize calculatePlatformsSizeHint(const QStyleOptionViewItem &option, Busstop *busstop) const;

    Busstop *busstopAtIndex(const QModelIndex &index) const;

    static QStringList iconIDsList(const BusstopFlags &flags);

    static inline int BoxVerticalPadding    = 2;
    static inline int BoxHorizontalPadding  = 4;
    static inline int ItemHorizontalPadding = 4;
    static inline int ItemGap               = 4;
    static inline int ItemBorderRadius      = 4;

private:
    ProjectData *_projectData;
};

class BusstopTableModel : public UnorderedProjectDataRowModel<Busstop> {
    Q_OBJECT

public:
    explicit BusstopTableModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual PDISet<Busstop> fetch() const override;

    static QString platformsToString(Busstop *b);
    static QString flagsToString(const BusstopFlags &flags);

private:
    ProjectData *_projectData;
};

#endif // BUSSTOPTABLEMODEL_H
