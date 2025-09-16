#ifndef PROJECTDATAITEMSIGNALS_H
#define PROJECTDATAITEMSIGNALS_H

#include <QObject>

/**
 * @brief The ProjectDataItemSignals class provides the signals for the ProjectDataItem class.
 *
 * This is because ProjectDataItem is a template class but Qt-Signals and Slots can't be defined inside template classes,
 * so it needs to inherit it's signals from this class.
 */

class ProjectDataItemSignals : public QObject {
    Q_OBJECT
public:

    /// Construcs a new ProjectDataItemSignals object.
    explicit ProjectDataItemSignals(QObject *parent) : QObject(parent) {}

    /**
     * @brief Finds the first parent of type T of the QObject.
     * @return The parent that is of type T or nullptr if there is no such parent
     */
    template<typename T>
    T *findParent() const {
        QObject *obj = parent();
        while(obj) {
            if(T *casted = qobject_cast<T *>(obj))
                return casted;

            obj = obj->parent();
        }
        return nullptr;
    }

signals:
    /**
     * @brief This signal is emited whenever the data is changed.
     *
     * This can be just one data property to be changed or the entire data object being replaced.
     */
    void changed();
};

#endif // PROJECTDATAITEMSIGNALS_H
