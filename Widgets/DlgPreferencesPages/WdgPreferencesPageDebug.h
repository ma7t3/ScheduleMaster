#ifndef WDGPREFERENCESPAGEDEBUG_H
#define WDGPREFERENCESPAGEDEBUG_H

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPageDebug;
}

class WdgPreferencesPageDebug : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageDebug(QWidget *parent);
    ~WdgPreferencesPageDebug();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private:
    Ui::WdgPreferencesPageDebug *ui;
};

#endif // WDGPREFERENCESPAGEDEBUG_H
