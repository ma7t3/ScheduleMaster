#ifndef WDGPREFERENCESPAGEAPPEARANCE_H
#define WDGPREFERENCESPAGEAPPEARANCE_H

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

class StylesModel;
class IconSetsModel;

namespace Ui {
class WdgPreferencesPageAppearance;
}

class WdgPreferencesPageAppearance : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageAppearance(QWidget *parent);
    ~WdgPreferencesPageAppearance();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

public slots:
    void setStyleIndex(const int &index);

protected slots:
    void onStyleChanged(const int &index);
    void onIconSetChanged(const int &index);
    void onColorSchemeChanged(const Qt::ColorScheme &colorScheme);
    void onAccentColorChanged(const QString &id);
    void onFontChanged(const QFont &font);

signals:
    void styleIndexChanged(int index);

private:
    Ui::WdgPreferencesPageAppearance *ui;

    StylesModel *_stylesModel;
    IconSetsModel *_iconSetsModel;
};

#endif // WDGPREFERENCESPAGEAPPEARANCE_H
