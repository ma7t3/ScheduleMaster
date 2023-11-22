#include "DlgFileHandler.h"
#include "ui_DlgFileHandler.h"

#include "App/global.h"

DlgFileHandler::DlgFileHandler(QWidget *parent, ProjectData *projectData) :
    QDialog(parent),
    ui(new Ui::DlgFileHandler),
    projectData(projectData)
{
    ui->setupUi(this);

    setModal(true);
}

DlgFileHandler::~DlgFileHandler()
{
    delete ui;
}

bool DlgFileHandler::readFromFile(QString filePath) {
    ui->buttonBox->setEnabled(false);

    qApp->processEvents();

    noErrors = true;

    QFile f(filePath);

    if(!f.exists()) {
        noErrors = false;
        logCritical(tr("couldn't load project - file not found: ") + filePath);
        ui->buttonBox->setEnabled(true);
        return false;
    }

    if(!f.open(QIODevice::ReadOnly)) {
        noErrors = false;
        logCritical(tr("couldn't load project - couldn't open file: ") + filePath);
        ui->buttonBox->setEnabled(true);
        return false;
    }

    QTextStream s(&f);
    s.setEncoding(QStringConverter::Utf8);

    logInfo(tr("reading file..."));
    QString jsonStr = s.readAll();
    jsonStr = jsonStr.remove(fileHeader);
    QByteArray br = jsonStr.toUtf8();
    QJsonDocument jDoc = QJsonDocument::fromJson(br);

    if(!jDoc.isObject()) {
        noErrors = false;
        logCritical(tr("couldn't load project - file is not a valid json object"));
        ui->buttonBox->setEnabled(true);
        return false;
    }

    logInfo(tr("read file - %1 characters").arg(jsonStr.length()));

    QJsonObject jMainObj = jDoc.object();

    logInfo(tr("loading project settings..."));
    if(jMainObj.contains("projectSettings") && jMainObj.find("projectSettings")->isObject())
        loadProjectSettings(jMainObj.find("projectSettings")->toObject());
    else { // Version 0.9.0 and older
        logWarning(tr("no project settings found - loading defaults..."));
        projectData->projectSettings()->addDayType(new DayType(global::getNewID(), tr("Monday-Friday"), DayType::MonFri));
        projectData->projectSettings()->addDayType(new DayType(global::getNewID(), tr("Saturday"), DayType::Sat));
        projectData->projectSettings()->addDayType(new DayType(global::getNewID(), tr("Sunday & Holiday"), DayType::Sun));
    }

    if(jMainObj.contains("busstops") && jMainObj.find("busstops")->isArray()) {
        logInfo(tr("loading busstops..."));
        loadBusstops(jMainObj.find("busstops")->toArray());
    } else {
        noErrors = false;
        logWarning(tr("no busstops found"));
    }

    if(jMainObj.contains("lines") && jMainObj.find("lines")->isArray()) {
        logInfo(tr("loading lines..."));
        loadLines(jMainObj.find("lines")->toArray());
    } else {
        noErrors = false;
        logWarning(tr("no lines found"));
    }

    if(jMainObj.contains("tours") && jMainObj.find("tours")->isArray()) {
        logInfo(tr("loading tours..."));
        loadTours(jMainObj.find("tours")->toArray());
    } else {
        noErrors = false;
        logWarning(tr("no tours found"));
    }

    if(jMainObj.contains("publications") && jMainObj.find("publications")->isObject()) {
        logInfo(tr("loading publications..."));
        loadPublications(jMainObj.find("publications")->toObject());
    } else {
        noErrors = false;
        logWarning(tr("no publications found"));
    }

    f.close();

    logSuccess("finished loading project!");

    if(noErrors)
        close();

    ui->buttonBox->setEnabled(true);
    qApp->processEvents();

    return true;
}

