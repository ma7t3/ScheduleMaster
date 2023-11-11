#include "ProjectData/busstop.h"

Busstop::Busstop(QString id, QString name, bool important) :
    AbstractProjectDataItem(id),
    m_name(name),
    m_important(important)

{}

QString Busstop::name() { return m_name; }

bool Busstop::isImportant()
{
    return m_important;
}

bool Busstop::important() { return isImportant(); }

void Busstop::setName(QString name) { m_name = name; }

void Busstop::setImportant(bool important) { m_important = important; }
