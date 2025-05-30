#include "Global.h"

#include <QStringList>
#include <QColor>
#include <QJsonArray>
#include <QJsonValue>
#include <QKeySequence>

Global::Global() :
    QObject(nullptr) {
}

Global::~Global() {

}

Global *Global::instance() {
    static Global instance;
    return &instance;
}

QColor Global::contrastColor(const QColor &c) {
    return sqrt(
       0.299 * pow(c.redF(), 2) +
       0.587 * pow(c.greenF(), 2) +
       0.114 * pow(c.blueF(), 2)
       ) >= 0.5 ? Qt::black : Qt::white;
}

QJsonArray Global::stringListToJsonArray(const QStringList &list) {
    return QJsonArray::fromStringList(list);
}

QStringList Global::jsonArrayToStringList(const QJsonArray &array) {
    QStringList list;
    for(const QJsonValue &val : array)
        list << val.toString();
    return list;
}

void Global::blockSignalsRecursive(QObject *object, const bool &block) {
    if(!object)
        return;

    object->blockSignals(block);

    const QObjectList children = object->children();
    for (QObject* child : children)
        blockSignalsRecursive(child, block);
}

QKeySequence Global::parseKeyboardShortcutConfigString(const QJsonValue &value) {
    QStringList values;
    QKeySequence resultSequence;

    if(value.isArray())
        values = Global::jsonArrayToStringList(value.toArray());
    else
        values = {value.toString()};

    for(const QString & value : std::as_const(values)) {
        if(value.startsWith("standard:")) {
            QString idPart = value.mid(QString("standard:").length());
            bool ok;
            int standardID = idPart.toInt(&ok);
            if (ok) {
                QKeySequence sequence = QKeySequence(static_cast<QKeySequence::StandardKey>(standardID));
                if(!sequence.toString(QKeySequence::PortableText).isEmpty()) {
                    return sequence;
                } else {
                    resultSequence = QKeySequence();
                }
            }
        }

        QKeySequence sequence(value);
        if (!sequence.toString(QKeySequence::PortableText).isEmpty()) {
            return sequence;
        } else
            resultSequence = QKeySequence();
    }

    return resultSequence;
}