void DlgFileHandler::loadBusstops(QJsonArray jBusstops)
{
    for(int i = 0; i < jBusstops.count(); i++) {
        if(!jBusstops.at(i).isObject()) {
            noErrors = false;
            logError(tr("busstop at index %1 is invalid - skipped").arg(i));
            continue;
        }

        QJsonObject jObj = jBusstops.at(i).toObject();

        if(!jObj.contains("id") || !jObj.find("id")->isString()) {
            noErrors = false;
            logWarning(tr("busstop no. %1 - no valid id found").arg(i));
        }
        if(!jObj.contains("name") || !jObj.find("name")->isString()) {
            noErrors = false;
            logWarning(tr("busstop no. %1 - no valid name found").arg(i));
        }

        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "unnamed";
        bool important = jObj.contains("important") ? jObj.find("important")->toBool() : false;

        Busstop *b = new Busstop(id, name, important);
        projectData->addBusstop(b);
    }
}

void DlgFileHandler::loadDirections(Line *l, QJsonArray jArr) {
    for(int i = 0; i < jArr.count(); i++) {
        QJsonObject jObj = jArr[i].toObject();
        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString description = jObj.contains("description") ? jObj.find("description")->toString() : QString::number(i);
        LineDirection *ld = new LineDirection(id, description);
        l->addDirection(ld);
    }
}

void DlgFileHandler::loadLines(QJsonArray jLines)
{
    for(int i = 0; i < jLines.count(); i++) {
        if(!jLines.at(i).isObject())
            continue;

        QJsonObject jObj = jLines.at(i).toObject();

        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "unnamed";
        QString description = jObj.contains("description") ? jObj.find("description")->toString() : "";
        QColor color = jObj.contains("color") ? QColor(jObj.find("color")->toString()) : QColor(0, 0, 0);

        Line *l = new Line(id, name, description, color);
        projectData->addLine(l);

        if(jObj.contains("directions")) {
            loadDirections(l, jObj.find("directions")->toArray());
        } else {
            l->addDirection(new LineDirection(global::getNewID(), "Forward"));
            l->addDirection(new LineDirection(global::getNewID(), "Reverse"));
        }

        if(jObj.contains("routes") && jObj.find("routes")->isArray())
            loadRoutes(jObj.find("routes")->toArray(), l);

        QJsonArray trips;
        if(jObj.contains("trips")) {
            trips = jObj.find("trips")->toArray();
        } else {
            if(jObj.contains("forwardTrips")) {
                QJsonArray tmpArr = (jObj.find("forwardTrips")->toArray());
                for(int j = 0; j < tmpArr.count(); j++)
                    trips.append(tmpArr[j]);
            }
            if(jObj.contains("reverseTrips")) {
                QJsonArray tmpArr = (jObj.find("reverseTrips")->toArray());
                for(int j = 0; j < tmpArr.count(); j++)
                    trips.append(tmpArr[j]);
            }
        }

        for(int j = 0; j < trips.count(); j++)
            loadTrip(trips[j].toObject(), l);
    }
}

void DlgFileHandler::loadRoutes(QJsonArray jRoutes, Line *l)
{
    for(int i = 0; i < jRoutes.count(); i++) {
        if(!jRoutes.at(i).isObject())
            continue;

        QJsonObject jObj = jRoutes.at(i).toObject();

        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        int code = jObj.contains("code") ? jObj.find("code")->toInt() : 01;
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "unnamed";

        LineDirection *direction;
        if(!jObj.contains("direction")) {
            direction = l->directionAt(0);
        } else {
            if(jObj.find("direction")->isBool()) {
                bool boolDirection = jObj.find("direction")->toBool();
                if(boolDirection) {
                    direction = l->directionAt(0);
                } else {
                    direction = l->directionAt(1);
                }
            } else {
                QString directionId = jObj.find("direction")->toString();
                direction = l->direction(directionId);
                if(!direction)
                    continue;
            }
        }

        Route *r = new Route(id, code, name, direction);
        l->addRoute(r);

        if(jObj.contains("busstops") && jObj.find("busstops")->isArray()) {
            QJsonArray jArr = jObj.find("busstops")->toArray();
            for(int j = 0; j < jArr.count(); j++) {
                if(!jArr[j].isString())
                    continue;

                QString id = jArr[j].toString();
                Busstop * b = projectData->busstop(id);
                if(!b)
                    continue;

                r->addBusstop(b);
            }
        }

        if(jObj.contains("timeProfiles") && jObj.find("timeProfiles")->isArray())
            loadProfiles(jObj.find("timeProfiles")->toArray(), r);
    }
}

