#include "WdgScheduleTable.h"

#include <QScrollBar>

WdgScheduleTable::WdgScheduleTable(QWidget *parent) :
    QTableWidget(parent),
    _shiftPressed(false) {
}

void WdgScheduleTable::wheelEvent(QWheelEvent *e) {
    const int delta = e->angleDelta().y();
    if(_shiftPressed)
        horizontalScroll(delta);
    else
        verticalScroll(delta);

    e->accept();
}

void WdgScheduleTable::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Shift)
        _shiftPressed = true;
}

void WdgScheduleTable::keyReleaseEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Shift)
        _shiftPressed = false;
}

void WdgScheduleTable::horizontalScroll(const int &delta) {
    const int currentValue = horizontalScrollBar()->value();
    horizontalScrollBar()->setValue(currentValue - delta);
}

void WdgScheduleTable::verticalScroll(const int &delta) {
    const int currentValue = verticalScrollBar()->value();
    verticalScrollBar()->setValue(currentValue - delta);
}
