#ifndef WDGLINES_H
#define WDGLINES_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidgetItem>

#include "DataModels/LineTableModel.h"

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
    Line *currentLine();

private slots:
    void actionNew();
    void actionEdit();
    void actionDelete();

    void onSelectionChanged();

signals:
    void currentLineChanged(Line *);
    void refreshRequested();

private:
    Ui::WdgLines *ui;

    ProjectData *projectData;
    LineTableModel *_model;
    Line *_currentLine;

    QAction *_actionNew;
    QAction *_actionEdit;
    QAction *_actionDelete;
};

#endif // WDGLINES_H
