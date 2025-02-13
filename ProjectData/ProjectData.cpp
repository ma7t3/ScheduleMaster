
#include "ProjectData\projectdata.h"

ProjectData::ProjectData(QObject *parent) :
    QObject(parent),
    _projectSettings(new ProjectSettings(this)),
    _publications(new Publications(this)),
    _updateTimer(new QTimer(this)),
    _loadingFromFile(false),
    _afterLoadingFromFile(false) {
    _updateTimer->setSingleShot(true);

    connect(_updateTimer, &QTimer::timeout, this, [this]() {
        if(_afterLoadingFromFile) {
            _afterLoadingFromFile = false;
            emit wasReset();
        } else {
            if(!_addedBusstops.isEmpty())
                emit busstopsAdded(_addedBusstops);

            if(!_changedBusstops.isEmpty())
                emit busstopsChanged(_changedBusstops);

            if(!_removedBusstops.isEmpty())
                emit busstopsRemoved(_removedBusstops);

            if(!_addedLines.isEmpty())
                emit linesAdded(_addedLines);

            if(!_changedLines.isEmpty())
                emit linesChanged(_changedLines);

            if(!_removedLines.isEmpty())
                emit linesRemoved(_removedLines);

            if(!_addedTours.isEmpty())
                emit toursAdded(_addedTours);

            if(!_changedTours.isEmpty())
                emit toursChanged(_changedTours);

            if(!_removedTours.isEmpty())
                emit toursRemoved(_removedTours);
        }

        _addedBusstops.clear();
        _changedBusstops.clear();
        _removedBusstops.clear();

        _addedLines.clear();
        _changedLines.clear();
        _removedLines.clear();

        _addedTours.clear();
        _changedTours.clear();
        _removedTours.clear();
    });
}

ProjectData::~ProjectData() {

}

ProjectSettings *ProjectData::projectSettings() { return _projectSettings; }

void ProjectData::reset() {
    _filePath = "";

    qDebug() << "\tclearing undoStack...";
    _undoStack.clear();

    qDebug() << "\tdeleting all childs...";
    QObjectList childs = children();
    for(QObject *c : childs) {
        if(c == _updateTimer)
            continue;
        c->deleteLater();
    }

    qDebug() << "\tresetting projectSettings...";
    _projectSettings = new ProjectSettings(this);

    qDebug() << "\tresetting publications...";
    _publications = new Publications(this);

    qDebug() << "\tdeleting tours...";
    _tours.clear();

    qDebug() << "\tdeleting lines...";
    _lines.clear();

    qDebug() << "\tdeleting footnotes...";
    _footnotes.clear();

    qDebug() << "\tdeleting busstops...";
    _busstops.clear();

    qDebug() << "\tremoving cache...";
    _routeLineCacheMap.clear();
    _routeTripCacheMap.clear();

    qDebug() << "\tprojectData reset!";

    emit wasReset();
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

    b->setInUse(true);
    onBusstopAdded(b);
    _busstops << b;
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

QList<Busstop *> ProjectData::busstops() const {
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

bool ProjectData::removeBusstop(Busstop *b) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i) != b)
            continue;

        _busstops[i]->setInUse(false);
        onBusstopRemoved(b);
        _busstops.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeBusstop(QString id) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->id() != id)
            continue;

        _busstops[i]->setInUse(false);
        onBusstopRemoved(_busstops[i]);
        _busstops.remove(i);
        return true;
    }

    return false;
}

