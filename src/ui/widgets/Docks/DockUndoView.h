#ifndef DOCKUNDOVIEW_H
#define DOCKUNDOVIEW_H

#include "DockAbstract.h"

class QUndoView;
class QUndoStack;

class DockUndoView : public DockAbstract {
    Q_OBJECT

public:
    explicit DockUndoView(QWidget *parent = nullptr);

signals:

private:
    QUndoView *_view;
};

#endif // DOCKUNDOVIEW_H
