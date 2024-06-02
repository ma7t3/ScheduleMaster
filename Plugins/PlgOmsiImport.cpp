#include "PlgOmsiImport.h"

#include <QtCore>
#include <QRegularExpression>

#include "App/global.h"

PlgOmsiImport::PlgOmsiImport(QObject *parent, ProjectData *projectData) :
    QThread(parent),
    projectData(projectData),
    _mergeTrips(true)
{}

void PlgOmsiImport::run() {
    int counter = 0;

    qInfo() << "starting import OMSI map:" << _mapDir;

    std::unordered_map<QString, QString> busstopsFromCfg;

    qInfo() << "loading busstops.cfg";

    QFile busstopsCfg(_mapDir + "/TTData/Busstops.cfg");
    if(!busstopsCfg.exists() || !busstopsCfg.open(QFile::ReadOnly))
        qCritical() << "Cannot read busstops.cfg file:" << busstopsCfg.errorString();

    QTextStream bs(&busstopsCfg);
    bs.setEncoding(QStringConverter::Latin1);
    while(!bs.atEnd()) {
        QString line = bs.readLine();
        if(line == "[busstop]") {
            QString name = bs.readLine();
            bs.readLine();
            QString id = bs.readLine();
            busstopsFromCfg[id] = name;
        }
    }

    busstopsCfg.close();


    qInfo() << "loading trips...";

    foreach(QString currentTrip, _trips) {
        QFile f(_mapDir + "/TTData/" + currentTrip + ".ttp");
        if(!f.exists()) {
            qCritical() << "Cannot import trip:" << currentTrip << "File not found!";
            continue;
        }
        if(!f.open(QFile::ReadOnly)) {
            qCritical() << "Cannot open trip file:" << currentTrip << "Reason:" << f.errorString();
            continue;
        }
        qDebug() << "Importing" << currentTrip;
        emit newFileStarted(counter, currentTrip);

        // determine line
        QRegularExpressionMatch match = _tripMask.match(currentTrip, 0, QRegularExpression::NormalMatch);
        QString lineName;
        if(!match.hasMatch())
            lineName = "Unkown";
        else
            lineName = match.captured(1);

        Line *l = projectData->lineWithName(lineName);
        LineDirection *ld1;
        LineDirection *ld2;
        if(!l) {
            QColor color(QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256));

            l = projectData->newLine();
            l->setName(lineName);
            l->setColor(color);
            l->setHourBreak(0);
            qInfo() << "new line created:" << lineName;
            projectData->addLine(l);
            ld1 = l->newDirection();
            ld2 = l->newDirection();
            ld1->setDescription("Direction 1");
            ld2->setDescription("Direction 2");
            l->addDirection(ld1);
            l->addDirection(ld2);
        }
        ld1 = l->directionAt(0);

        Route *r = l->newRoute();
        r->setName(currentTrip);
        r->setDirection(ld1);
        l->addRoute(r);

        QTextStream s(&f);
        s.setEncoding(QStringConverter::Latin1);

        while(!s.atEnd()) {
            QString line = s.readLine();

            startRouteWithoutReadingNextLine:

            if(line == "[station]") {
                s.readLine();
                s.readLine();
                QString name = s.readLine();
                Busstop *b = projectData->busstopWithName(name);
                if(!b) {
                    b = projectData->newBusstop();
                    b->setName(name);
                    projectData->addBusstop(b);
                    qInfo() << "new busstop created:" << name;
                }
                r->addBusstop(b);
            }

            if(line == "[station_typ2]") {
                QString id = s.readLine();
                auto it = busstopsFromCfg.find(id);
                if(it == busstopsFromCfg.end())
                    continue;

                QString name = busstopsFromCfg[id];
                Busstop *b = projectData->busstopWithName(name);
                if(!b) {
                    b = projectData->newBusstop();
                    b->setName(name);
                    projectData->addBusstop(b);
                    qInfo() << "new busstop created:" << name;
                }
                r->addBusstop(b);
            }

            if(line == "[profile]") {
                QString name        = s.readLine();
                QString durationStr = s.readLine();
                int duration;
                bool ok;
                duration = durationStr.toFloat(&ok);

                TimeProfile *p = r->newTimeProfile();
                p->setName(name);
                p->setDuration(duration);
                r->addTimeProfile(p);

                for(int i = 0; i < r->busstopCount(); i++) {
                    TimeProfileItem *itm = new TimeProfileItem(p, r->busstopAt(i));
                    p->addBusstop(itm);
                }

                while(!s.atEnd()) {
                    line = s.readLine();

                    if(line == "[profile_man_arr_time]" || line == "[profile_man_dep_time]") {
                        bool setDep = false;
                        if(line == "[profile_man_dep_time]")
                            setDep = true;

                        QString indexStr   = s.readLine();
                        QString timeStr = s.readLine();
                        int index;
                        float time;
                        bool ok1, ok2;
                        index = indexStr.toInt(&ok1);
                        time  = timeStr.toFloat(&ok2);

                        if(!ok1 || !ok2)
                            continue;

                        TimeProfileItem *itm = p->busstop(r->busstopAt(index));
                        if(!itm) {
                            itm = new TimeProfileItem(p, r->busstopAt(index));
                            itm->setBusstopMode(1);
                            p->addBusstop(itm);
                        }

                        if(setDep) {
                            // if we actually have a departure value, set seperate arrival before
                            itm->setArrValue(itm->depValue());
                            itm->setDepValue(time);
                        } else {
                            // import arrival value as departure because it makes more sense :P
                            itm->setDepValue(time);
                        }
                    }

                    if(line == "[profile_otherstopping]") {
                        QString indexStr = s.readLine();
                        QString modeStr  = s.readLine();
                        int index, mode;
                        bool ok1, ok2;
                        index = indexStr.toInt(&ok1);
                        mode  = modeStr.toInt(&ok2);

                        if(!ok1 || !ok2)
                            continue;

                        TimeProfileItem *itm = p->busstop(r->busstopAt(index));
                        if(!itm) {
                            itm = new TimeProfileItem(p, r->busstopAt(index));
                            p->addBusstop(itm);
                        }

                        itm->setBusstopMode(mode);
                    }

                    if(line == "[profile]")
                        goto startRouteWithoutReadingNextLine;
                }
            }
        }

        f.close();

        counter ++;
    }


    qInfo() << "loading lines...";

    foreach(QString currentLine, _lines) {
        QFile f(_mapDir + "/TTData/" + currentLine + ".ttl");
        if(!f.exists()) {
            qCritical() << "Cannot import line:" << currentLine << "File not found!";
            continue;
        }
        if(!f.open(QFile::ReadOnly)) {
            qCritical() << "Cannot open line file:" << currentLine << "Reason:" << f.errorString();
            continue;
        }
        qDebug() << "Importing" << currentLine;
        emit newFileStarted(counter, currentLine);

        QTextStream s(&f);
        s.setEncoding(QStringConverter::Latin1);

        while(!s.atEnd()) {
            QString line = s.readLine();

            startTourWithoutReadingNextLine:

            if(line == "[newtour]") {
                QString name = s.readLine();
                s.readLine();
                QString dayStr = s.readLine();
                WeekDays w = importWeekDays(dayStr);
                Tour *o = projectData->newTour();
                o->setName(name);
                o->setWeekDays(w);
                projectData->addTour(o);

                while(!s.atEnd()) {
                    nextTrip:
                    line = s.readLine();

                    if(line == "[addtrip]") {
                        QString routeName = s.readLine();
                        QString timeProfileIndexStr = s.readLine();
                        QString timeStr = s.readLine();

                        bool ok1, ok2;
                        int timeProfileIndex;
                        float timeFloat;

                        timeProfileIndex = timeProfileIndexStr.toInt(&ok1);
                        timeFloat        = timeStr.toFloat(&ok2);

                        if(!ok1 || !ok2)
                            return;

                        if(timeFloat >= 1440)
                            timeFloat -= 1440;

                        QTime time = QTime::fromMSecsSinceStartOfDay(timeFloat * 60 * 1000);
                        Route *r = projectData->routeWithName(routeName);
                        if(!r)
                            continue;

                        TimeProfile *p = r->timeProfileAt(timeProfileIndex);
                        if(!p && r->timeProfileCount() > 0)
                            p = r->timeProfileAt(0);
                        else if(!p)
                            continue;


                        Line *l = projectData->lineOfRoute(r);

                        // only do if we want to merge same trips
                        if(_mergeTrips) {
                            // check, if trip already exists
                            QList<Trip *> trips = l->tripsOfRoute(r);

                            for(int i = 0; i < trips.count(); i++) {
                                Trip *t = trips[i];
                                if(t->startTime() == time && t->timeProfile() == p) {
                                    t->setWeekDays(WeekDays::combine(t->weekDays(), w));
                                    o->addTrip(t);
                                    goto nextTrip;
                                }
                            }
                        }

                        Trip *t = l->newTrip();
                        t->setRoute(r);
                        t->setStartTime(time);
                        t->setTimeProfile(p);
                        t->setWeekDays(w);
                        l->addTrip(t);
                        o->addTrip(t);
                    }

                    if(line == "[newtour]")
                        goto startTourWithoutReadingNextLine;
                }
            }
        }

        f.close();
        counter++;
    }

    qInfo() << "refreshing direction assignments...";
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        Route *referenceRoute = nullptr;

        // determine longest route
        for(int j = 0; j < l->routeCount(); j++) {
            Route *rTest = l->routeAt(j);
            if(!referenceRoute || referenceRoute->busstopCount() < rTest->busstopCount())
                referenceRoute = rTest;
        }

        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            if(r == referenceRoute)
                continue;

            int minCount = std::min(referenceRoute->busstopCount(), r->busstopCount()) * 0.66;

            if(countCommonBusstopSequences(r, referenceRoute) < minCount)
                r->setDirection(l->directionAt(1));
        }

        LineDirection *ld1 = l->directionAt(0);
        LineDirection *ld2 = l->directionAt(1);

        QList<Route *> routes1 = l->routesToDirection(l->directionAt(0));
        QList<Route *> routes2 = l->routesToDirection(l->directionAt(1));

        int counter = 0;
        int codeCounter = 1;
        QString name;
        foreach(Route *r, routes1) {
            if(r->busstopCount() > counter)
                name = r->lastBusstop()->name();
            r->setCode(codeCounter);
            codeCounter += 2;
        }
        ld1->setDescription(name);

        counter = 0;
        codeCounter = 2;
        foreach(Route *r, routes2) {
            if(r->busstopCount() > counter)
                name = r->lastBusstop()->name();
            r->setCode(codeCounter);
            codeCounter += 2;
        }
        ld2->setDescription(name);
    }

    qInfo() << "generating day types";
    DayType *d1 = projectData->projectSettings()->newDayType();
    DayType *d2 = projectData->projectSettings()->newDayType();
    DayType *d3 = projectData->projectSettings()->newDayType();

    d1->setName(tr("Monday - Friday"));
    d1->setCode(WeekDay::monday|WeekDay::tuesday|WeekDay::wednesday|WeekDay::thursday|WeekDay::friday|WeekDay::school|WeekDay::vacation);

    d2->setName(tr("Saturday"));
    d1->setCode(WeekDay::saturday|WeekDay::school|WeekDay::vacation);

    d3->setName(tr("Sunday & Holiday"));
    d1->setCode(WeekDay::sunday|WeekDay::holiday|WeekDay::school|WeekDay::vacation);

    projectData->projectSettings()->addDayType(d1);
    projectData->projectSettings()->addDayType(d2);
    projectData->projectSettings()->addDayType(d3);
}

