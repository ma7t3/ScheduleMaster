
#include "ProjectData\projectdata.h"

ProjectData::ProjectData() :
    _projectSettings(new ProjectSettings),
    _publications(new Publications)
{}

ProjectSettings *ProjectData::projectSettings() { return _projectSettings; }

void ProjectData::reset() {
    _lines.clear();
    _busstops.clear();
    _tours.clear();
    _projectSettings = new ProjectSettings;
}

void ProjectData::cleanup() {
    _busstops = sortBusstops(_busstops);
    _lines = sortLines(_lines);
    _tours = sortTours(_tours);
    
    for(int i = 0; i < _lines.count(); i++) {
        Line *l = _lines[i];
        l->setRouteList(sortRoutes(l->routes()));
        l->setTripList(sortTrips(l->trips()));
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

void ProjectData::addBusstop(Busstop *b) { _busstops << b; }
void ProjectData::addLine(Line *l)       { _lines << l; }
void ProjectData::addTour(Tour *o)       { _tours << o; }


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

QList <Busstop *> ProjectData::busstops() { return _busstops; }



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

QList<Line *> ProjectData::lines() { return _lines; }

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
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            if(l->routeAt(j)->id() == r->id())
                return l;
        }
    }

    return nullptr;
}

Line *ProjectData::lineOfTrip(Trip *t) {
    if(!t)
        return nullptr;
    
    for(int i = 0; i < this->lineCount(); i++) {
        Line *l = this->lineAt(i);
        for(int j = 0; j < l->tripCount(); j++) {
            if(l->tripAt(j)->id() == t->id())
                return l;
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
            for(int k = 0; k < r->profileCount(); k++) {
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

    return sortLines(usedLines);
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

QList<Busstop *> ProjectData::sortBusstops(QList<Busstop *> list) {
    std::sort(list.begin(), list.end(), [](Busstop *a, Busstop *b) {return *a < *b;});
    return list;
}

QList<Line *> ProjectData::sortLines(QList<Line *> list) {
    std::sort(list.begin(), list.end(), [](Line *a, Line *b) {return *a < *b;});
    return list;
}

QList<Route *> ProjectData::sortRoutes(QList<Route *> list) {
    std::sort(list.begin(), list.end(), [](Route *a, Route *b) {return *a < *b;});
    return list;
}

QList<Trip *> ProjectData::sortTrips(QList<Trip *> list) {
    std::sort(list.begin(), list.end(), [](Trip *a, Trip *b) {return *a < *b;});
    return list;
}

QList<Tour *> ProjectData::sortTours(QList<Tour *> list) {
    std::sort(list.begin(), list.end(), [](Tour *a, Tour *b) {return *a < *b;});
    return list;
}

Publications *ProjectData::publications() const {
    return _publications;
}
