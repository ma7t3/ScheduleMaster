#ifndef WDGLINES_H
#define WDGLINES_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgLines;
}

class WdgLines : public QWidget
{
    Q_OBJECT

public:
    explicit WdgLines(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgLines();

public slots:
    void actionNew();
    void actionEdit();
    void actionDelete();
    void actionExportList();
    void actionExportListAndRoutes();

    void setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDelete);

    void refreshUI();

    void refresh();

    Line *currentLine();

private slots:
    void on_twLines_itemSelectionChanged();

    void on_twLines_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

signals:
    void currentLineChanged(Line *);

private:
    Ui::WdgLines *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    Line *_currentLine = nullptr;
    QList<Line *> tableReference;
    bool refreshing = false;

    QAction *_actionNew = new QAction;
    QAction *_actionEdit = new QAction;
    QAction *_actionDelete = new QAction;
    QAction *_actionExportList = new QAction;
    QAction *_actionExportListAndRoutes = new QAction;
};

#endif // WDGLINES_H
