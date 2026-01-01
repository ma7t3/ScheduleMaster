#include "StyleRepository.h"

namespace ScheduleMaster::Core {

StyleRepository::StyleRepository(QObject *parent, const QString &dummy) : GlobalConfigRepositoryCRTP(parent, "Styles") {
    Q_UNUSED(dummy)
    loadAccentColors();
}

QColor StyleRepository::accentColor(const QString &accentColorID) const {
    return _accentColors.value(accentColorID, QColor());
}

QMap<QString, QColor> StyleRepository::accentColors() const {
    return _accentColors;
}

bool StyleRepository::accentColorExists(const QString &accentColorID) const {
    return _accentColors.contains(accentColorID);
}

void StyleRepository::loadAccentColors() {
    const QJsonDocument doc = GlobalConfigRepository::loadSingleConfigResource("accentColors");
    const QJsonObject object = doc.object();
    const QStringList keys = object.keys();
    for(const QString &key : keys)
        _accentColors.insert(key, QColor(object[key].toString()));
}

}