void DlgFileHandler::loadProfiles(QJsonArray jProfiles, Route *r)
{
    for(int i = 0; i < jProfiles.count(); i++) {
        if(!jProfiles.at(i).isObject())
            continue;

        QJsonObject jObj = jProfiles.at(i).toObject();

        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "unnamed";
        float duration = jObj.contains("duration") ? jObj.find("duration")->toDouble() : 0;

        TimeProfile *p = new TimeProfile(id, name);
        p->setDuration(duration);
        r->addTimeProfile(p);

        if(jObj.contains("items") && jObj.find("items")->isArray()) {
            QJsonArray jArr = jObj.find("items")->toArray();

            for(int j = 0; j < jArr.count(); j++) {
                if(!jArr[j].isObject())
                    continue;

                QJsonObject jSObj = jArr[j].toObject();

                QString busstopID = jSObj.contains("busstopID") ? jSObj.find("busstopID")->toString() : global::getNewID();
                float dep = jSObj.contains("dep") ? jSObj.find("dep")->toDouble() : 0;
                float arr = jSObj.contains("arr") ? jSObj.find("arr")->toDouble() : 0;
                bool sepTimes = jSObj.contains("arr");
                int busstopMode = jSObj.contains("busstopMode") ? jSObj.find("busstopMode")->toInt() : 1;

                TimeProfileItem *itm = new TimeProfileItem(busstopID);

                itm->setDepValue(dep);
                if(sepTimes)
                    itm->setArrValue(arr);
                itm->setBusstopMode(busstopMode);
                p->addBusstop(itm);
            }
        }
    }
}

void DlgFileHandler::loadTrip(QJsonObject jObj, Line *l)
{
    if(!jObj.contains("routeID"))
        return;

    QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
    QString routeID = jObj.find("routeID")->toString();
    QString timeProfileID = jObj.contains("timeProfileID") ? jObj.find("timeProfileID")->toString() : "";
    QTime startTime = jObj.contains("startTime") ? QTime::fromMSecsSinceStartOfDay(jObj.find("startTime")->toInt()) : QTime(0, 0, 0, 0);
    int weekDaysCode = jObj.contains("weekdays") ? jObj.find("weekdays")->toInt() : WeekDays::MonFri;

    bool repeat = false;
    if(jObj.contains("repeat")) {
        if(jObj.find("repeat")->isObject())
            repeat = true;
    }

    Route *r = l->route(routeID);
    if(!r)
        return;

    TimeProfile *p = r->timeProfile(timeProfileID);
    if(!p)
        return;

    Trip *t = new Trip(id, r, startTime, p);
    t->weekDays()->setCode(weekDaysCode);
    l->addTrip(t);

    if(repeat) {
        QJsonObject jRepObj = jObj.find("repeat")->toObject();
        QStringList childIDs;
        if(jRepObj.contains("childIDs")) {
            QJsonArray jArrChildIDs = jRepObj.find("childIDs")->toArray();
            for(int i = 0; i < jArrChildIDs.count(); i++) {
                childIDs << jArrChildIDs[i].toString();
            }
        }

        QTime repeatInterval = jRepObj.contains("repeatInterval") ? QTime::fromMSecsSinceStartOfDay(jRepObj.find("repeatInterval")->toInt()) : QTime(0, 10, 0, 0);
        QTime repeatEnd = jRepObj.contains("repeatEnd") ? QTime::fromMSecsSinceStartOfDay(jRepObj.find("repeatEnd")->toInt()) : startTime;

        t->setRepeat(true);
        t->setChildIdList(childIDs);
        t->setRepeatInterval(repeatInterval);
        t->setRepeatEnd(repeatEnd);
    } else {
        t->setRepeat(false);
    }
}

