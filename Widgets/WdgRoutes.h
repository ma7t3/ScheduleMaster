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
    Line *m_currentLine = nullptr;
    Route *m_currentRoute = nullptr;
    QList<Route *> tableReference;
    bool refreshing = false;

    QAction *m_actionNew = new QAction("Route");
    QAction *m_actionEdit = new QAction("Route");
    QAction *m_actionDuplicate = new QAction("Route");
    QAction *m_actionDelete = new QAction("Route");
    QAction *m_actionExportListCurrent = new QAction("Current route list");
    QAction *m_actionExportListAll = new QAction("All route list");
};

#endif // WDGROUTES_H
