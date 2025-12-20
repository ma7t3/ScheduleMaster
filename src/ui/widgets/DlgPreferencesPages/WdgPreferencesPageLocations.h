#ifndef WDGPREFERENCESPAGELOCATIONS_H
#define WDGPREFERENCESPAGELOCATIONS_H

#include "src/namespace.h"
#include "src/api/IFolderLocationService.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPage.h"

class QListWidgetItem;

namespace Ui {
class WdgPreferencesPageLocations;
}

class WdgPreferencesPageLocations : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageLocations(QWidget *parent);
    ~WdgPreferencesPageLocations();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private slots:
    void on_lwLocationCategories_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbBrowseLocationSingleFolder_clicked();

    void on_pbLocationMultipleFoldersAdd_clicked();

    void on_pbLocationMultipleFoldersRemove_clicked();

    void on_lwLocationMultipleFolders_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void onIconSetChanged();

private:
    Ui::WdgPreferencesPageLocations *ui;

    QMap<QString, SMA::FolderLocationConfig> _folderLocations;
    QMap<QString, QStringList> _folderLocationsPaths;
};

#endif // WDGPREFERENCESPAGELOCATIONS_H
