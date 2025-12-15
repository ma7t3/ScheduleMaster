#ifndef DLGROUTEEDITOR_H
#define DLGROUTEEDITOR_H

#include <QDialog>

#include "ProjectData/Route.h"

class ProjectData;
class Line;
class LineDirectionTableModel;
class RouteBusstopTableModel;
class RouteBusstopTableDelegate;
class BusstopTableModel;
class QSortFilterProxyModel;
class TimeProfileTableModel;

namespace Ui {
class DlgRouteEditor;
}

class DlgRouteEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgRouteEditor(Route *route = nullptr, QWidget *parent = nullptr);
    ~DlgRouteEditor();

    Route *route();

protected slots:
    void onBusstopAdd();
    void onBusstopRemove();
    void onBusstopUp();
    void onBusstopDown();
    void onBusstopsReverse();

    void onProfileNew();
    void onProfileDelete();
    void onProfileUp();
    void onProfileDown();

    void onCurrentTimeProfileChanged();

    void onSomethingChanged();

    void accept() override;
    void reject() override;

private:
    Ui::DlgRouteEditor *ui;

    ProjectData *_projectData;
    Line *_line;
    Route *_route;
    QAction *_actionBusstopAdd, *_actionBusstopRemove, *_actionBusstopUp, *_actionBusstopDown, *_actionBusstopsReverse;
    QAction *_actionProfileNew, *_actionProfileUp, *_actionProfileDown, *_actionProfileDelete;

    LineDirectionTableModel *_directionModel;
    BusstopTableModel *_allBusstopsModel;
    QSortFilterProxyModel *_allBusstopsProxyModel;
    RouteBusstopTableModel *_routeBusstopModel;
    RouteBusstopTableDelegate *_routeBusstopDelegate;
    TimeProfileTableModel *_timeProfileModel;

    bool _changed;
};

#endif // DLGROUTEEDITOR_H
