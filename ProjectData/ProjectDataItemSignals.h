#ifndef PROJECTDATAITEMSIGNALS_H
#define PROJECTDATAITEMSIGNALS_H

#include <QObject>

class ProjectDataItemSignals : public QObject {
    Q_OBJECT
public:
    explicit ProjectDataItemSignals(QObject *parent) :
        QObject(parent) {
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
