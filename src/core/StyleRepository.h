#ifndef STYLEREPOSITORY_H
#define STYLEREPOSITORY_H

#include "GlobalConfigRepository.h"
#include "src/api/IAppearanceService.h"

namespace ScheduleMaster::Core {

class StyleRepository : public GlobalConfigRepositoryCRTP<StyleConfig> {
public:
    explicit StyleRepository(QObject *parent, const QString &dummy);

    QColor accentColor(const QString &accentColorID) const;
    QMap<QString, QColor> accentColors() const;
    bool accentColorExists(const QString &accentColorID) const;

protected:
    void loadAccentColors();

private:
    QMap<QString, QColor> _accentColors;
};

}

#endif // STYLEREPOSITORY_H
