#ifndef WDGROUTES_H
#define WDGROUTES_H


#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgRoutes;
}

class WdgRoutes : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRoutes(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgRoutes();

public slots:
    void actionNew();
    void actionEdit();
    void actionDuplicate();
    void actionDelete();
    void actionExportProfiles();

    void setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDuplicate, QAction *actionDelete);

    void refreshUI();

    void setCurrentLine(Line *);

    void refreshRouteTable();

    Route *currentRoute();

    QAction *menubarActionNew();
    QAction *menubarActionEdit();
    QAction *menubarActionDuplicate();
    QAction *menubarActionDelete();
    QAction *menubarActionExportListCurrent();
    QAction *menubarActionExportListAll();

private slots:
    void on_twRoutes_itemSelectionChanged();

    void omsiExport();

signals:
    void currentRouteChanged(Route *);

private:
    Ui::WdgRoutes *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    Line *_currentLine = nullptr;
    Route *_currentRoute = nullptr;
    QList<Route *> tableReference;
    bool refreshing = false;

    QAction *_actionNew = new QAction("Route");
    QAction *_actionEdit = new QAction("Route");
    QAction *_actionDuplicate = new QAction("Route");
    QAction *_actionDelete = new QAction("Route");
    QAction *_actionExportListCurrent = new QAction("Current route list");
    QAction *_actionExportListAll = new QAction("All route list");
};

#endif // WDGROUTES_H
