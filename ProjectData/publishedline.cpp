#include "publishedline.h"

PublishedLine::PublishedLine(const QString &id, const QString &title, const QString &description, const QString &subTitle) :
    AbstractProjectDataItem(id),
    m_title(title),
    m_subTitle(subTitle)
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

QString PublishedLine::subTitle() const {
    return m_subTitle;
}

void PublishedLine::setSubTitle(const QString &newSubTitle) {
    m_subTitle = newSubTitle;
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

void PublishedLine::setNew(const PublishedLine newPublishedLine) {
    setFilePath(newPublishedLine.filePath());
    setTitle(newPublishedLine.title());
    setSubTitle(newPublishedLine.subTitle());
}











