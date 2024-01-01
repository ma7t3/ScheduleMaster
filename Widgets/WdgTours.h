#ifndef WDGTOURS_H
#define WDGTOURS_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidget>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgTours;
}

class WdgTours : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTours(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgTours();

public slots:
    void actionTourNew();
    void actionTourEdit();
    void actionTourDuplicate();
    void actionTourDelete();
    void actionExport();

    void refreshTourList();

    Tour * currentTour();

private slots:
    void on_twTours_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

signals:
    void currentTourChanged(Tour *);

private:
    Ui::WdgTours *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    QList<Tour *> tableReference;
    Tour *_currentTour = nullptr;

    //QString currentTourId;
};

#endif // WDGTOURS_H
