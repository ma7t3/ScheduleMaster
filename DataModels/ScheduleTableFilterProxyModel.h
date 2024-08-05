#ifndef SCHEDULETABLEFILTERPROXYMODEL_H
#define SCHEDULETABLEFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel >
#include <QWidget>

class ScheduleTableFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    ScheduleTableFilterProxyModel(QObject *parent);

    void setBusstopNameFilter(const QString &);
    void setOnlyImportantBusstop(const bool &);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString _nameFilter;
    bool _onlyImportant;
};

#endif // SCHEDULETABLEFILTERPROXYMODEL_H
