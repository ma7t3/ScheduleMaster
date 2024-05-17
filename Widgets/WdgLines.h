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
    explicit WdgLines(QWidget *parent);
    ~WdgLines();

    QList<QAction *> actions();

public slots:
    void refreshUI();
    void refreshLineList();
    Line *currentLine();

private slots:
    void actionNew();
    void actionEdit();
    void actionDelete();

    void on_twLines_itemSelectionChanged();
    void on_twLines_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

signals:
    void currentLineChanged(Line *);
    void refreshRequested();

private:
    Ui::WdgLines *ui;

    ProjectData *projectData;
    Line *_currentLine;
    QList<Line *> tableReference;

    bool refreshing;

    QAction *_actionNew;
    QAction *_actionEdit;
    QAction *_actionDelete;
};

#endif // WDGLINES_H
