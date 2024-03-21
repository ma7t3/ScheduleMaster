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
        }


        f.close();

        counter ++;
    }

    foreach(QString currentLine, _lines) {
        emit newFileStarted(counter, currentLine);
        QFile f(_mapDir + "/TTData/" + currentLine + ".ttl");
        qDebug() << "Importing" << currentLine;
        counter ++;
    }
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
