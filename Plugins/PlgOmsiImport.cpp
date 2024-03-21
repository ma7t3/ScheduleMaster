#include "PlgOmsiImport.h"

#include <QtCore>
#include <QRegularExpression>

#include "App/global.h"

PlgOmsiImport::PlgOmsiImport(QObject *parent, ProjectData *projectData) :
    QThread(parent),
    projectData(projectData)
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
        if(!l) {
            l = new Line(projectData, global::getNewID(), lineName, "", Qt::white);
            qInfo() << "new line created:" << lineName;
            projectData->addLine(l);
            l->addDirection(new LineDirection(l, global::getNewID(), "direction"));
        }
        LineDirection *ld = l->directionAt(0);

        Route *r = new Route(l, global::getNewID(), 1, currentTrip, ld);
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
                    b = new Busstop(projectData, global::getNewID(), name);
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
                    b = new Busstop(projectData, global::getNewID(), name);
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

                TimeProfile *p = new TimeProfile(r, global::getNewID(), name);
                p->setDuration(duration);
                r->addTimeProfile(p);

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
                Tour *o = new Tour(projectData, global::getNewID(), name, w);
                projectData->addTour(o);

                while(!s.atEnd()) {
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

                        QTime time = QTime::fromMSecsSinceStartOfDay(timeFloat * 60 * 1000);
                        Route *r = projectData->routeWithName(routeName);

                        TimeProfile *p = r->timeProfileAt(timeProfileIndex);
                        if(!p && r->timeProfileCount() > 0)
                            p = r->timeProfileAt(0);
                        else if(!p)
                            continue;

                        Line *l = projectData->lineOfRoute(r);
                        Trip *t = new Trip(nullptr, global::getNewID(), r, time, p, w);
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

    projectData->projectSettings()->addDayType(new DayType(projectData, global::getNewID(), "Monday - Firday", 995));
    projectData->projectSettings()->addDayType(new DayType(projectData, global::getNewID(), "Saturday", 19));
    projectData->projectSettings()->addDayType(new DayType(projectData, global::getNewID(), "Sunday & Holiday", 15));
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
        return WeekDays(nullptr, 995);

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
