void ProjectData::addLine(Line *l) {
    if(!l)
        return;

    l->setParent(this);
    l->setInUse(true);
    onLineAdded(l);
    _lines << l;
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


void ProjectData::addTour(Tour *o) {
    if(!o)
        return;

    o->setParent(this);
    onTourAdded(o);
    _tours << o;
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

bool ProjectData::removeLine(Line *l) {
    for(int i = 0; i < lineCount(); i++) {
        if(lineAt(i) != l)
            continue;
        
        onLineRemoved(l);
        l->setInUse(true);
        _lines.remove(i);
        return true;
    }

    return false;
}

bool ProjectData::removeLine(QString id) {
    for(int i = 0; i < lineCount(); i++) {
        if(lineAt(i)->id() != id)
            continue;
        
        onLineRemoved(lineAt(i));
        lineAt(i)->setInUse(true);
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
        onTourRemoved(o);
        return true;
    }

    return false;
}

bool ProjectData::removeTour(QString id) {
    for(int i = 0; i < tourCount(); i++) {
        if(tourAt(i)->id() != id)
            continue;
        
        Tour *o = tourAt(i);
        _tours.remove(i);
        onTourRemoved(o);
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

QList<Footnote *> ProjectData::autoAssignedFootnotesOfTrip(Trip *t) {

    QList<Footnote *> result;

    for(int i = 0; i < footnoteCount(); i++) {
        Footnote *f = footnoteAt(i);
        WeekDays w, tW, cW;
        w  = t->weekDays();
        tW = f->autoAssignWeekDays();
        cW = f->autoAssignCareWeekDays();

        // equalize don't-care states
        if(!cW.day(monday))    w.setDay(monday,    tW.day(monday));
        if(!cW.day(tuesday))   w.setDay(tuesday,   tW.day(tuesday));
        if(!cW.day(wednesday)) w.setDay(wednesday, tW.day(wednesday));
        if(!cW.day(thursday))  w.setDay(thursday,  tW.day(thursday));
        if(!cW.day(friday))    w.setDay(friday,    tW.day(friday));
        if(!cW.day(saturday))  w.setDay(saturday,  tW.day(saturday));
        if(!cW.day(sunday))    w.setDay(sunday,    tW.day(sunday));
        if(!cW.day(holiday))   w.setDay(holiday,   tW.day(holiday));
        if(!cW.day(school))    w.setDay(school,    tW.day(school));
        if(!cW.day(vacation))  w.setDay(vacation,  tW.day(vacation));

        if(w == tW)
            result << f;
    }

    return result;
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

QList<Trip *> ProjectData::sortTrips(QList<Trip *> list, const int &hourBreak) {
    // first: sort by start time
    std::sort(list.begin(), list.end(), [](Trip *a, Trip *b) {
        return *a < *b;
    });

    // 2nd run: gnome sort by first common busstop
    int i = 0;
    while(true) {
        if(i + 1 >= list.count()) // finish if we reached the end
            break;

        // compare
        Trip *t1 = list[i];
        Trip *t2 = list[i + 1];
        Busstop *b = t1->route()->firstCommonBusstop(t2->route());

        bool swap;
        if(!b)
            swap = t1->startTime() > t2->startTime();
        else
            swap = t1->busstopTime(b) > t2->busstopTime(b);

        if(swap) {
            list[i] = t2;
            list[i + 1] = t1;

            if(i > 0) i--;
            else      i++;

        } else {
            i++;
        }
    }

    if(list.isEmpty())
        return list;

    int j = 0;
    while(list.first()->startTime().hour() < hourBreak) {
        if(j == list.count())
            break;

        Trip *t = list.first();
        list.removeFirst();
        list << t;
        j++;
    }

    return list;
}

QPixmap ProjectData::linesPixmap(const QList<Line *> lines) {
    QFont f;
    QFontMetrics fm(f);

    int xValue = 5;
    for(int j = 0; j < lines.count(); j++) {
        int width = fm.boundingRect(lines[j]->name()).width();
        xValue += (width + 20);
    }

    QPixmap pixmap(xValue, 15);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    xValue = 5;
    for(int j = 0; j < lines.count(); j++) {
        int width = fm.boundingRect(lines[j]->name()).width();
        QColor color = lines[j]->color();
        QRect rect(xValue, 0, width + 15, 15);
        xValue += (width + 20);
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect, 7.5, 7.5);
        painter.setPen(global::getContrastColor(color));
        painter.drawText(rect, Qt::AlignCenter, lines[j]->name());
    }

    return pixmap;
}

QPixmap ProjectData::linesPixmap(Busstop *b) {
    return linesPixmap(linesAtBusstop(b));
}

QPixmap ProjectData::linesPixmap(Tour *o) {
    QList<Line *> lines;
    QList<Trip *> trips = o->trips();
    for(Trip *t : trips) {
        Line *l = lineOfTrip(t);
        if(!lines.contains(l))
            lines << l;
    }
    lines = sortItems(lines);
    return linesPixmap(lines);
}

Publications *ProjectData::publications() {
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

    for(int i = 0; i < footnoteCount(); i++) {
        jFootnotes.append(footnoteAt(i)->toJson());
    }

    jsonObject.insert("busstops", jBusstops);
    jsonObject.insert("lines", jLines);
    jsonObject.insert("tours", jTours);
    jsonObject.insert("footnotes", jFootnotes);
    jsonObject.insert("projectSettings", projectSettings()->toJson());
    qDebug() << "projectSettings saved.";
    jsonObject.insert("publications", publications()->toJson());
    qDebug() << "publications saved.";

    return jsonObject;
}

void ProjectData::setJson(const QJsonObject &jsonObject) {
    _loadingFromFile = true;
    QJsonArray jBusstops, jLines, jTours, jFootnotes;

    jBusstops = jsonObject.value("busstops").toArray();
    jLines = jsonObject.value("lines").toArray();
    jTours = jsonObject.value("tours").toArray();
    jFootnotes = jsonObject.value("footnotes").toArray();

    int totalCount = jBusstops.count() +
                     jLines.count() +
                     jTours.count() +
                     jFootnotes.count();

    emit loadingProgressMaxValue(totalCount);

    QString progressText = tr("Reading busstops...");
    emit loadingProgressTextUpdated(1, progressText, true);
    int counter = 0, invalidCounter = 0, maxCount = jBusstops.count();
    for(int i = 0; i < maxCount; ++i) {
        if(jBusstops[i].isObject()) {
            counter++;
            addBusstop(newBusstop(jBusstops[i].toObject()));
        } else invalidCounter++;
        emit loadingProgressUpdated(i + 1);
        emit loadingProgressTextUpdated(1, progressText + " (" + QString::number(counter) + "/" + QString::number(maxCount) + ")", true);
    }

    qInfo().noquote() << counter << "valid busstops found (" + QString::number(invalidCounter) + " invalid)";

    progressText = tr("Reading lines...");
    emit loadingProgressTextUpdated(1, progressText, true);
    counter = 0, invalidCounter = 0, maxCount = jLines.count();
    for(int i = 0; i < maxCount; ++i) {
        if(jLines[i].isObject()) {
            counter++;
            addLine(newLine(jLines[i].toObject()));
        } else invalidCounter++;
        emit loadingProgressUpdated(i + 1 + jLines.count());
        emit loadingProgressTextUpdated(1, progressText + " (" + QString::number(counter) + "/" + QString::number(maxCount) + ")", true);
    }

    qInfo().noquote() << counter << "valid lines found (" + QString::number(invalidCounter) + " invalid)";
    progressText = tr("Reading tours...");
    emit loadingProgressTextUpdated(1, progressText, true);
    counter = 0, invalidCounter = 0, maxCount = jTours.count();
    for(int i = 0; i < maxCount; ++i) {
        if(jTours[i].isObject()) {
            counter++;
            addTour(newTour(jTours[i].toObject()));
        } else invalidCounter++;
        emit loadingProgressUpdated(i + 1 + jBusstops.count() + jLines.count());
        emit loadingProgressTextUpdated(1, progressText + " (" + QString::number(counter) + "/" + QString::number(maxCount) + ")", true);
    }

    qInfo().noquote() << counter << "valid tours found (" + QString::number(invalidCounter) + " invalid)";
    progressText = tr("Reading footnotes...");
    emit loadingProgressTextUpdated(1, progressText, true);
    counter = 0, invalidCounter = 0, maxCount = jFootnotes.count();
    for(int i = 0; i < maxCount; ++i) {
        if(jFootnotes[i].isObject()) {
            counter++;
            addFootnote(newFootnote(jFootnotes[i].toObject()));
        } else invalidCounter++;
        emit loadingProgressUpdated(i + 1 + jBusstops.count() + jLines.count() + jTours.count());
        emit loadingProgressTextUpdated(1, progressText + " (" + QString::number(counter) + "/" + QString::number(maxCount) + ")", true);
    }

    qInfo().noquote() << counter << "valid footnotes found (" + QString::number(invalidCounter) + " invalid)";

    emit loadingProgressUpdated(totalCount);

    projectSettings()->setJson(jsonObject.value("projectSettings").toObject());
    publications()->setJson(jsonObject.value("publications").toObject());
    _loadingFromFile = false;
    _afterLoadingFromFile = true;
}

Busstop *ProjectData::newBusstop(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    Busstop *b = new Busstop(this, id);
    connect(b, &Busstop::changed, this, &ProjectData::onBusstopChanged);
    return b;
}

Busstop *ProjectData::newBusstop(const QJsonObject &obj) {
    Busstop *b = new Busstop(this, obj);
    connect(b, &Busstop::changed, this, &ProjectData::onBusstopChanged);
    return b;
}

Busstop *ProjectData::newBusstop(const Busstop &newBusstop) {
    Busstop *b = new Busstop(newBusstop);
    b->setParent(this);
    connect(b, &Busstop::changed, this, &ProjectData::onBusstopChanged);
    return b;
}

Line *ProjectData::newLine(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    Line *l = new Line(this, id);
    connect(l, &Line::changed, this, &ProjectData::onLineChanged);
    return l;
}

Line *ProjectData::newLine(const QJsonObject &obj) {
    Line *l = new Line(this, obj);
    connect(l, &Line::changed, this, &ProjectData::onLineChanged);
    return l;
}

Line *ProjectData::newLine(const Line &newLine) {
    Line *l = new Line(newLine);
    l->setParent(this);
    connect(l, &Line::changed, this, &ProjectData::onLineChanged);
    return l;
}

Tour *ProjectData::newTour(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    Tour *o = new Tour(this, id);
    connect(o, &Tour::changed, this, &ProjectData::onTourChanged);
    return o;
}

Tour *ProjectData::newTour(const QJsonObject &obj) {
    Tour *o = new Tour(this, obj);
    connect(o, &Tour::changed, this, &ProjectData::onTourChanged);
    return o;
}

Tour *ProjectData::newTour(const Tour &newTour) {
    Tour *o = new Tour(newTour);
    o->setParent(this);
    connect(o, &Tour::changed, this, &ProjectData::onTourChanged);
    return o;
}

Footnote *ProjectData::newFootnote(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    return new Footnote(this, id);
}

Footnote *ProjectData::newFootnote(const QJsonObject &obj) {
    return new Footnote(this, obj);
}

Footnote *ProjectData::newFootnote(const Footnote &newFootnote) {
    Footnote *f = new Footnote(newFootnote);
    f->setParent(this);
    return f;
}

QUndoStack *ProjectData::undoStack() {
    return &_undoStack;
}

bool ProjectData::isLoadingFromFile() const {
    return _loadingFromFile;
}

void ProjectData::onBusstopAdded(Busstop *b) {
    if(_addedBusstops.indexOf(b) == -1)
        _addedBusstops << b;
    _updateTimer->start(0);
}

void ProjectData::onBusstopChanged(Busstop *b) {
    if(_changedBusstops.indexOf(b) == -1)
        _changedBusstops << b;
    _updateTimer->start(0);
}

void ProjectData::onBusstopRemoved(Busstop *b) {
    if(_removedBusstops.indexOf(b) == -1)
        _removedBusstops << b;
    _updateTimer->start(0);
}

void ProjectData::onLineAdded(Line *l) {
    if(_addedLines.indexOf(l) == -1)
        _addedLines << l;
    _updateTimer->start(0);
}

void ProjectData::onLineChanged(Line *l) {
    if(_changedLines.indexOf(l) == -1)
        _changedLines << l;
    _updateTimer->start(0);
}

void ProjectData::onLineRemoved(Line *l) {
    if(_removedLines.indexOf(l) == -1)
        _removedLines << l;
    _updateTimer->start(0);
}

void ProjectData::onTourAdded(Tour *o) {
    if(_addedTours.indexOf(o) == -1)
        _addedTours << o;
    _updateTimer->start(0);
}

void ProjectData::onTourChanged(Tour *o) {
    if(_changedTours.indexOf(o) == -1)
        _changedTours << o;
    _updateTimer->start(0);
}

void ProjectData::onTourRemoved(Tour *o) {
    if(_removedTours.indexOf(o) == -1)
        _removedTours << o;
    _updateTimer->start(0);
}
