#ifndef IACTIONSERVICE_H
#define IACTIONSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

#include <QKeySequence>

namespace ScheduleMaster {

class ActionConfig : public GlobalConfigItem {
public:
    ActionConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    ActionConfig(const QString &id, const int &index = 0);

    bool canHaveShortcut;
    QString text;
    QString tooltip;
    QString description;
    QString icon;
    QStringList breadcrumb;
    QKeySequence defaultKeyboardShortcut;
};


class SCHEDULEMASTERINTERFACE_EXPORT IActionService {
public:
    virtual ~IActionService() = default;

    // TODO Add Interface
};

}

#endif // IACTIONSERVICE_H
