#ifndef TIMEPROFILEITEM_H
#define TIMEPROFILEITEM_H

#include "ProjectData/RouteBusstopItem.h"

struct TimeProfileItemData : ProjectDataItemData<TimeProfileItemData> {
    TimeProfileItemData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {};}

    QUuid busstopID;
    float departure = 0, arrival = -1;
};

class TimeProfileItem : public ProjectDataItem<TimeProfileItem, TimeProfileItemData> {
    friend class TimeProfile;
    friend class ProjectDataItem<TimeProfileItem, TimeProfileItemData>;
    Q_OBJECT

private:
    explicit TimeProfileItem(QObject *parent, RouteBusstopItem *busstop, const QUuid &id = QUuid(), const bool &isClone = false);
    explicit TimeProfileItem(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);
    explicit TimeProfileItem(QObject *parent, const QJsonObject &jsonObject);

public:
    QUuid routeBusstopItemID() const;

    bool hasArrival() const;
    float arrival() const;
    void setArrival(const float &value);
    void removeArrival();

    float departure() const;
    void setDeparture(const float &value);

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // TIMEPROFILEITEM_H
