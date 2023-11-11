#pragma once

#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QtCore>
#include <QJsonDocument>
#include <QColor>

#include "busstop.h"
#include "line.h"
#include "tour.h"
#include "projectsettings.h"
#include "publications.h"

class ProjectData {
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

    int lineCount();
    Line *line(QString);
    Line *lineAt(int);
    QList<Line *> lines();

    Route *route(QString);
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


    QList<Route *> matchingRoutes(Route *);

    Line *lineOfRoute(Route *);
    Line *lineOfTrip(Trip *);
    Route *routeOfTimeProfile(TimeProfile *);

    QList<Line *> linesAtBusstop(Busstop *b);
    QList<Line *> linesAtBusstop(QString);

    QList<Tour *> toursOfTrip(Trip *);

    static QList<Busstop *> sortBusstops(QList<Busstop *>);
    static QList<Line *> sortLines(QList<Line *>);
    static QList<Route *> sortRoutes(QList<Route *>);
    static QList<Trip *> sortTrips(QList<Trip *>);
    static QList<Tour *> sortTours(QList<Tour *>);

    Publications *publications() const;

private:
    QString m_filePath;
    ProjectSettings *m_projectSettings;
    QList<Busstop *> m_busstops;
    QList<Line *> m_lines;
    QList<Tour *> m_tours;
    Publications *m_publications;
};

#endif // PROJECTDATA_H
















