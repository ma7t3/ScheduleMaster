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

    void refreshLineTable();    

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
    Line *m_currentLine = nullptr;
    QList<Line *> tableRefernce;
    bool refreshing = false;

    QAction *m_actionNew = new QAction("Line");
    QAction *m_actionEdit = new QAction("Line");
    QAction *m_actionDelete = new QAction("Line");
    QAction *m_actionExportList = new QAction("Line list");
    QAction *m_actionExportListAndRoutes = new QAction("Line list (with routes)");
};

#endif // WDGLINES_H
