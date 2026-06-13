#ifndef WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
#define WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H

#include "namespace.h"
#include "ui/widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace ScheduleMaster::UI::Models {
class KeyboardShortcutsSortFilterProxyModel;
class KeyboardShortcutsModel;
}

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

protected slots:
    void onCurrentIndexChanged(const QModelIndex &current, const QModelIndex &previous);

    void onRestoreDefaultShortcut();
    void onRemoveShortcut();
    void onCopyID();

    void onShortcutChanged(const QKeySequence &shortcut);

    void onImport();
    void onExport();
    void onResetAll();

private:
    Ui::WdgPreferencesPageKeyboardShortcuts *ui;

    UIMO::KeyboardShortcutsSortFilterProxyModel *_sortFilterProxyModel;
    UIMO::KeyboardShortcutsModel *_model;

    QAction *_restoreDefaultShortcutAction, *_removeShortcutAction, *_copyIDAction, *_showOnlyModifiedAction, *_importAction, *_exportAction, *_resetAllAction, *_focusSearchAction;
};

#endif // WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