void PlgOmsiImport::setMapDirectory(const QString &path) {
    _mapDir = path;
}

void PlgOmsiImport::setTripMask(const QString &tripMask) {
    _tripMask.setPattern(tripMask);
}

void PlgOmsiImport::setSelectedTrips(const QStringList &selectedTrips) {
    _trips = selectedTrips;
}

void PlgOmsiImport::setSelectedLines(const QStringList &selectedLines) {
    _lines = selectedLines;
}

WeekDays PlgOmsiImport::importWeekDays(const QString &str) {
    bool ok;
    int num = str.toInt(&ok);
    if(!ok)
        return WeekDays(WeekDay::monday|WeekDay::tuesday|WeekDay::wednesday|WeekDay::thursday|WeekDay::friday|WeekDay::school|WeekDay::vacation);

    QString bin = QString::number(num, 2);

    while(bin.length() < 10)
        bin = "0" + bin;

    WeekDays w;
    w.setDay(monday,    bin[9] == '1');
    w.setDay(tuesday,   bin[8] == '1');
    w.setDay(wednesday, bin[7] == '1');
    w.setDay(thursday,  bin[6] == '1');
    w.setDay(friday,    bin[5] == '1');
    w.setDay(saturday,  bin[4] == '1');
    w.setDay(sunday,    bin[3] == '1');
    w.setDay(holiday,   bin[2] == '1');
    w.setDay(school,    bin[1] == '1');
    w.setDay(vacation,  bin[0] == '1');
    return w;
}

int PlgOmsiImport::countCommonBusstopSequences(Route *r1, Route *r2) const {
    int counter = 0;

    for(int i = 0; i < r1->busstopCount(); i++) {
        Busstop *b = r1->busstopAt(i);
        if(!r2->hasBusstop(b))
            continue;

        int index = r2->indexOfBusstop(b);
        if(index >= r2->busstopCount() || i >= r1->busstopCount())
            continue;

        if(r1->busstopAt(i + 1) == r2->busstopAt(index + 1))
            counter++;
    }

    return counter;
}

void PlgOmsiImport::setMergeTrips(bool newMergeTrips) {
    _mergeTrips = newMergeTrips;
}