void DlgFileHandler::loadTours(QJsonArray jArr)
{
    for(int i = 0; i < jArr.count(); i++) {
        QJsonObject jObj = jArr[i].toObject();

        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "new tour";
        int weekDays = jObj.contains("weekdays") ? jObj.find("weekdays")->toInt() : WeekDays::MonFri;

        Tour *o = new Tour(id, name, WeekDays(weekDays));
        projectData->addTour(o);

        if(jObj.contains("trips") && jObj.find("trips")->isArray()) {
            QJsonArray jArrTrips = jObj.find("trips")->toArray();
            for(int j = 0; j < jArrTrips.count(); j++) {
                QString tripID = jArrTrips.at(j).toString();
                Trip *t = projectData->trip(tripID);
                if(t)
                    o->addTrip(t);
                else
                    qDebug() << "123 not found!!!";
            }
        }
    }
}

void DlgFileHandler::loadPublications(QJsonObject jObj) {
    if(jObj.contains("lineSchedules") && jObj.find("lineSchedules")->isArray()) {
        QJsonArray jLineSchedules = jObj.find("lineSchedules")->toArray();

        for(int i = 0; i < jLineSchedules.count(); i++) {
            QJsonObject jCurrentLine = jLineSchedules.at(i).toObject();

            QString id = jCurrentLine.contains("id") ? jCurrentLine.find("id")->toString() : global::getNewID();
            QString title = jCurrentLine.contains("title") ? jCurrentLine.find("title")->toString() : "unnamed";
            QString footer = jCurrentLine.contains("footer") ? jCurrentLine.find("footer")->toString() : "";
            QString filePath = jCurrentLine.contains("filePath") ? jCurrentLine.find("filePath")->toString() : "";

            PublishedLine *l = new PublishedLine(id, title, footer);
            l->setFilePath(filePath);

            QJsonArray jDirections = jCurrentLine.find("directions")->toArray();

            for(int j = 0; j < jDirections.count(); j++) {
                QJsonObject jCurrentDirection = jDirections.at(j).toObject();
                QString id = jCurrentDirection.contains("id") ? jCurrentDirection.find("id")->toString() : global::getNewID();
                QString name = jCurrentDirection.contains("name") ? jCurrentDirection.find("name")->toString() : "";

                PublishedLineDirection *ld = new PublishedLineDirection(id, name);

                QJsonArray jBusstops = jCurrentDirection.find("busstops")->toArray();
                QJsonArray jRoutes = jCurrentDirection.find("routes")->toArray();

                for(int k = 0; k < jBusstops.count(); k++) {
                    QJsonObject jCurrentBusstop = jBusstops.at(k).toObject();

                    if(!jCurrentBusstop.contains("busstopID"))
                        continue;

                    Busstop *referenceBusstop = projectData->busstop(jCurrentBusstop.find("busstopID")->toString());
                    if(!referenceBusstop)
                        continue;

                    QString id = jCurrentBusstop.contains("id") ? jCurrentBusstop.find("id")->toString() : global::getNewID();
                    QString label = jCurrentBusstop.contains("label") ? jCurrentBusstop.find("label")->toString() : "";
                    bool joinWithPrevious = jCurrentBusstop.contains("joinWithPrevious") ? jCurrentBusstop.find("joinWithPrevious")->toBool() : false;
                    bool showArrAndDep = jCurrentBusstop.contains("showArrAndDepevious") ? jCurrentBusstop.find("showArrAndDep")->toBool() : false;
                    bool showDivider = jCurrentBusstop.contains("showDivider") ? jCurrentBusstop.find("showDivider")->toBool() : false;

                    PublishedBusstop *b = new PublishedBusstop(id, referenceBusstop, label);
                    b->joinWithPrevious(joinWithPrevious);
                    b->setShowArrAndDep(showArrAndDep);
                    b->setShowDivider(showDivider);

                    ld->addBusstop(b);
                }

                for(int k = 0; k < jRoutes.count(); k++) {
                    if(!jRoutes.at(k).isString())
                        return;

                    QString routeID = jRoutes.at(k).toString();
                    Route *r = projectData->route(routeID);
                    if(!r)
                        return;

                    ld->addRoute(r);
                }

                l->addDirection(ld);
            }

            projectData->publications()->addLine(l);
        }
    }
}

