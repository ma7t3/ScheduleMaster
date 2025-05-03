#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QColor>

#include "GlobalConfigManager.h"

class Style : public GlobalConfigItem {
public:
    Style(const QJsonObject &jsonObject = QJsonObject());
    Style(const QString &id);

    bool supportsColorScheme(const Qt::ColorScheme &) const;
    QList<Qt::ColorScheme> supportedColorSchemes() const;

    enum StyleType {
        SystemDefaultType,
        StyleClassType,
        StyleSheetType,
        StyleFactoryType,
        InvalidType
    };

    QString name, styleClassID, styleSheetUrl, styleFactoryName;
    StyleType type;
    bool lightSupport, darkSupport, accentColorSupport, applyPalette;
};

class StyleManager : public GlobalConfigManager<StyleManager, Style> {
    friend class GlobalConfigManager<StyleManager, Style>;
    Q_OBJECT
public:
    explicit StyleManager(QObject *parent);

    /**
     * @brief Returns the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The accent color
     */
    static QColor accentColor(const QString &id);

    /**
     * @brief Returns a darker version of the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The daker version of the accent color
     */
    static QColor accentColorDark(const QString &id);

    /**
     * @brief Returns a list of all accent colors
     * @return The list of accent colors
     */
    static QMap<QString, QColor> accentColors();

protected:
    static void loadAccentColors();

private:
    static inline QMap<QString, QColor> _accentColors;
};

#endif // STYLEMANAGER_H
