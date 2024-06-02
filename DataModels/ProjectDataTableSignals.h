#ifndef PROJECTDATATABLESIGNALS_H
#define PROJECTDATATABLESIGNALS_H

#include <QAbstractTableModel>

class ProjectDataTableSignals : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProjectDataTableSignals(QObject *parent) : QAbstractTableModel(parent) {}

signals:
    void updateFinished();
};

#endif // PROJECTDATATABLESIGNALS_H
