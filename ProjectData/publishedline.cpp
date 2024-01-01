#include "publishedline.h"

PublishedLine::PublishedLine(const QString &id, const QString &title, const QString &footer) :
    AbstractProjectDataItem(id),
    _title(title),
    _footer(footer)
    //_description(description)
{
}

QString PublishedLine::filePath() const {
    return _filePath;
}

void PublishedLine::setFilePath(const QString &newFilePath) {
    _filePath = newFilePath;
}

QString PublishedLine::title() const {
    return _title;
}

void PublishedLine::setTitle(const QString &newTitle) {
    _title = newTitle;
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

PublishedLineDirection *PublishedLine::directionAt(const int &index) {
    if(index < 0 || index >= directionCount())
        return nullptr;

    return _directions[index];
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

void PublishedLine::overwrite(const PublishedLine &other) {
    setFilePath(other.filePath());
    setTitle(other.title());
    setFooter(other.footer());
    setDayTypes(other.dayTypes());
}









