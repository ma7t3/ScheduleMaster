#ifndef HELPERS_H
#define HELPERS_H

#include <QJsonValue>
#include <QKeySequence>

namespace ScheduleMaster {
    QKeySequence parseKeyboardShortcutConfigString(const QJsonValue &value);
}

#endif // HELPERS_H
