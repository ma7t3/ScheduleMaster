#ifndef PROJECTDATAITEMLIST_H
#define PROJECTDATAITEMLIST_H

#include <QList>
#include <QUuid>

template <typename T>
class ProjectDataItemList : public QList<T> {
public:
    ProjectDataItemList() : QList<T>() {};

    bool contains(const QUuid &id) const {
        auto it = std::find_if(this->begin(), this->end(),
                               [id](T current) {
            return current->id() == id;
        });

        return it != this->end();
    }

    T find(const QUuid &id) const {
        auto it = std::find_if(this->begin(), this->end(),
                               [id](T current) {
            return current->id() == id;
        });

        return it != this->end() ? *it : nullptr;
    }

    void append(T item, const bool &updateInUse = false) {
        QList<T>::append(item);
        if(updateInUse)
            item->setInUse(true);
    }

    void insert(const int &index, T item, const bool &updateInUse = false) {
        QList<T>::insert(index, item);
        if(updateInUse)
            item->setInUse(true);
    }

    template <typename Filter>
    ProjectDataItemList<T> filter(Filter filter) const {
        ProjectDataItemList<T> result;
        for(T current : *this) {
            if(filter(current))
                result.append(current);
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
        QList<T>::removeAll(item);
        if(updateInUse)
            item->setInUse(false);
    }

    void remove(const QUuid &id, const bool &updateInUse = false) {
        int i = 0;
        while(i < this->count()) {
            if(this->value(i)->id() == id)
                removeAt(i, updateInUse);
            else
                i++;
        }
    }

    void removeAt(const int &index, const bool &updateInUse = false) {
        T current = this->value(index);
        QList<T>::remove(index);
        if(updateInUse)
            current->setInUse(false);
    }

    void clear(const bool &updateInUse = false) {
        if(updateInUse)
            for(T current : *this)
                current->setInUse(false);

        QList<T>::clear();
    }
};

template <typename T>
using PDIList = ProjectDataItemList<T>;

#endif // PROJECTDATAITEMLIST_H
