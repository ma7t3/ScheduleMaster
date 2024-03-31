#ifndef WDGFOOTNOTES_H
#define WDGFOOTNOTES_H

#include <QDialog>

#include <QUndoStack>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgFootnotes;
}

class WdgFootnotes : public QWidget {
    Q_OBJECT

public:
    explicit WdgFootnotes(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgFootnotes();

public slots:
    void refreshFootnotes();

private slots:
    void actionNew();
    void actionEdit();
    void actionDuplicate();
    void actionDelete();

    void refreshUI();

    void on_tableWidget_itemSelectionChanged();

signals:
    void refreshRequested();

private:
    Ui::WdgFootnotes *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;

    QList<Footnote *> _footnotesReference;
};

#endif // WDGFOOTNOTES_H
