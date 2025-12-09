#ifndef CMDSCRIPTING_H
#define CMDSCRIPTING_H

#include "Commands/CmdGeneral.h"

#include "ProjectData/ProjectDataItem.h"

struct CmdScriptOperationItem {
    ProjectDataItem *prodItem, *oldItem, *newItem;
};

class CmdScriptOperation : public Command {
public:
    CmdScriptOperation(QList<ProjectDataItem *> items) :
        Command(QObject::tr("Script operation (%n object(s) affected)", "", items.count())) {
        for(ProjectDataItem *item : items) {
            ProjectDataItem *prodItem = item->originalBasePtr();
            _items.append({prodItem, prodItem->cloneBasePtr(), item});
        }
    }

    void redo() override {
        for(const CmdScriptOperationItem &item : _items)
            item.prodItem->mergeBasePtr(item.newItem);
    }

    void undo() override {
        for(const CmdScriptOperationItem &item : _items)
            item.prodItem->mergeBasePtr(item.oldItem);
    }

private:
    QList<CmdScriptOperationItem> _items;
};

#endif // CMDSCRIPTING_H
