#ifndef BUSSTOPPLATFORMTABLEMODEL_H
#define BUSSTOPPLATFORMTABLEMODEL_H

#include "ProjectDataModels/UnorderedProjectDataRowModel.h"
#include "src/projectdata/Busstop.h"

class BusstopPlatformTableModel : public UnorderedProjectDataRowModel<BusstopPlatform> {
    Q_OBJECT

public:
    explicit BusstopPlatformTableModel(QObject *parent = nullptr);

    void setBusstop(Busstop *busstop);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    static QString platformFlagsToString(const BusstopPlatformFlags &flags);

protected:
    virtual PDISet<BusstopPlatform> fetch() const override;

protected slots:
    void onDefaultPlatformChanged(BusstopPlatform *before, BusstopPlatform *after);

private:
    Busstop *_busstop;
};

#endif // BUSSTOPPLATFORMTABLEMODEL_H
