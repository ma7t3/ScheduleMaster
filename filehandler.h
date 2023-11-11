#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "ProjectData/projectdata.h"

#include <QDialog>

namespace Ui {
class FileHandler;
}

class FileHandler : public QObject
{
    Q_OBJECT

public:
    explicit FileHandler(ProjectData *projectData);

    bool readFromFile(QString filePath);
    bool saveToFile(QString filePath);

private:
    ProjectData *d;

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

#endif // FILEHANDLER_H
