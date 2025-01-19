#ifndef WDGSCHEDULETABLE_H
#define WDGSCHEDULETABLE_H

#include <QObject>
#include <QTableWidget>
#include <QWidget>

#include <QScrollEvent>

class WdgScheduleTable : public QTableView {
    Q_OBJECT
public:
    WdgScheduleTable(QWidget *);

protected:
    void wheelEvent(QWheelEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;

    void horizontalScroll(const int &delta);
    void verticalScroll(const int &delta);

private:
    bool _shiftPressed;
};

#endif // WDGSCHEDULETABLE_H
