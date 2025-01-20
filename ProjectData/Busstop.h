#ifndef BUSSTOP_H
#define BUSSTOP_H

#include <QObject>

#include "BusstopPlatform.h"

enum class BusstopFlag {
    StandardBusstop = 0x0,
    FinalBusstop = 0x1,
    TransferBusstop = 0x2,
    CentralTransferBusstop = 0x4,
    InternalBusstopPlatform = 0x8
};

Q_DECLARE_FLAGS(BusstopFlags, BusstopFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BusstopFlags)

class Busstop : public ProjectDataItem {
    Q_OBJECT
public:
    Busstop(QObject *parent, const QUuid &id = QUuid());
    Busstop(QObject *parent, const QJsonObject &);

    bool operator<(const Busstop &) const;

    struct Data {
        QString name;
        BusstopFlags flags;
        QHash<QUuid, BusstopPlatform *> platforms;
        BusstopPlatform *defaultPlatform;
    };

    Data data() const;
    void setData(const Data &);

    QString name() const;
    void setName(const QString &);

    BusstopFlags flags() const;
    void setFlags(const BusstopFlags &);

    int platformCount() const;
    BusstopPlatform *platform(const QUuid &id) const;
    QList<BusstopPlatform *> platforms() const;
    void addPlatform(BusstopPlatform *);
    void removePlatform(BusstopPlatform *);
    void removePlatform(const QUuid &id);

    QList<BusstopPlatform *> platformsWithFlag(const BusstopPlatformFlag &flag);

    BusstopPlatform *defaultPlatform() const;
    bool isDefaultPlatform(BusstopPlatform *) const;
    void setDefaultPlatform(BusstopPlatform *);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    Busstop::Data _data;
};

#endif // BUSSTOP_H
