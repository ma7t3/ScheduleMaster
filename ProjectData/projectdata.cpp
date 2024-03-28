
#include "ProjectData\projectdata.h"

ProjectData::ProjectData() :
    QObject(nullptr),
    _projectSettings(new ProjectSettings(this)),
    _publications(new Publications(this))
{}

ProjectSettings *ProjectData::projectSettings() { return _projectSettings; }

void ProjectData::reset() {
    _filePath = "";

    _projectSettings->reset();
    qDebug() << "projectSettings reset";

    _publications->reset();
    qDebug() << "publications reset";

    qDeleteAll(_tours);
    _tours.clear();
    qDebug() << "tours deleted";

    qDeleteAll(_lines);
    _lines.clear();
    qDebug() << "lines deleted";

    qDeleteAll(_footnotes);
    _footnotes.clear();
    qDebug() << "footnotes deleted";

    qDeleteAll(_busstops);
    _busstops.clear();
    qDebug() << "busstops deleted";

    _routeLineCacheMap.clear();
    _routeTripCacheMap.clear();
}

void ProjectData::cleanup() {
    _busstops = sortItems(_busstops);
    _lines = sortItems(_lines);
    _tours = sortItems(_tours);
    
    for(int i = 0; i < _lines.count(); i++) {
        Line *l = _lines[i];
        l->setRoutes(sortItems(l->routes()));
        l->setTrips(sortItems(l->trips()));
    }
}

void ProjectData::clearLines() {
    _lines.clear();
}

void ProjectData::setFilePath(QString str) {
    _filePath = str;
}

QString ProjectData::filePath() {
    return _filePath;
}

void ProjectData::addBusstop(Busstop *b) {
    if(!b)
        return;

    b->setParent(this);
    _busstops << b;
}
void ProjectData::addLine(Line *l) {
    if(!l)
        return;

    l->setParent(this);
    _lines << l;
}

void ProjectData::addTour(Tour *o) {
    if(!o)
        return;

    o->setParent(this);
    _tours << o;
}


int ProjectData::busstopCount() { return _busstops.count(); }

Busstop *ProjectData::busstop(QString id) {
    for (int i = 0; i < busstopCount(); ++i)
        if(busstopAt(i)->id() == id)
            return busstopAt(i);

    return nullptr;
}

Busstop *ProjectData::busstopAt(int i) {
    if(_busstops.count() < i)
        return nullptr;
    
    return _busstops[i];
}

QList<Busstop *> ProjectData::busstops() {
    return _busstops;
}

Busstop *ProjectData::busstopWithName(const QString &name) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->name().toLower() == name.toLower())
            return busstopAt(i);
    }
    return nullptr;
}

bool ProjectData::busstopWithNameExists(const QString &name) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->name().toLower() == name.toLower())
            return true;
    }
    return false;
}

int ProjectData::lineCount() { return _lines.count(); }

Line *ProjectData::line(QString id) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->id() == id)
            return lineAt(i);

    return nullptr;
}

Line *ProjectData::lineAt(int i) {
    if(_lines.count() < i)
        return nullptr;
    
    return _lines[i];
}

QList<Line *> ProjectData::lines() {
    return _lines;
}

Line *ProjectData::lineWithName(const QString &name) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->name().toLower() == name.toLower())
            return lineAt(i);

    return nullptr;
}

bool ProjectData::lineWithNameExists(const QString &name) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->name().toLower() == name.toLower())
            return true;

    return false;
}

Route *ProjectData::route(QString id) {
    for(int i = 0; i < lineCount(); i++) {
        Line *l = lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            if(r->id() == id)
                return r;
        }
    }

    return nullptr;
}

Route *ProjectData::routeWithName(QString name) {
    for(int i = 0; i < lineCount(); i++) {
        Line *l = lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            if(r->name() == name)
                return r;
        }
    }

    return nullptr;
}

Trip *ProjectData::trip(QString id) {
    for(int i = 0; i < lineCount(); i++) {
        Line *l = lineAt(i);
        Trip *t = l->trip(id);
        if(t)
            return t;
    }

    return nullptr;
}


int ProjectData::tourCount() { return _tours.count(); }

Tour *ProjectData::tour(QString id) {
    for(int i = 0; i < tourCount(); i++)
        if(tourAt(i)->id() == id)
            return tourAt(i);

    return nullptr;
}

