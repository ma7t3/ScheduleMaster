#ifndef WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
#define WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"
#include "ItemModels/KeyboardShortcutsModel.h"
#include "Global/ActionShortcutMapper.h"

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

    KeyboardShortcutsSortFilterProxyModel *_sortFilterProxyModel;
    KeyboardShortcutsModel *_model;

    QAction *_restoreDefaultShortcutAction, *_removeShortcutAction, *_copyIDAction, *_showOnlyModifiedAction, *_importAction, *_exportAction, *_resetAllAction, *_focusSearchAction;
};

#endif // WDGPREFERENCESPAGEKEYBOARDSHORTCUTS_H
