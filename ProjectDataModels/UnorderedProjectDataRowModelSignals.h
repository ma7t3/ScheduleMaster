#ifndef UNORDEREDPROJECTDATAROWMODELSIGNALS_H
#define UNORDEREDPROJECTDATAROWMODELSIGNALS_H

#include <QAbstractTableModel>
#include <QObject>

class UnorderedProjectDataRowModelSignals : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit UnorderedProjectDataRowModelSignals(QObject *parent = nullptr) :
        QAbstractTableModel(parent) {}

signals:
    void multipleRowsInserted(QList<QPersistentModelIndex> indexes);
};

#endif // UNORDEREDPROJECTDATAROWMODELSIGNALS_H
