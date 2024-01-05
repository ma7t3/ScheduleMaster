#ifndef WDGDAYSELECTOR_H
#define WDGDAYSELECTOR_H

#include <QWidget>

#include "ProjectData/weekdays.h"

namespace Ui {
class WdgDaySelector;
}

class WdgDaySelector : public QWidget
{
    Q_OBJECT

public:
    explicit WdgDaySelector(QWidget *parent = nullptr);
    ~WdgDaySelector();

    void setSmallMode(bool b);

    void setWeekDays(WeekDays);
    WeekDays weekDays();

private slots:
    void emitChangedSignal();

signals:
    void weekDaysChanged();

private:

    Ui::WdgDaySelector *ui;
};

#endif // WDGDAYSELECTOR_H
