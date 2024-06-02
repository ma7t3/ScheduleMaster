#ifndef TOUREDITOR_H
#define TOUREDITOR_H

#include <QDialog>

#include "ProjectData/weekdays.h"
#include "ProjectData/daytype.h"

namespace Ui {
class TourEditor;
}

class TourEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TourEditor(QWidget *parent = nullptr, bool = false, QString = "", WeekDays = WeekDays(), QList<DayType *> dayTypes = {});
    ~TourEditor();

    void setName(QString);
    void setWeekDays(WeekDays);

    QString name();
    WeekDays weekDays();

private slots:
    void on_cbDayTypes_activated(int index);
    void refreshDayTypeSelector();

private:
    Ui::TourEditor *ui;

    QList<DayType *> _dayTypes;
};

#endif // TOUREDITOR_H
