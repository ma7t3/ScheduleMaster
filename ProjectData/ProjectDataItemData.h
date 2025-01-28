#ifndef PROJECTDATAITEMDATA_H
#define PROJECTDATAITEMDATA_H

#include "ProjectDataItemContainer.h"

#include <QList>

template <typename DerivedType>
struct ProjectDataItemData {
    ProjectDataItemData() {};

    void initParentOwnsItemMembers() {
        QList<ProjectDataItemContainer *> list = parentOwnsItemsMembersList();
        for(ProjectDataItemContainer *item : list)
            item->setParentOwnsItems(true);
    }

    DerivedType clone() const {
        DerivedType ref = static_cast<const DerivedType &>(*this);
        DerivedType copy = static_cast<const DerivedType &>(*this);
        copy.cloneContainerItems();
        return copy;
    }

protected:
    void cloneContainerItems() {
        for(ProjectDataItemContainer *container : parentOwnsItemsMembersList())
            container->cloneItems();
    }

    virtual QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() = 0;
};

#endif // PROJECTDATAITEMDATA_H
