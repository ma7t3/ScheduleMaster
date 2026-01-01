#ifndef IAPPEARANCESERVICE_H
#define IAPPEARANCESERVICE_H

#include <QStyle>

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

class StyleConfig : public GlobalConfigItem {
public:
    StyleConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    StyleConfig(const QString &id, const int &index = 0);

    QList<Qt::ColorScheme> supportedColorSchemes() const;
    bool supportsColorScheme(const Qt::ColorScheme &colorScheme) const;

    enum StyleType {
        StyleClassType,
        StyleSheetType,
        StyleFactoryType,
        InvalidType
    };

    QString name, styleClassID, styleSheetUrl, styleFactoryName;
    StyleType type;
    bool lightSupport, darkSupport, accentColorSupport, applyPalette;
};

class SCHEDULEMASTERINTERFACE_EXPORT IAppearanceService {
public:
    virtual ~IAppearanceService() = default;

    virtual QColor accentColor(const QString &accentColorID) const = 0;
    virtual QMap<QString, QColor> accentColors() const = 0;

    virtual QString currentAccentColorID() const = 0;
    virtual QColor currentAccentColor() const = 0;
    virtual void setCurrentAccentColor(const QString &accentColorID) = 0;

    virtual QList<StyleConfig> styles() const = 0;
    virtual QStringList styleIDs() const = 0;
    virtual StyleConfig style(const QString &id) const = 0;
    virtual QString currentStyleID() const = 0;
    virtual QStyle *currentStyle() const = 0;
    virtual void setCurrentStyle(const QString &styleID) = 0;
    virtual bool registerStyle(StyleConfig styleConfig) = 0;

    virtual Qt::ColorScheme currentColorScheme() const = 0;
    virtual void setCurrentColorScheme(const Qt::ColorScheme &colorScheme) = 0;

    virtual QString currentUiFontFamily() const = 0;
    virtual void setCurrentUiFontFamily(const QString &fontFamily) = 0;
};

} // namespace ScheduleMaster

#endif // IAPPEARANCESERVICE_H
