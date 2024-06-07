#ifndef PROJECTDATAMODELSIGNALS_H
#define PROJECTDATAMODELSIGNALS_H

#include <QAbstractTableModel>

class ProjectDataModelSignals : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProjectDataModelSignals(QObject *parent) : QAbstractTableModel(parent) {}

signals:
    void updateFinished();
};

#endif // PROJECTDATAMODELSIGNALS_H
