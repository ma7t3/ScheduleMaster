#ifndef WDGPREFERENCESPAGEUPDATES_H
#define WDGPREFERENCESPAGEUPDATES_H

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPageUpdates;
}

class WdgPreferencesPageUpdates : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageUpdates(QWidget *parent);
    ~WdgPreferencesPageUpdates();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private:
    Ui::WdgPreferencesPageUpdates *ui;
};

#endif // WDGPREFERENCESPAGEUPDATES_H
