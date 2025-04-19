#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLocale>
#include <QFileDialog>

#include "Dialogs/DlgConfigEditor.h"

#include "Global/LocalConfig.h"

#include "Widgets/DlgPreferencesPages/WdgPreferencesPageHome.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageLocations.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageDebug.h"

namespace Ui {
class DlgPreferences;
}

class DlgPreferences : public QDialog {
    Q_OBJECT

public:
    explicit DlgPreferences(QWidget *parent);
    ~DlgPreferences();

public:
    void addPage(WdgPreferencesPage *page);

public slots:
    void setCurrentPageIndex(const int &index);
    void setCurrentPage(const QString &id);
    void openConfigEditor();

private:
    void reloadPreferences();
    void savePreferences();
    void discardPreviewPreferences();
    bool unsavedChanges();
    void checkRestartRequired();

private slots:
    void on_lwList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void accept() override;
    void reject() override;

private:
    Ui::DlgPreferences *ui;

    QList<WdgPreferencesPage *> _pages;
};

#endif // DLGPREFERENCES_H
