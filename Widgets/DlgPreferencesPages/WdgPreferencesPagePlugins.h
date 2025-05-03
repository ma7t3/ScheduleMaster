#ifndef WDGPREFERENCESPAGEPLUGINS_H
#define WDGPREFERENCESPAGEPLUGINS_H

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPluginsPage;
}

class WdgPreferencesPagePlugins : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPagePlugins(QWidget *parent);
    ~WdgPreferencesPagePlugins();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private:
    Ui::WdgPreferencesPluginsPage *ui;
};

#endif // WDGPREFERENCESPAGEPLUGINS_H