Tour *ProjectData::tourAt(int i) {
    if(i < 0 || i >= _tours.count())
        return nullptr;
    
    return _tours[i];
}

QList<Tour *> ProjectData::tours() { return _tours; }


bool ProjectData::removeBusstop(Busstop *b) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i) != b)
            continue;
        
        _busstops.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeBusstop(QString id) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->id() != id)
            continue;
        
        _busstops.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeLine(Line *l) {
    for(int i = 0; i < lineCount(); i++) {
        if(lineAt(i) != l)
            continue;
        
        _lines.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeLine(QString id) {
    for(int i = 0; i < lineCount(); i++) {
        if(lineAt(i)->id() != id)
            continue;
        
        _lines.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeTour(Tour *o) {
    for(int i = 0; i < tourCount(); i++) {
        if(tourAt(i) != o)
            continue;
        
        _tours.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeTour(QString id) {
    for(int i = 0; i < tourCount(); i++) {
        if(tourAt(i)->id() != id)
            continue;
        
        _tours.remove(i);
        return true;
    }

    return false;
}

QList<Footnote *> ProjectData::footnotes() const {
    return _footnotes;
}

void ProjectData::setFootnotes(const QList<Footnote *> &newFootnotes) {
    _footnotes = newFootnotes;
}

void ProjectData::addFootnote(Footnote *f) {
    if(!f)
        return;

    f->setParent(this);
    _footnotes << f;
}

void ProjectData::removeFootnote(Footnote *f) {
    for (int i = 0; i < footnoteCount(); ++i)
        if(footnoteAt(i) == f) {
            _footnotes.remove(i);
            return;
        }
}

void ProjectData::removeFootnote(const QString &id) {
    for (int i = 0; i < footnoteCount(); ++i)
        if(footnoteAt(i)->id() == id) {
            _footnotes.remove(i);
            return;
        }
}

Footnote *ProjectData::footnoteAt(int index) {
    if(index < 0 || index > footnoteCount())
        return nullptr;

    return _footnotes[index];
}

int ProjectData::footnoteCount() const {
    return _footnotes.count();
}

QList<Route *> ProjectData::matchingRoutes(Route *inputR) {
    QList<Route *> result;
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            for(int k = 0 ; k < r->busstopCount(); k++) {
                Busstop *b = r->busstopAt(k);
                for(int m = 0; m < inputR->busstopCount(); m++) {
                    Busstop *c = inputR->busstopAt(m);
                    if(b == c)
                        if(r->busstopAt(k + 1) == inputR->busstopAt(m + 1))
                            if(result.count() == 0 || result[result.count() - 1] != r)
                                result << r;
                }
            }
        }
    }
    return result;
}

Line *ProjectData::lineOfRoute(Route *r) {
    if(!r)
        return nullptr;

    auto it = _routeLineCacheMap.find(r);
    if(it != _routeLineCacheMap.end())
        return _routeLineCacheMap[r];
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            if(l->routeAt(j)->id() == r->id()) {
                _routeLineCacheMap[r] = l;
                return l;
            }
        }
    }

    return nullptr;
}

Line *ProjectData::lineOfTrip(Trip *t) {
    if(!t)
        return nullptr;

    auto it = _routeTripCacheMap.find(t);
    if(it != _routeTripCacheMap.end())
        return _routeTripCacheMap[t];
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->tripCount(); j++) {
            if(l->tripAt(j)->id() == t->id()) {
                _routeTripCacheMap[t] = l;
                return l;
            }
        }
    }

    return nullptr;
}

Route *ProjectData::routeOfTimeProfile(TimeProfile *p) {
    if(!p)
        return nullptr;
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            for(int k = 0; k < r->timeProfileCount(); k++) {
                if(r->timeProfileAt(k)->id() == p->id())
                    return r;
            }
        }
    }

    return nullptr;
}

QList<Trip *> ProjectData::tripsOfRoute(Route *route) {
    QList<Trip *> resultList;

    for(int i = 0; i < lineCount(); i++) {
        Line *l = lineAt(i);
        resultList << l->tripsOfRoute(route);
    }

    return resultList;
}

QList<Line *> ProjectData::linesAtBusstop(QString id) {
    
    Busstop *b = this->busstop(id);
    if(!b)
        return {};
    
    return linesAtBusstop(b);
}

