#ifndef PROJECTDATAITEMDATA_H
#define PROJECTDATAITEMDATA_H

#include "ProjectDataItemContainer.h"

#include <QList>

struct ProjectDataItemData {
    ProjectDataItemData() {};

    void setParentOwnsItemsMembers(const QList<ProjectDataItemContainer *> &list) {
        _parentOwnsItemsMembers = list;
        for(ProjectDataItemContainer *item : list)
            item->setParentOwnsItems(true);
    }

    QList<ProjectDataItemContainer *> _parentOwnsItemsMembers;
};

#endif // PROJECTDATAITEMDATA_H
