#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QtCore>
#include <QJsonDocument>
#include <QColor>
#include <QUndoStack>

#include "projectData/busstop.h"
#include "projectData/line.h"
#include "projectData/tour.h"
#include "projectData/projectsettings.h"
#include "projectData/publications.h"
#include "projectData/footnote.h"

class ProjectData : public QObject {
    Q_OBJECT
public:
    ProjectData(QObject *parent = nullptr);
    ~ProjectData();

    ProjectSettings *projectSettings();

    void reset();
    void cleanup();

    void clearLines();

    void setFilePath(QString);
    QString filePath();


    void addBusstop(Busstop *);
    int busstopCount();
    Busstop *busstop(QString);
    Busstop *busstopAt(int);
    QList <Busstop*> busstops() const;
    Busstop *busstopWithName(const QString &name);
    bool busstopWithNameExists(const QString &name);
    bool removeBusstop(Busstop *);
    bool removeBusstop(QString);

    void addLine(Line *);
    int lineCount();
    Line *line(QString);
    Line *lineAt(int);
    QList<Line *> lines();
    Line *lineWithName(const QString &name);
    bool lineWithNameExists(const QString &name);

    Route *route(QString);
    Route *routeWithName(QString);
    Trip *trip(QString);

    void addTour(Tour *);
    int tourCount();
    Tour *tour(QString);
    Tour *tourAt(int);
    QList<Tour *> tours();



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

    Publications *publications();

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

    QUndoStack *undoStack();

    void onBusstopAdded(Busstop *);
    void onBusstopChanged(Busstop *);
    void onBusstopRemoved(Busstop *);

    void onLineAdded(Line *);
    void onLineChanged(Line *);
    void onLineRemoved(Line *);

signals:
    void loadingProgressMaxValue(const int &maxValue);
    void loadingProgressUpdated(const int &currentValue);
    void loadingProgressTextUpdated(const int &type, const QString &message, const bool &showAsCurrent = false);

    void wasReset();

    void busstopsAdded(const QList<Busstop *>);
    void busstopsChanged(const QList<Busstop *>);
    void busstopsRemoved(const QList<Busstop *>);

    void linesAdded(const QList<Line *>);
    void linesChanged(const QList<Line *>);
    void linesRemoved(const QList<Line *>);

private:
    QString _filePath;
    ProjectSettings *_projectSettings;
    QList<Busstop *> _busstops;
    QList<Line *> _lines;
    QList<Tour *> _tours;
    QList<Footnote *> _footnotes;
    Publications *_publications;

    QList<Busstop *> _addedBusstops;
    QList<Busstop *> _changedBusstops;
    QList<Busstop *> _removedBusstops;

    QList<Line *> _addedLines;
    QList<Line *> _changedLines;
    QList<Line *> _removedLines;

    QUndoStack _undoStack;

    std::unordered_map<Route *, Line *> _routeLineCacheMap;
    std::unordered_map<Trip *, Line *> _routeTripCacheMap;

    QTimer *_updateTimer;
};

#endif // PROJECTDATA_H
