QJsonObject DlgFileHandler::projectSettingsToJson(ProjectSettings *projectSettings) {
    QJsonObject jObj;
    jObj.insert("displayName", projectSettings->displayName());
    jObj.insert("shortName", projectSettings->shortName());
    jObj.insert("icon", projectSettings->icon().fileName());

    QJsonArray jArrDayTypes;
    for(int i = 0; i < projectSettings->dayTypeCount(); i++)
        jArrDayTypes.append(dayTypeToJson(projectSettings->dayTypeAt(i)));

    jObj.insert("dayTypes", jArrDayTypes);

    return jObj;
}

QJsonObject DlgFileHandler::dayTypeToJson(DayType *dayType) {
    QJsonObject jObj;
    jObj.insert("id", dayType->id());
    jObj.insert("name", dayType->name());
    jObj.insert("code", dayType->toCode());
    return jObj;
}

QJsonObject DlgFileHandler::directionToJson(LineDirection *ld) {
    QJsonObject jObj;

    jObj.insert("id", ld->id());
    jObj.insert("description", ld->description());
    return jObj;
}

QJsonObject DlgFileHandler::publicationsToJson(Publications *publications) {
    QJsonObject jObj;

    QJsonArray jLineSchedules;

    for(int i = 0; i < publications->lineCount(); i++) {
        PublishedLine *l = publications->lineAt(i);
        QJsonObject jCurrentLine;
        jCurrentLine.insert("filePath", l->filePath());
        jCurrentLine.insert("title", l->title());
        jCurrentLine.insert("footer", l->footer());

        QJsonArray jDirections;

        for(int j = 0; j < l->directionCount(); j++) {
            PublishedLineDirection *ld = l->directionAt(j);
            QJsonObject jCurrentLineDirection;
            jCurrentLineDirection.insert("name", ld->name());

            QJsonArray jBusstops;
            QJsonArray jRoutes;

            for(int k = 0; k < ld->routeCount(); k++)
                jRoutes.append(ld->routeAt(k)->id());

            for(int k = 0; k < ld->busstopCount(); k++) {
                PublishedBusstop *b = ld->busstopAt(k);

                QJsonObject jCurrentBusstop;

                jCurrentBusstop.insert("busstopID", b->linkedBusstop()->id());
                jCurrentBusstop.insert("id", b->id());
                jCurrentBusstop.insert("label", b->label());
                jCurrentBusstop.insert("joinWithPrevious", b->isJoinedWithPrevious());
                jCurrentBusstop.insert("showArrAndDep", b->showArrAndDep());
                jCurrentBusstop.insert("showDivider", b->showDivider());

                jBusstops.append(jCurrentBusstop);
            }

            jCurrentLineDirection.insert("busstops", jBusstops);
            jCurrentLineDirection.insert("routes", jRoutes);

            jDirections.append(jCurrentLineDirection);
        }

        jCurrentLine.insert("directions", jDirections);
        jLineSchedules.append(jCurrentLine);
    }

    jObj.insert("lineSchedules", jLineSchedules);

    return jObj;
}

