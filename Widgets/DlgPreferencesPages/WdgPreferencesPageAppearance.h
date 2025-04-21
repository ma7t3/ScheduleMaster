#ifndef WDGPREFERENCESPAGEAPPEARANCE_H
#define WDGPREFERENCESPAGEAPPEARANCE_H

#include <QWidget>

#include "Global/LocalConfig.h"
#include "Global/StyleHandler.h"

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

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


private slots:
    void on_fcbFont_currentFontChanged(const QFont &f);

    void onAccentColorChanged(const QString &id);
    void onColorSchemeChanged(const Qt::ColorScheme &colorScheme);

    void on_cbAppearance_currentIndexChanged(int index);

private:
    Ui::WdgPreferencesPageAppearance *ui;
};

#endif // WDGPREFERENCESPAGEAPPEARANCE_H
