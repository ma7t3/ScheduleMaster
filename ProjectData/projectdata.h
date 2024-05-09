#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QtCore>
#include <QJsonDocument>
#include <QColor>

#include "projectData/busstop.h"
#include "projectData/line.h"
#include "projectData/tour.h"
#include "projectData/projectsettings.h"
#include "projectData/publications.h"
#include "projectData/footnote.h"

class ProjectData : public QObject {
    Q_OBJECT
public:
    ProjectData();

    ProjectSettings *projectSettings();

    void reset();
    void cleanup();

    void clearLines();

    void setFilePath(QString);
    QString filePath();

    void addBusstop(Busstop *);
    void addLine(Line *);
    void addTour(Tour *);

    int busstopCount();
    Busstop *busstop(QString);
    Busstop *busstopAt(int);
    QList <Busstop*> busstops();
    Busstop *busstopWithName(const QString &name);
    bool busstopWithNameExists(const QString &name);

    int lineCount();
    Line *line(QString);
    Line *lineAt(int);
    QList<Line *> lines();
    Line *lineWithName(const QString &name);
    bool lineWithNameExists(const QString &name);

    Route *route(QString);
    Route *routeWithName(QString);
    Trip *trip(QString);

    int tourCount();
    Tour *tour(QString);
    Tour *tourAt(int);
    QList<Tour *> tours();

    bool removeBusstop(Busstop *);
    bool removeBusstop(QString);

    bool removeLine(Line *);
    bool removeLine(QString);

    bool removeTour(Tour *);
    bool removeTour(QString);

    QList<Footnote *> footnotes() const;
    void setFootnotes(const QList<Footnote *> &newFootnotes);
    void addFootnote(Footnote *);
    void removeFootnote(Footnote *);
    void removeFootnote(const QString &);
    Footnote *footnoteAt(int);
    int footnoteCount() const;
    QList<Footnote *> autoAssignedFootnotesOfTrip(Trip *);


    QList<Route *> matchingRoutes(Route *);

    Line *lineOfRoute(Route *);
    Line *lineOfTrip(Trip *);
    Route *routeOfTimeProfile(TimeProfile *);

    QList<Trip *> tripsOfRoute(Route *);

    QList<Line *> linesAtBusstop(Busstop *b);
    QList<Line *> linesAtBusstop(QString);

    QList<Tour *> toursOfTrip(Trip *);

    QList<Busstop *> combinedRoutes(const QList<Route *> &routes);

    template<typename T>
    static QList<T *> sortItems(QList<T *> list) {
        std::sort(list.begin(), list.end(), [](T *a, T *b) {return *a < *b;});
        return list;
    }

    static QList<Trip *> sortTrips(QList<Trip *> list, const int &hourBreak);

    Publications *publications() const;

    QJsonObject toJson();
    void setJson(const QJsonObject &);

    Busstop *newBusstop(QString id = "");
    Busstop *newBusstop(const QJsonObject &);
    Busstop *newBusstop(const Busstop &newBusstop);

    Line *newLine(QString id = "");
    Line *newLine(const QJsonObject &);
    Line *newLine(const Line &newLine);

    Tour *newTour(QString id = "");
    Tour *newTour(const QJsonObject &);
    Tour *newTour(const Tour &newTour);

    Footnote *newFootnote(QString id = "");
    Footnote *newFootnote(const QJsonObject &);
    Footnote *newFootnote(const Footnote &newFootnote);

private:
    QString _filePath;
    ProjectSettings *_projectSettings;
    QList<Busstop *> _busstops;
    QList<Line *> _lines;
    QList<Tour *> _tours;
    QList<Footnote *> _footnotes;
    Publications *_publications;

    std::unordered_map<Route *, Line *> _routeLineCacheMap;
    std::unordered_map<Trip *, Line *> _routeTripCacheMap;
};

#endif // PROJECTDATA_H
