void DlgFileHandler::logInfo(const QString &text) {
    QIcon icon(":/main/icons/info.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logSuccess(const QString &text) {
    QIcon icon(":/main/icons/success.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logWarning(const QString &text) {
    QIcon icon(":/main/icons/warning.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logError(const QString &text) {
    QIcon icon(":/main/icons/error.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}

void DlgFileHandler::logCritical(const QString &text) {
    QIcon icon(":/main/icons/error.ico");

    QTreeWidgetItem *itm = new QTreeWidgetItem({text});
    itm->setForeground(0, Qt::red);
    itm->setIcon(0, icon);

    ui->log->addTopLevelItem(itm);
    qApp->processEvents();
}
bool DlgFileHandler::saveToFile(QString filePath)
{
    QJsonObject jFileInfo;

    jFileInfo.insert("primaryVersion", global::primaryVersion);
    jFileInfo.insert("secondaryVersion", global::secondaryVersion);
    jFileInfo.insert("tertiaryVersion", global::tertiaryVersion);
    jFileInfo.insert("releaseType", global::releaseType);

    QJsonObject jProjectSettings = projectSettingsToJson(projectData->projectSettings());

    QJsonArray jBusstops;

    for(int i = 0; i < projectData->busstopCount(); i++) {
        Busstop *b = projectData->busstopAt(i);

        QString id = b->id();
        QString name = b->name();
        bool important = b->isImportant();

        QJsonObject jObj;
        jObj.insert("id", id);
        jObj.insert("name", name);
        jObj.insert("important", important);

        jBusstops << jObj;
    }

    QJsonArray jLines;

    for(int i = 0; i < projectData->lineCount(); i++) {
        QJsonObject jObj;

        Line *l = projectData->lineAt(i);

        QString id = l->id();
        QString name = l->name();
        QString description = l->description();
        QString colorName = l->color().name(QColor::HexRgb);

        QJsonArray directions;
        QJsonArray routes;
        QJsonArray trips;

        for(int j = 0; j < l->routeCount(); j++) {
            routes.append(routeToJson(l->routeAt(j)));
        }

        for(int j = 0; j < l->tripCount(); j++) {
            trips.append(tripToJson(l->tripAt(j)));
        }

        for(int j = 0; j < l->directionCount(); j++) {
            directions.append(directionToJson(l->directionAt(j)));
        }

        jObj.insert("id", id);
        jObj.insert("name", name);
        jObj.insert("description", description);
        jObj.insert("color", colorName);
        jObj.insert("directions", directions);
        jObj.insert("routes", routes);
        jObj.insert("trips", trips);

        jLines.append(jObj);
    }

    QJsonArray jTours;

    for(int i = 0; i < projectData->tourCount(); i++) {
        Tour *o = projectData->tourAt(i);
        jTours.append(tourToJson(o));
    }

    QJsonObject jPublications = publicationsToJson(projectData->publications());

    QJsonObject jMainObj;
    jMainObj.insert("projectSettings", jProjectSettings);
    jMainObj.insert("busstops", jBusstops);
    jMainObj.insert("lines", jLines);
    jMainObj.insert("tours", jTours);
    jMainObj.insert("publications", jPublications);
    jMainObj.insert("_fileInfo", jFileInfo);

    QJsonDocument jDoc;
    jDoc.setObject(jMainObj);

    QString jsonString = jDoc.toJson(QJsonDocument::Indented);

    // write to file

    QFile f(filePath);

    if(!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream s(&f);
    s.setEncoding(QStringConverter::Utf8);

    s << fileHeader;
    s << jsonString;

    f.close();

    return true;
}

void DlgFileHandler::loadProjectSettings(QJsonObject jObj) {
    QString displayName = jObj.contains("displayName") ? jObj.find("displayName")->toString() : "";
    QString shortName = jObj.contains("shortName") ? jObj.find("shortName")->toString() : "";
    QString iconPath = jObj.contains("icon") ? jObj.find("icon")->toString() : "";

    projectData->projectSettings()->setNames(displayName, shortName);
    projectData->projectSettings()->setIcon(QFile(iconPath));

    if(jObj.contains("dayTypes"))
        loadDayTypes(jObj.find("dayTypes")->toArray());
}

void DlgFileHandler::loadDayTypes(QJsonArray jArr) {
    for(int i = 0; i < jArr.count(); i++) {
        if(!jArr.at(i).isObject())
            continue;

        QJsonObject jObj = jArr.at(i).toObject();
        QString id = jObj.contains("id") ? jObj.find("id")->toString() : global::getNewID();
        QString name = jObj.contains("name") ? jObj.find("name")->toString() : "";
        int code = jObj.contains("code") ? jObj.find("code")->toInt() : DayType::MonFri;
        DayType *dayType = new DayType(id, name, code);
        projectData->projectSettings()->addDayType(dayType);
    }
}

QJsonObject DlgFileHandler::routeToJson(Route *r)
{
    QJsonObject jObj;

    QString id = r->id();
    int code = r->code();
    QString name = r->name();
    QString direction = r->direction()->id();

    QJsonArray busstopList;
    QJsonArray timeProfileList;

    for(int i = 0; i < r->busstopCount(); i++)
        busstopList << r->busstopAt(i)->id();

    for(int i = 0; i < r->profileCount(); i++) {
        timeProfileList << timeProfileToJson(r->timeProfileAt(i));
    }

    jObj.insert("id", id);
    jObj.insert("code", code);
    jObj.insert("name", name);
    jObj.insert("direction", direction);
    jObj.insert("busstops", busstopList);
    jObj.insert("timeProfiles", timeProfileList);

    return jObj;
}

QJsonObject DlgFileHandler::timeProfileToJson(TimeProfile *p)
{
    QJsonObject jObj;

    QString id = p->id();
    QString name = p->name();
    float duration = p->duration();
    QList<TimeProfileItem *> l = p->busstops();

    jObj.insert("id", id);
    jObj.insert("name", name);
    jObj.insert("duration", duration);

    QJsonArray jArr;

    for(int i = 0; i < l.count(); i++)
        jArr << timeProfileItemToJson(l[i]);

    jObj.insert("items", jArr);

    return jObj;
}

QJsonObject DlgFileHandler::timeProfileItemToJson(TimeProfileItem *i)
{
    QJsonObject jObj;

    QString busstopID = i->busstopId();
    bool seperateTimes = i->hasSeperateTimes();
    float arr = i->arrValue();
    float dep = i->depValue();
    int busstopMode = i->busstopMode();

    jObj.insert("busstopID", busstopID);
    if(seperateTimes)
        jObj.insert("arr", arr);
    jObj.insert("dep", dep);
    jObj.insert("busstopMode", busstopMode);

    return jObj;
}


QJsonObject DlgFileHandler::tripToJson(Trip *t)
{
    QJsonObject jObj;

    if(!t)
        return jObj;

    QString id = t->id();
    QString routeID = t->route()->id();
    QString timeProfileID = t->timeProfile()->id();
    int startTime = t->startTime().msecsSinceStartOfDay();
    int weekdays = t->weekDays()->toCode();
    bool repeat = t->hasRepeat();


    jObj.insert("id", id);
    jObj.insert("routeID", routeID);
    jObj.insert("timeProfileID", timeProfileID);
    jObj.insert("startTime", startTime);
    jObj.insert("weekdays", weekdays);


    if(!repeat)
        jObj.insert("repeat", false);
    else {
        QJsonObject jRepObj;
        int repeatInterval = t->repeatInterval().msecsSinceStartOfDay();
        int repeatEnd = t->repeatEnd().msecsSinceStartOfDay();

        QJsonArray jArrChildIDs;

        QStringList childIDs = t->childIDs();
        for(int i = 0; i < childIDs.count(); i++)
            jArrChildIDs.append(childIDs[i]);

        jRepObj.insert("repeatInterval", repeatInterval);
        jRepObj.insert("repeatEnd", repeatEnd);
        jRepObj.insert("childIDs", jArrChildIDs);

        jObj.insert("repeat", jRepObj);
    }

    return jObj;
}

QJsonObject DlgFileHandler::tourToJson(Tour *o)
{
    QJsonObject jObj;

    if(!o)
        return jObj;

    QString id = o->id();
    QString name = o->name();
    int weekDays = o->weekDays()->toCode();

    QJsonArray jArrTrips;
    for(int i = 0; i < o->tripCount(); i++)
        jArrTrips.append(o->tripAt(i)->selfChildId());

    jObj.insert("id", id);
    jObj.insert("name", name);
    jObj.insert("weekdays", weekDays);
    jObj.insert("trips", jArrTrips);

    return jObj;
}


void DlgFileHandler::on_buttonBox_accepted() {
    accept();
}

