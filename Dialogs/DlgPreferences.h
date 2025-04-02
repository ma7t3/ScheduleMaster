#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLocale>
#include <QFileDialog>

#include "Global/LocalConfig.h"

namespace Ui {
class DlgPreferences;
}

class DlgPreferences : public QDialog {
    Q_OBJECT

public:
    explicit DlgPreferences(QWidget *parent);
    ~DlgPreferences();

    enum Pages {
        GeneralPage,
        AppearancePage,
        FolderLocationsPage,
        PluginsPage,
        WorkspacesPage,
        MiscellaneousPage,
        InfoPage,
        DebugPage
    };

public:
    void setCurrentPage(const int &index);

private:
    void loadPreferences();
    void savePreferences();

private slots:
    void on_lwList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pbReset_clicked();

    void accept() override;

    void on_cbLogfileMode_currentIndexChanged(int index);

    void on_lwLocationCategories_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbBrowseLocationSingleFolder_clicked();

    void on_pbLocationMultipleFoldersAdd_clicked();

    void on_pbLocationMultipleFoldersRemove_clicked();

    void on_lwLocationMultipleFolders_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::DlgPreferences *ui;

    QHash<QString, FolderLocation> _folderLocations;
};

#endif // DLGPREFERENCES_H
