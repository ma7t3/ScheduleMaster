#ifndef DOCKLINES_H
#define DOCKLINES_H

#include <QWidget>

#include "DockAbstract.h"
#include "ProjectDataModels/LineTableModel.h"
#include "ProjectDataModels/LineTableProxyModel.h"
#include "Widgets/WdgTableColumnVisibilitySelector.h"

class QSortFilterProxyModel;
class QAction;

namespace Ui {
class DockLines;
}

class DockLines : public DockAbstract {
    Q_OBJECT

public:
    explicit DockLines(QWidget *parent = nullptr);
    ~DockLines();

    Line *currentLine() const;
    PDISet<Line> selectedLines() const;

protected slots:
    void onLineNew();
    void onLineEdit();
    void onLineDelete();

    void onSelectionChanged();
    void onRowsAdded(const QList<QPersistentModelIndex> &indexes);

signals:
    void currentLineChanged(Line *);
    void selectedLinesChaned(PDISet<Line>);

private:
    Ui::DockLines *ui;
    QAction *_newAction, *_editAction, *_deleteAction, *_searchAction;
    LineTableModel *_model;
    LineTableProxyModel *_proxyModel;

    WdgTableColumnVisibilitySelector *_columnVisibilitySelector;

    ProjectData *_projectData;
    Line *_currentLine;
};

#endif // DOCKLINES_H
