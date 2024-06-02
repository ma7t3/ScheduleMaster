#ifndef WDGBUSSTOPS_H
#define WDGBUSSTOPS_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>
#include <QItemSelectionModel>

#include "ProjectData/projectdata.h"
#include "DataModels/BusstopTableModel.h"

namespace Ui {
class WdgBusstops;
}

class WdgBusstops : public QWidget
{
    Q_OBJECT

public:
    explicit WdgBusstops(QWidget *parent);
    ~WdgBusstops();

    QList<QAction *> actions();

public slots:
    void refreshUI();
    Busstop *currentBusstop();

private slots:
    void actionNew();
    void actionEdit();
    void actionDelete();

    void onSelectionChanged();

signals:
    void currentBusstopChanged(Busstop *);
    void refreshRequested();
    void busstopScheduleRequested(Busstop *, QList<Route *>, DayType *);

private:
    Ui::WdgBusstops *ui;

    ProjectData *projectData;
    BusstopTableModel *_model;
    Busstop *_currentBusstop;

    QAction *_actionNew;
    QAction *_actionEdit;
    QAction *_actionDelete;
    QAction *_actionViewSchedule;
};

#endif // WDGBUSSTOPS_H
