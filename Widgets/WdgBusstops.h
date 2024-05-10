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
    explicit WdgBusstops(QWidget *parent);
    ~WdgBusstops();

    QList<QAction *> actions();

public slots:
    void refreshUI();
    void refreshBusstopList();
    Busstop *currentBusstop();

private slots:
    void actionNew();
    void actionEdit();
    void actionDelete();

    void on_twBusstops_itemSelectionChanged();

signals:
    void currentBusstopChanged(Busstop *);
    void refreshRequested();

private:
    Ui::WdgBusstops *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;
    Busstop *_currentBusstop = nullptr;
    QList<Busstop *> _tableReference;

    bool _refreshing = false;

    QAction *_actionNew = nullptr;
    QAction *_actionEdit = nullptr;
    QAction *_actionDelete = nullptr;
    QAction *_actionExportList = nullptr;
};

#endif // WDGBUSSTOPS_H
