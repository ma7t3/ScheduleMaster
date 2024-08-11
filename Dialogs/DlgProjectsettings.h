#ifndef DLGPROJECTSETTINGS_H
#define DLGPROJECTSETTINGS_H

#include <QDialog>
#include <QFile>

#include <QListWidgetItem>

#include "DataModels/DayTypeListModel.h"
#include "projectData/projectsettings.h"

namespace Ui {
class DlgProjectSettings;
}

class DlgProjectSettings : public QDialog {
    Q_OBJECT

public:
    explicit DlgProjectSettings(QWidget *parent, ProjectSettings *);
    ~DlgProjectSettings();

    ProjectSettings projectSettings() const;

private slots:
    void loadProjectSettings();

    void actionSelectIcon();
    void reloadIconPreview();
    void refreshTayTypesDetails();


    void on_leDayTypesName_textEdited(const QString &arg1);
    void on_pbDayNew_clicked();
    void on_pbDaysDelete_clicked();
    void on_pbDaysUp_clicked();
    void on_pbDaysDown_clicked();
    void on_daySelector_weekDaysChanged();

    void onSelectionChanged(QModelIndex current, QModelIndex previous);
    void onDayTypeInserted(QModelIndex parent, int first, int last);

private:
    Ui::DlgProjectSettings *ui;

    ProjectSettings *_projectSettingsPtr;
    ProjectSettings _projectSettings;
    DayTypeListModel *_model;
    QFile _icon;
};

#endif // DLGPROJECTSETTINGS_H
