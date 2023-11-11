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
    void refreshBusstopTable();
    Busstop * currentBusstop();

private slots:   
    void on_twBusstops_itemSelectionChanged();

signals:
    void currentBusstopChanged(Busstop *);

private:
    Ui::WdgBusstops *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;
    Busstop *m_currentBusstop = nullptr;
    QList<Busstop *> tableReference;

    bool refreshing = false;

    QAction *m_actionNew = nullptr;
    QAction *m_actionEdit = nullptr;
    QAction *m_actionDelete = nullptr;
    QAction *m_actionExportList = nullptr;
};

#endif // WDGBUSSTOPS_H
