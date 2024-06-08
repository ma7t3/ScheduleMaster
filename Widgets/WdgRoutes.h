#ifndef WDGROUTES_H
#define WDGROUTES_H


#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>

#include "ProjectData/projectdata.h"
#include "DataModels/RouteTableModel.h"

namespace Ui {
class WdgRoutes;
}

class WdgRoutes : public QWidget
{
    Q_OBJECT

public:
    explicit WdgRoutes(QWidget *parent = nullptr);
    ~WdgRoutes();

    QList<QAction *> actions();

public slots:
    void refreshUI();
    void setCurrentLine(Line *);

public:
    Route *currentRoute() const;

private slots:
    void actionNew();
    void actionEdit();
    void actionDuplicate();
    void actionDelete();

    void omsiExport();

    void onSelectionChanged();

signals:
    void currentRouteChanged(Route *);
    void refreshRequested();

private:
    Ui::WdgRoutes *ui;

    ProjectData *projectData;
    RouteTableModel *_model;
    QSortFilterProxyModel *_proxyModel;
    Line *_currentLine;
    Route *_currentRoute;

    QAction *_actionNew;
    QAction *_actionEdit;
    QAction *_actionDuplicate;
    QAction *_actionDelete;
};

#endif // WDGROUTES_H
