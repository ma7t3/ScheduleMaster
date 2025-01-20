#ifndef BUSSTOPPLATFORM_H
#define BUSSTOPPLATFORM_H

#include "ProjectDataItem.h"

enum class BusstopPlatformFlag {
    StandardBusstopPlatform = 0x0,
    ArrivalBusstopPlatform = 0x1,
    BreakBusstopPlatform = 0x2,
    DepartureBusstopPlatform = 0x4,
    WaitBusstopPlatform = 0x8,
    InternalBusstopPlatform = 0x10
};

Q_DECLARE_FLAGS(BusstopPlatformFlags, BusstopPlatformFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BusstopPlatformFlags)

class BusstopPlatform : public ProjectDataItem {
    Q_OBJECT
public:
    BusstopPlatform(QObject *parent, const QUuid &id = QUuid());
    BusstopPlatform(QObject *parent, const QJsonObject &);

    bool operator<(const BusstopPlatform &) const;

    struct Data {
        QString name;
        BusstopPlatformFlags flags;
    };

    Data data() const;
    void setData(const Data &);

    QString name() const;
    void setName(const QString &);

    BusstopPlatformFlags flags() const;
    void setFlags(const BusstopPlatformFlags &);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    BusstopPlatform::Data _data;
};

#endif // BUSSTOPPLATFORM_H
