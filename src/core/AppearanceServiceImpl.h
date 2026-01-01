#ifndef APPEARANCESERVICEIMPL_H
#define APPEARANCESERVICEIMPL_H

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "src/api/IAppearanceService.h"
#include "StyleRepository.h"

namespace ScheduleMaster::Core {

class AppearanceServiceImpl :
    public GlobalConfigServiceCRTP<StyleRepository, AppearanceServiceImpl>, public IAppearanceService {
    Q_OBJECT

public:
    explicit AppearanceServiceImpl(QObject *parent);

    void applyAppearance();

    virtual QColor accentColor(const QString &accentColorID) const override;
    virtual QMap<QString, QColor> accentColors() const override;

    virtual QString currentAccentColorID() const override;
    virtual QColor currentAccentColor() const override;
    virtual void setCurrentAccentColor(const QString &accentColorID) override;

    void previewAccentColor(const QString &accentColorID);
    void endAccentColorPreview();
    bool isAccentColorPreviewEnabled() const;

    virtual QList<StyleConfig> styles() const override;
    virtual QStringList styleIDs() const override;
    virtual StyleConfig style(const QString &id) const override;
    virtual QString currentStyleID() const override;
    virtual QStyle *currentStyle() const override;
    virtual void setCurrentStyle(const QString &styleID) override;
    virtual bool registerStyle(StyleConfig styleConfig) override;

    void previewStyle(const QString &styleID);
    void endStylePreview();
    bool isStylePreviewEnabled() const;

    virtual Qt::ColorScheme currentColorScheme() const override;
    virtual void setCurrentColorScheme(const Qt::ColorScheme &colorScheme) override;

    void previewColorScheme(const Qt::ColorScheme &colorScheme);
    void endColorSchemePreview();
    bool isColorSchemePreviewEnabled() const;

    virtual QString currentUiFontFamily() const override;
    virtual void setCurrentUiFontFamily(const QString &fontFamily) override;

    void previewUiFontFamily(const QString &fontFamily);
    void endUiFontFamilyPreview();
    bool isUiFontFamilyPreviewEnabled() const;

    static Qt::ColorScheme colorSchemeFromVariant(const QVariant &value);
    static Qt::ColorScheme colorSchemeFromInt(const int &value);

protected:
    void applyStyle(const QString &id);
    void applyColorScheme(const Qt::ColorScheme &colorScheme);
    void applyAccentColor(const QString &id);
    void applyFont(const QString &fontFamily);
    void applyPalette();

signals:
    void currentStyleChanged(const QString &styleID);
    void currentColorSchemeChanged(const Qt::ColorScheme &colorScheme);
    void currentAccentColorChanged(const QString &accentColorID);
    void currentUiFontFamilyChanged(const QString &fontFamily);

private:
    QPalette _initPalette;
    StyleConfig _currentStyle;
    QString _currentAccentColor;

    QString _previewStyleID, _previewAccentColorID, _previewUiFontFamily;
    int _previewColorScheme;
};

} // namespace ScheduleMaster::Core

#endif // APPEARANCESERVICEIMPL_H
