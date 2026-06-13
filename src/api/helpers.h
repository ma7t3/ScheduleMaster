#ifndef HELPERS_H
#define HELPERS_H

#include <QJsonValue>
#include <QKeySequence>

#include "ScheduleMaster_global.h"

namespace ScheduleMaster {
    SCHEDULEMASTERINTERFACE_EXPORT QKeySequence parseKeyboardShortcutConfigString(const QJsonValue &value);

    SCHEDULEMASTERINTERFACE_EXPORT QVariant convertVariant(const QVariant &value, const QMetaType::Type &type);
    SCHEDULEMASTERINTERFACE_EXPORT QString  convertVariantToString(const QVariant &value);
    SCHEDULEMASTERINTERFACE_EXPORT QVariant convertVariantFromJson(const QJsonValue &value, const QMetaType::Type &type);
    SCHEDULEMASTERINTERFACE_EXPORT QColor   contrastColor(const QColor &color);
}

#endif // HELPERS_H
