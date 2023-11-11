#ifndef WDGUNDOVIEW_H
#define WDGUNDOVIEW_H

#include <QWidget>
#include <QUndoStack>

namespace Ui {
class WdgUndoView;
}

class WdgUndoView : public QWidget
{
    Q_OBJECT

public:
    explicit WdgUndoView(QWidget *parent = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgUndoView();

private:
    Ui::WdgUndoView *ui;

    QUndoStack *undoStack;
};

#endif // WDGUNDOVIEW_H
