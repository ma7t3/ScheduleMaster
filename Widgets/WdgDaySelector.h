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

    void setSmallMode(const bool &b);
    void setTristate(const bool &b);

    void setWeekDays(const WeekDays &);
    void setCareWeekDays(const WeekDays &);
    WeekDays weekDays() const;
    WeekDays careWeekDays() const;

private slots:
    void emitChangedSignal();

signals:
    void weekDaysChanged();

private:

    Ui::WdgDaySelector *ui;
};

#endif // WDGDAYSELECTOR_H
