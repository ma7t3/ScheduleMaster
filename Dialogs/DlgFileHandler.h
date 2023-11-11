#ifndef DLGFILEHANDLER_H
#define DLGFILEHANDLER_H

#include "ProjectData/projectdata.h"

#include <QDialog>

namespace Ui {
class DlgFileHandler;
}

class DlgFileHandler : public QDialog
{
    Q_OBJECT

public:
    explicit DlgFileHandler(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~DlgFileHandler();

    bool readFromFile(QString filePath);
    bool saveToFile(QString filePath);


private:
    Ui::DlgFileHandler *ui;

    ProjectData *projectData;

    const QString fileHeader = "Schedule Master File\r\n====================\r\n";

    void loadProjectSettings(QJsonObject);
    void loadDayTypes(QJsonArray);
    void loadDirections(Line *l, QJsonArray jLines);
    void loadBusstops(QJsonArray);
    void loadLines(QJsonArray);
    void loadRoutes(QJsonArray, Line *);
    void loadProfiles(QJsonArray, Route *);
    void loadTrip(QJsonObject, Line *);
    void loadTours(QJsonArray);

    QJsonObject projectSettingsToJson(ProjectSettings *);
    QJsonObject dayTypeToJson(DayType *);
    QJsonObject routeToJson(Route *);
    QJsonObject timeProfileToJson(TimeProfile *);
    QJsonObject timeProfileItemToJson(TimeProfileItem *);
    QJsonObject tripToJson(Trip *);
    QJsonObject tourToJson(Tour *);
    QJsonObject directionToJson(LineDirection *ld);
};

#endif // DLGFILEHANDLER_H
