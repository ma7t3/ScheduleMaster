#ifndef TIMEPROFILEEDITOR_H
#define TIMEPROFILEEDITOR_H

#include "ProjectData\route.h"
#include "ProjectData\timeProfile.h"

#include <QDialog>

namespace Ui {
class TimeProfileEditor;
}

class TimeProfileEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TimeProfileEditor(QWidget *parent = nullptr, bool = false, QString = "unnamed", float = 0, Route* = nullptr,  QList<TimeProfileItem *> = {}, QList<Route *> = {});
    ~TimeProfileEditor();

    void setName(QString);
    void setDuration(float);
    void setRoute(Route *);
    void setTimeProfileItemList(QList<TimeProfileItem *>);

    QString name();
    float getDuration();
    Route * getRoute();
    QList<TimeProfileItem *> getTimeProfileItemList();

    QList<Route *> matchingRoutes;

public slots:
    void refreshTable(int);
    void executeCopy();

private:
    Ui::TimeProfileEditor *ui;

    Route *route;
    QList<TimeProfileItem *> itemList;

    TimeProfileItem * findItemForBusstop(QString id);
};

#endif // TIMEPROFILEEDITOR_H
