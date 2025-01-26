#ifndef PROJECTDATAITEMCONTAINER_H
#define PROJECTDATAITEMCONTAINER_H

class ProjectDataItemContainer {
public:
    ProjectDataItemContainer() : _parentOwnsItems(false), _isInClone(false) {}

    bool parentOwnsItems() const {
        return _parentOwnsItems;
    }

    void setParentOwnsItems(const bool &parentOwnsItems) {
        _parentOwnsItems = parentOwnsItems;
    }

    bool isInClone() const {
        return _isInClone;
    }

    void setIsInClone(const bool &isInClone) {
        _isInClone = isInClone;
    }

    bool shouldUpdateInUse() const {
        return _parentOwnsItems && !_isInClone;
    }

protected:
    bool _parentOwnsItems;
    bool _isInClone;
};

#endif // PROJECTDATAITEMCONTAINER_H
