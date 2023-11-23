#ifndef WDGPUBLISHEDLINES_H
#define WDGPUBLISHEDLINES_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgPublishedLines;
}

class WdgPublishedLines : public QWidget
{
    Q_OBJECT

public:
    explicit WdgPublishedLines(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgPublishedLines();

    PublishedLine *currentLine() const;

public slots:
    void setCurrentLine(PublishedLine *newCurrentLine);

    void actionNew();
    void actionEdit();
    void actionDelete();

    void actionDirectionNew();
    void actionDirectionEdit();
    void actionDirectionDelete();

    void actionRoutesChange();

    void actionBusstopAdd();
    void actionBusstopsAddAll();

    void actionBusstopRemove();
    void actionBusstopRemoveAll();

    void actionBusstopUp();
    void actionBusstopDown();

    void actionEditBusstop();

    void refreshLineList();
    void refreshCurrentLine();
    void refreshCurrentLineDirection();

    void refreshDayTypes();
    //void refreshDayCheckBoxes();

    void refreshRouteList();
    void refreshRouteCheckBoxes();
    void refreshRouteCheckBoxRelations(QTreeWidgetItem *);

    void refreshAllBusstops();
    void refreshBusstopList();

signals:
    void currentLineChanged(PublishedLine *);

private slots:
    void on_lwLines_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbFilePathBrowse_clicked();

    void on_lwDirections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_lwBusstops_itemClicked(QListWidgetItem *item);

    void on_pbBusstopSearchAndReplace_clicked();

    void on_lwDayTypes_itemChanged(QListWidgetItem *item);

private:
    Ui::WdgPublishedLines *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    QList<PublishedLine *> m_listReference;
    QList<PublishedLineDirection *> m_directionsListReference;
    PublishedLine *m_currentLine;
    PublishedLineDirection *m_currentLineDirection;
    PublishedBusstop *m_currentBusstop;
    QList<DayType *> m_dayTypesReference;
    QList<QList<QList<Route *>>> m_routesReference;
    QList<QList<LineDirection *>> m_routesDirectionsReference;
    QList<Line *> m_routesLinesReference;

    QList<Busstop *> m_allBusstopsReference;

    bool refreshing = false;
    bool refreshingCurrentLine = false;
    bool refreshingRouteCheckBoxes = false;
};

#endif // WDGPUBLISHEDLINES_H
