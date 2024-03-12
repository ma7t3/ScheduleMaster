#ifndef DLGPROJECTSETTINGS_H
#define DLGPROJECTSETTINGS_H

#include <QDialog>
#include <QFile>

#include <QListWidgetItem>

#include "ProjectData/daytype.h"

namespace Ui {
class DlgProjectSettings;
}

class DlgProjectSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DlgProjectSettings(QWidget *parent = nullptr);
    ~DlgProjectSettings();

    void setDisplayName(QString);
    void setShortName(QString);
    void setNames(QString, QString);
    void setIcon(QString);
    void setDayTypes(QList<DayType>);

    QString displayName();
    QString shortName();
    QString icon();
    QList<DayType *> dayTypes();

private slots:
    void actionSelectIcon();
    void reloadIconPreview();
    void refreshDayTypesTable();
    void refreshTayTypesDetails();

    void on_lwDays_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_leDayTypesName_textEdited(const QString &arg1);

    void on_pbDayNew_clicked();

    void on_pbDaysDelete_clicked();

    void on_pbDaysUp_clicked();

    void on_pbDaysDown_clicked();

    void on_daySelector_weekDaysChanged();

private:
    Ui::DlgProjectSettings *ui;

    QList<DayType *> tableReference;
    DayType *_currentDayType = nullptr;
    bool refreshingDayTypes = false;
    QFile _icon;
};

#endif // DLGPROJECTSETTINGS_H
