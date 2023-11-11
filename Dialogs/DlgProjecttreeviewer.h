#ifndef PROJECTTREEVIEWER_H
#define PROJECTTREEVIEWER_H

#include <QDialog>
#include "ProjectData\projectdata.h"
#include <QTreeWidgetItem>

namespace Ui {
class ProjectTreeViewer;
}

class ProjectTreeViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectTreeViewer(QWidget *parent = nullptr, ProjectData * = new ProjectData);
    ~ProjectTreeViewer();

private:
    Ui::ProjectTreeViewer *ui;
    ProjectData *projectData;

    void loadBusstops();
    void loadLines();
    QList<QTreeWidgetItem*> loadRoutes(Line *, QTreeWidgetItem *);
    QList<QTreeWidgetItem*> loadTrips(QList<Trip *>, QTreeWidgetItem *);
    QList<QTreeWidgetItem*> loadTripChilds(Trip *, QTreeWidgetItem *);
};

#endif // PROJECTTREEVIEWER_H
