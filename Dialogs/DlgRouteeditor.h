#ifndef ROUTEEDITOR_H
#define ROUTEEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

#include "ProjectData\projectdata.h"

namespace Ui {
class DlgRouteEditor;
}

class DlgRouteEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRouteEditor(QWidget *parent, Route *, const bool &createMode = false);
    ~DlgRouteEditor();

    void setCreateMode(const bool &);

    Route route() const;
    void setRoute(const Route &);

private slots:
    void refreshAllBusstops();
    void refreshProfiles();
    void refreshRouteBusstops();

    void actionBusstopAdd();
    void actionBusstopRemove();
    void actionBusstopUp();
    void actionBusstopDown();
    void actionBusstopsReverse();
    void actionProfileNew();
    void actionProfileEdit();
    void actionProfileDelete();
    void moveCurrentRouteBusstop(bool);

    void on_lwRouteBusstops_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::DlgRouteEditor *ui;

    Route _route;
    Route *_routePtr;
    QList<LineDirection *> _directionsReference;
    QList<Busstop *> _allBusstops;
    QList<Busstop *> _allBusstopsReference;
    QList<Route *> _matchingRoutes;
    Busstop *_currentRouteBusstop;
    QList<TimeProfile *> _timeProfilesReference;

    //QList<LineDirection *> directions;
};

#endif // ROUTEEDITOR_H
