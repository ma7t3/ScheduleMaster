#ifndef PROJECTDATAITEMDATA_H
#define PROJECTDATAITEMDATA_H

#include "ProjectDataItemContainer.h"

#include <QList>

struct ProjectDataItemData {
    ProjectDataItemData() {};

    void updateParentOwnsItemsMembers() {
        for(ProjectDataItemContainer *item : defineParentOwnsItemsMembers())
            item->setParentOwnsItems(true);
    }

    void setIsClone(const bool &isClone) {
        for(ProjectDataItemContainer *item : defineParentOwnsItemsMembers())
            item->setIsInClone(isClone);
    }

protected:
    virtual QList<ProjectDataItemContainer *> defineParentOwnsItemsMembers() = 0;
};

#endif // PROJECTDATAITEMDATA_H
