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
#include "footnote.h"

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

    QList<Footnote *> footnotes() const;
    void setFootnotes(const QList<Footnote *> &newFootnotes);
    void addFootnote(Footnote *);
    void removeFootnote(Footnote *);
    void removeFootnote(const QString &);
    Footnote *footnoteAt(int);
    int footnoteCount() const;


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

    Publications *publications() const;

    QJsonObject toJson();
    void setJson(const QJsonObject &);

private:
    QString _filePath;
    ProjectSettings *_projectSettings;
    QList<Busstop *> _busstops;
    QList<Line *> _lines;
    QList<Tour *> _tours;
    QList<Footnote *> _footnotes;
    Publications *_publications;
};

#endif // PROJECTDATA_H
















