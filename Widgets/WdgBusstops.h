#ifndef WDGBUSSTOPS_H
#define WDGBUSSTOPS_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>
#include <QItemSelectionModel>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgBusstops;
}

class WdgBusstops : public QWidget
{
    Q_OBJECT

public:
    explicit WdgBusstops(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgBusstops();

public slots:
    void actionNew();
    void actionEdit();
    void actionDelete();
    void actionSearch();
    void actionExportList();

    void setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDelete);

    void refreshUI();
    void refresh();
    Busstop * currentBusstop();

private slots:   
    void on_twBusstops_itemSelectionChanged();

signals:
    void currentBusstopChanged(Busstop *);
    void refreshRequested();

private:
    Ui::WdgBusstops *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;
    Busstop *_currentBusstop = nullptr;
    QList<Busstop *> tableReference;

    bool refreshing = false;

    QAction *_actionNew = nullptr;
    QAction *_actionEdit = nullptr;
    QAction *_actionDelete = nullptr;
    QAction *_actionExportList = nullptr;
};

#endif // WDGBUSSTOPS_H
