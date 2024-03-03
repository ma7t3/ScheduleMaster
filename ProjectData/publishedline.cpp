#include "publishedline.h"

PublishedLine::PublishedLine(QObject *parent, const QString &id, const QString &title, const QString &footer) :
    ProjectDataItem(parent, id), _title(title), _footer(footer) {}

PublishedLine::PublishedLine(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

PublishedLine::PublishedLine(const PublishedLine &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

PublishedLine PublishedLine::operator=(const PublishedLine &other) {
    copy(other);
    return *this;
}

void PublishedLine::copy(const PublishedLine &other) {
    ProjectDataItem::copy(other);

    setFilePath(other.filePath());
    setTitle(other.title());
    setFooter(other.footer());
    setDayTypes(other.dayTypes());

    QList<PublishedLineDirection *> newDirections;
    for(int i = 0; i < other.directionCount(); i++) {
        PublishedLineDirection *ld = other.directionAt(i);
        if(hasDirection(ld->id())) {
            PublishedLineDirection *currentLd = direction(ld->id());
            *currentLd = *ld;
            newDirections << currentLd;
        } else {
            newDirections << ld;
        }
    }
    setDirections(newDirections);
}

void PublishedLine::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setFilePath(jsonObject.value("filePath").toString(""));
    setTitle(jsonObject.value("title").toString(tr("Unnamed Published Line")));
    setFooter(jsonObject.value("footer").toString(""));

    QJsonArray jDirections = jsonObject.value("directions").toArray();
    for(int i = 0; i < jDirections.count(); ++i)
        addDirection(new PublishedLineDirection(this, jDirections.at(i).toObject()));
}


QJsonObject PublishedLine::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.remove("id");
    jsonObject.insert("filePath", filePath());
    jsonObject.insert("title", title());
    jsonObject.insert("footer", footer());

    QJsonArray jDirections;
    //QJsonArray jDayTypes;

    for (int i = 0; i < directionCount(); ++i)
        jDirections.append(directionAt(i)->toJson());

    jsonObject.insert("directions", jDirections);

    return jsonObject;
}


QString PublishedLine::title() const {
    return _title;
}

void PublishedLine::setTitle(const QString &newTitle) {
    _title = newTitle;
}

QString PublishedLine::filePath() const {
    return _filePath;
}

void PublishedLine::setFilePath(const QString &newFilePath) {
    _filePath = newFilePath;
}

QString PublishedLine::footer() const {
    return _footer;
}

void PublishedLine::setFooter(const QString &newSubTitle) {
    _footer = newSubTitle;
}

QList<PublishedLineDirection *> PublishedLine::directions() const {
    return _directions;
}

int PublishedLine::directionCount() const {
    return _directions.count();
}

PublishedLineDirection *PublishedLine::direction(const QString &id) const {
    for (int i = 0; i < directionCount(); ++i)
        if(directionAt(i)->id() == id)
            return directionAt(i);

    return nullptr;
}

PublishedLineDirection *PublishedLine::directionAt(const int &index) const {
    if(index < 0 || index >= directionCount())
        return nullptr;

    return _directions[index];
}

bool PublishedLine::hasDirection(const QString &id) const {
    for (int i = 0; i < directionCount(); ++i)
        if(directionAt(i)->id() == id)
            return true;

    return false;
}

void PublishedLine::setDirections(const QList<PublishedLineDirection *> &newDirections) {
    _directions = newDirections;
}

void PublishedLine::addDirection(PublishedLineDirection *direction) {
    if(!direction)
        return;

    _directions << direction;
}

void PublishedLine::removeDirection(PublishedLineDirection *direction) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i) == direction)
            _directions.remove(i);
}

void PublishedLine::removeDirection(const QString &id) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            _directions.remove(i);
}


QList<DayType *> PublishedLine::dayTypes() const {
    return _dayTypes;
}

int PublishedLine::dayTypeCount() const {
    return _dayTypes.count();
}

bool PublishedLine::hasDayType(DayType *dt) {
    for(int i = 0; i < dayTypeCount(); i++)
        if(_dayTypes[i] == dt)
            return true;

    return false;
}

void PublishedLine::setDayTypes(const QList<DayType *> &newDayTypes) {
    _dayTypes = newDayTypes;
}

void PublishedLine::addDayType(DayType *dt) {

    bool alreadyAdded = false;
    for(int i = 0; i < dayTypeCount(); i++)
        if(_dayTypes[i] == dt)
            alreadyAdded = true;

    if(!alreadyAdded)
        _dayTypes << dt;
}

void PublishedLine::removeDayType(DayType *dt) {
    for(int i = 0; i < dayTypeCount(); i++)
        if(_dayTypes[i] == dt)
            _dayTypes.remove(i);
}
