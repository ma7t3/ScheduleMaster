#ifndef PROJECTDATAITEMDATA_H
#define PROJECTDATAITEMDATA_H

#include "ProjectDataItemContainer.h"

#include <QList>

/**
 * @brief The ProjectDataItemData class is the base class for all data classes of ProjectDataItem's Data sub-obejects.
 *
 * Therefore, it provides general functionality that is necessarry for every kind of data.
 *
 * **Note:** This is a template class. It takes its own derived type as a template typename.
 * For example, BusstopData subclasses ProjectDataItemData<BusstopData>.
 */

template <typename DerivedType>
struct ProjectDataItemData {

    /// Constructs a new, etmpy data object.
    ProjectDataItemData() {};

    /**
     * @brief Initializes all containers which contain items owned by the parent object.
     *
     * You should call this function from the constructor before the containers is being populated!
     */
    void initParentOwnsItemMembers() {
        QList<ProjectDataItemContainer *> list = parentOwnsItemsMembersList();
        for(ProjectDataItemContainer *item : list)
            item->setParentOwnsItems(true);
    }

    /**
     * @brief Returns the internal comment of the ProjectDataItemData.
     * @return The internal comment
     */
    QString comment() const { return _comment; }

    /**
     * @brief Sets the internal comment of the ProjectDataItemData to newComment.
     * @param newComment The comment to set
     */
    void setComment(const QString &newComment) { _comment = newComment; }

    /**
     * @brief Retruns a cloned version of the data.
     *
     * This will create deep copies of all sub-items in containers.
     * @return The cloned data
     */
    DerivedType clone(QObject *parent) const {
        DerivedType copy = static_cast<const DerivedType &>(*this);
        copy.cloneContainerItems(parent);
        return copy;
    }

    void merge(DerivedType &mergeData, QObject *parent) {
        // Speichere die aktuellen Container-Referenzen
        QList<ProjectDataItemContainer *> selfList = parentOwnsItemsMembersList();
        QList<ProjectDataItemContainer *> selfCopyList;

        for(ProjectDataItemContainer *container : selfList)
            selfCopyList.append(container->clone());

        QList<ProjectDataItemContainer *> otherCopyList = mergeData.parentOwnsItemsMembersList();

        // Überschreiben der primitiven Daten
        *static_cast<DerivedType *>(this) = mergeData;

        // Merge der Container-Variablen (analog zu originalem Code)
        for (int i = 0; i < selfList.size(); i++) {
            selfCopyList[i]->mergeItems(otherCopyList[i], parent);
        }

        selfList = parentOwnsItemsMembersList();

        // Wiederherstellen der ursprünglichen Container-Daten
        for (int i = 0; i < selfList.size(); i++) {
            ProjectDataItemContainer *selfContainer = selfList[i];
            ProjectDataItemContainer *otherContainer = selfCopyList[i];
            selfContainer->replaceItems(otherContainer);
        }

        // Speicher der Kopien freigeben
        for (auto *copy : std::as_const(selfCopyList))
            delete copy;
    }

protected:
    /**
     * @brief Clones all items in the containers that are owned by the parent object and replaces the content of the container with the copies.
     */
    void cloneContainerItems(QObject *parent) {
        for(ProjectDataItemContainer *container : parentOwnsItemsMembersList())
            container->cloneItems(parent);
    }

    /**
     * @brief The parentOwnsItemsMembersList function is a pure virtual function that has to be implemented by the derived class.
     *
     * It should return a QList of ProjectDataItemContainer pointers. This list should contain pointers to all member containers holding items that are owned by the parent object.
     *
     * This is important, so the initParentOwnsItemMembers() function can set up the containers properly.
     * @return The list of member pointers.
     */
    virtual QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() = 0;

    /**
     * @brief The _comment member variable is a general string that can be used to store an internal comment for every item in the ProjectData class.
     */
    QString _comment;
};

#endif // PROJECTDATAITEMDATA_H
