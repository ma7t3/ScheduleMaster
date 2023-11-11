#ifndef TOUREDITOR_H
#define TOUREDITOR_H

#include <QDialog>

#include "ProjectData\weekdays.h"

namespace Ui {
class TourEditor;
}

class TourEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TourEditor(QWidget *parent = nullptr, bool = false, QString = "", WeekDays = WeekDays());
    ~TourEditor();

    void setName(QString);
    void setWeekDays(WeekDays);

    QString name();
    WeekDays getWeekDays();

private slots:
    void on_pbMonFri_clicked();

    void on_pbSat_clicked();

    void on_pbSun_clicked();

private:
    Ui::TourEditor *ui;
};

#endif // TOUREDITOR_H
