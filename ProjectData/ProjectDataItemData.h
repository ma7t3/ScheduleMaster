#ifndef PROJECTDATAITEMDATA_H
#define PROJECTDATAITEMDATA_H

#include "ProjectDataItemContainer.h"

#include <QList>

#include <QDebug>

template <typename DerivedType>
struct ProjectDataItemData {
    ProjectDataItemData() {};

    void setParentOwnsItemsMembers(const QList<ProjectDataItemContainer *> &list) {
        _parentOwnsItemsMembers = list;
        for(ProjectDataItemContainer *item : list)
            item->setParentOwnsItems(true);
    }

    DerivedType clone() const {
        DerivedType copy = static_cast<const DerivedType &>(*this);
        copy.cloneContainerItems();
        return copy;
    }

protected:
    void cloneContainerItems() {
        for(ProjectDataItemContainer *container : _parentOwnsItemsMembers)
            container->cloneItems();
        }
    }

private:
    QList<ProjectDataItemContainer *> _parentOwnsItemsMembers;
};

#endif // PROJECTDATAITEMDATA_H
