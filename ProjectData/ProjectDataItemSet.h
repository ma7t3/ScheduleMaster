#ifndef PROJECTDATAITEMLIST_H
#define PROJECTDATAITEMLIST_H

#include <QHash>
#include <QUuid>

template <typename T>
class ProjectDataItemSet : public QHash<QUuid, T> {
public:
    ProjectDataItemSet() : QHash<QUuid, T>() {};

    bool contains(T item) const {
        return QHash<QUuid, T>::contains(item->id());
    }

    T find(const QUuid &id) const {
        return QHash<QUuid, T>::contains(id) ? QHash<QUuid, T>::value(id) : nullptr;
    }

    void add(T item, const bool &updateInUse = false) {
        QHash<QUuid, T>::insert(item->id(), item);
        if(updateInUse)
            item->setInUse(true);
    }

    template <typename Filter>
    ProjectDataItemSet<T> filter(Filter filter) const {
        ProjectDataItemSet<T> result;
        for(T current : *this) {
            if(filter(current))
                result.add(current);
        }
        return result;
    }

    template <typename Filter>
    T filterOne(Filter filter) const {
        for(T current : *this) {
            if(filter(current))
                return current;
        }
        return nullptr;
    }

    template <typename Filter>
    bool some(Filter filter) const {
        for(T current : *this)
            if(filter(current))
                return true;

        return false;
    }

    template <typename Filter>
    bool every(Filter filter) const {
        for(T current : *this)
            if(!filter(current))
                return false;

        return true;
    }

    void remove(T item, const bool &updateInUse = false) {
        if(updateInUse)
            item->setInUse(false);
        remove(item->id());
    }

    void remove(const QUuid &id, const bool &updateInUse = false) {
        T item = QHash<QUuid, T>::value(id);
        if(item && updateInUse)
            item->setInUse(false);

        QHash<QUuid, T>::remove(id);
    }

    void clear(const bool &updateInUse = false) {
        if(updateInUse)
            for(T current : *this)
                current->setInUse(false);

        QHash<QUuid, T>::clear();
    }
};

template <typename T>
using PDISet = ProjectDataItemSet<T>;

#endif // PROJECTDATAITEMLIST_H
