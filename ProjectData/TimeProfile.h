#ifndef TIMEPROFILE_H
#define TIMEPROFILE_H

#include "ProjectData/TimeProfileItem.h"

struct TimeProfileData : ProjectDataItemData<TimeProfileData> {
    TimeProfileData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override { return {&items}; }

    QString name;

    PDISet<TimeProfileItem> items;
};

class TimeProfile : public ProjectDataItem<TimeProfile, TimeProfileData> {
    friend class ProjectDataItem<TimeProfile, TimeProfileData>;

    Q_OBJECT
public:
    explicit TimeProfile(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);
    explicit TimeProfile(QObject *parent, const QJsonObject &jsonObject);

    QString name() const;
    void setName(const QString &name);

    float duration();

    PDISet<TimeProfileItem> items();
    TimeProfileItem *item(RouteBusstopItem *busstop);

    QJsonObject toJson() const override;

protected:
    TimeProfileItem *createItem(const QJsonObject &jsonObject);

signals:
    void itemChanged(TimeProfileItem *);

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // TIMEPROFILE_H