QList<Line *> ProjectData::linesAtBusstop(Busstop *busstop) {

    if(!busstop)
        return {};

    QList<Line *> usedLines;
    
    for(int i = 0; i < this->lineCount(); i++) {
        bool found = false;
        Line *l = this->lineAt(i);
        Line *currentLine = l;
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            for(int k = 0; k < r->busstopCount(); k++) {
                Busstop *b = r->busstopAt(k);
                if(b->id() == busstop->id()) {
                    found = true;
                    goto endpoint;
                }
            }
        }
    endpoint:
        if(found)
            usedLines << currentLine;
    }

    return sortItems(usedLines);
}

QList<Tour *> ProjectData::toursOfTrip(Trip *t)
{
    QString tripID = t->id();
    QList<Tour *> resultList;
    
    for(int i = 0; i < this->tourCount(); i++) {
        Tour *o = this->tourAt(i);
        for(int j = 0; j < o->tripCount(); j++) {
            Trip *c = o->tripAt(j);
            if(c->id() == tripID) {
                resultList << o;
                break;
            }
        }
    }

    return resultList;
}

QList<Busstop *> ProjectData::combinedRoutes(const QList<Route *> &routes) {
    QList<Busstop *> result;

    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];
        Busstop *lastBusstop = nullptr;
        bool firstBusstopFound = false;

        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);

            // determine, if busstop was already added
            bool busstopAlreadyExists = false;
            for(int k = 0; k < result.count(); k++) {
                if(result[k] == b) {
                    busstopAlreadyExists = true;
                    firstBusstopFound = true;
                    lastBusstop = b;
                    break;
                }
            }

            if(busstopAlreadyExists)
                continue;

            int targetIndex;
            if(firstBusstopFound) {
                targetIndex = result.count();
            } else {
                targetIndex = 0;
            }

            for(int k = 0; k < result.count(); k++) {
                if(result[k] == lastBusstop) {
                    targetIndex = k + 1;
                    break;
                }
            }

            result.insert(targetIndex, b);
            lastBusstop = b;
        }
    }

    return result;
}

Publications *ProjectData::publications() const {
    return _publications;
}

QJsonObject ProjectData::toJson() {
    QJsonObject jsonObject;

    QJsonArray jBusstops, jLines, jTours, jFootnotes;

    for(int i = 0; i < busstopCount(); ++i)
        jBusstops.append(busstopAt(i)->toJson());

    for(int i = 0; i < lineCount(); ++i)
        jLines.append(lineAt(i)->toJson());

    for(int i = 0; i < tourCount(); ++i)
        jTours.append(tourAt(i)->toJson());

    jsonObject.insert("busstops", jBusstops);
    jsonObject.insert("lines", jLines);
    jsonObject.insert("tours", jTours);
    qDebug() << "a";
    jsonObject.insert("projectSettings", projectSettings()->toJson());
    qDebug() << "projectSettings saved.";
    jsonObject.insert("publications", publications()->toJson());
    qDebug() << "publications saved.";

    return jsonObject;
}

void ProjectData::setJson(const QJsonObject &jsonObject) {
    QJsonArray jBusstops, jLines, jTours, jFootnotes;

    jBusstops = jsonObject.value("busstops").toArray();
    jLines = jsonObject.value("lines").toArray();
    jTours = jsonObject.value("tours").toArray();

    int counter = 0, invalidCounter = 0;
    for(int i = 0; i < jBusstops.count(); ++i)
        if(jBusstops[i].isObject()) {
            counter++;
            addBusstop(new Busstop(this, jBusstops[i].toObject()));
        } else invalidCounter++;

    qInfo().noquote() << counter << "valid busstops found (" + QString::number(invalidCounter) + " invalid)";

    counter = 0, invalidCounter = 0;
    for(int i = 0; i < jLines.count(); ++i)
        if(jLines[i].isObject()) {
            counter++;
            addLine(new Line(this, jLines[i].toObject()));
        } else invalidCounter++;

    qInfo().noquote() << counter << "valid lines found (" + QString::number(invalidCounter) + " invalid)";

    counter = 0, invalidCounter = 0;
    for(int i = 0; i < jTours.count(); ++i)
        if(jTours[i].isObject()) {
            counter++;
            addTour(new Tour(this, jTours[i].toObject()));
        } else invalidCounter++;

    qInfo().noquote() << counter << "valid tours found (" + QString::number(invalidCounter) + " invalid)";

    projectSettings()->setJson(jsonObject.value("projectSettings").toObject());
    publications()->setJson(jsonObject.value("publications").toObject());
}









