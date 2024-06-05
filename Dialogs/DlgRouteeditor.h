#ifndef ROUTEEDITOR_H
#define ROUTEEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

#include "DataModels/SimpleBusstopListModel.h"

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

    void onSelectionChanged();

private:
    Ui::DlgRouteEditor *ui;

    SimpleBusstopListModel *_model;

    Route _route;
    Route *_routePtr;
    QList<LineDirection *> _directionsReference;
    QList<Route *> _matchingRoutes;
    Busstop *_currentRouteBusstop;
    QList<TimeProfile *> _timeProfilesReference;
};

#endif // ROUTEEDITOR_H
