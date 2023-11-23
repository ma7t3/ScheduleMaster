#include "publishedline.h"

PublishedLine::PublishedLine(const QString &id, const QString &title, const QString &footer) :
    AbstractProjectDataItem(id),
    m_title(title),
    m_footer(footer)
    //m_description(description)
{
}

QString PublishedLine::filePath() const {
    return m_filePath;
}

void PublishedLine::setFilePath(const QString &newFilePath) {
    m_filePath = newFilePath;
}

QString PublishedLine::title() const {
    return m_title;
}

void PublishedLine::setTitle(const QString &newTitle) {
    m_title = newTitle;
}

QString PublishedLine::footer() const {
    return m_footer;
}

void PublishedLine::setFooter(const QString &newSubTitle) {
    m_footer = newSubTitle;
}

QList<PublishedLineDirection *> PublishedLine::directions() const {
    return m_directions;
}

int PublishedLine::directionCount() const {
    return m_directions.count();
}

PublishedLineDirection *PublishedLine::directionAt(const int &index) {
    if(index < 0 || index >= directionCount())
        return nullptr;

    return m_directions[index];
}

void PublishedLine::setDirections(const QList<PublishedLineDirection *> &newDirections) {
    m_directions = newDirections;
}

void PublishedLine::addDirection(PublishedLineDirection *direction) {
    if(!direction)
        return;

    m_directions << direction;
}

void PublishedLine::removeDirection(PublishedLineDirection *direction) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i) == direction)
            m_directions.remove(i);
}

void PublishedLine::removeDirection(const QString &id) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            m_directions.remove(i);
}


QList<DayType *> PublishedLine::dayTypes() const {
    return m_dayTypes;
}

int PublishedLine::dayTypeCount() const {
    return m_dayTypes.count();
}

bool PublishedLine::hasDayType(DayType *dt) {
    for(int i = 0; i < dayTypeCount(); i++)
        if(m_dayTypes[i] == dt)
            return true;

    return false;
}

void PublishedLine::setDayTypes(const QList<DayType *> &newDayTypes) {
    m_dayTypes = newDayTypes;
}

void PublishedLine::addDayType(DayType *dt) {

    bool alreadyAdded = false;
    for(int i = 0; i < dayTypeCount(); i++)
        if(m_dayTypes[i] == dt)
            alreadyAdded = true;

    if(!alreadyAdded)
        m_dayTypes << dt;
}

void PublishedLine::removeDayType(DayType *dt) {
    for(int i = 0; i < dayTypeCount(); i++)
        if(m_dayTypes[i] == dt)
            m_dayTypes.remove(i);
}

void PublishedLine::overwrite(const PublishedLine &other) {
    setFilePath(other.filePath());
    setTitle(other.title());
    setFooter(other.footer());
    setDayTypes(other.dayTypes());
}









