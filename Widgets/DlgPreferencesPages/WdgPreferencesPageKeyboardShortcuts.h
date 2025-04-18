#ifndef WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
#define WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H

#include <QWidget>

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPageKeyboardShortcuts;
}

class WdgPreferencesPageKeyboardShortcuts : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageKeyboardShortcuts(QWidget *parent = nullptr);
    ~WdgPreferencesPageKeyboardShortcuts();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private:
    Ui::WdgPreferencesPageKeyboardShortcuts *ui;
};

#endif // WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
