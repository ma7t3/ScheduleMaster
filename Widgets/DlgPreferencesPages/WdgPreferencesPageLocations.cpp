#include "WdgPreferencesPageLocations.h"
#include "ui_WdgPreferencesPageLocations.h"

#include <QListWidgetItem>
#include <QFileDialog>


// TODO: Review (variable names)

WdgPreferencesPageLocations::WdgPreferencesPageLocations(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageLocations) {
    ui->setupUi(this);

    ui->lwLocationMultipleFolders->setSortingEnabled(true);

    reloadPreferences();

    ui->lwLocationCategories->setCurrentRow(0);
}

WdgPreferencesPageLocations::~WdgPreferencesPageLocations() {
    delete ui;
}

void WdgPreferencesPageLocations::reloadPreferences() {
    ui->lwLocationCategories->clear();

    QList<FolderLocationConfig> locations = FolderLocationManager::items();
    for(FolderLocationConfig &loc : locations) {
        QListWidgetItem *item = new QListWidgetItem(loc.name);
        item->setData(Qt::UserRole, loc.id());
        item->setIcon(QIcon(loc.icon));
        item->setSizeHint(QSize(0, 28));
        ui->lwLocationCategories->addItem(item);

        _folderLocations.insert(loc.id(), loc);
    }

    _folderLocationsPaths = FolderLocationManager::currentFolderLocations();

    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageLocations::savePreferences() {
    ui->lwLocationCategories->setCurrentItem(nullptr);

    FolderLocationManager::setCurrentFolderLocations(_folderLocationsPaths);

    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageLocations::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageLocations::id() {
    return "base.locations";
}

QString WdgPreferencesPageLocations::name() {
    return tr("Folders & Locations");
}

QIcon WdgPreferencesPageLocations::icon() {
    return QIcon(":/Icons/folder-open.ico");
}

void WdgPreferencesPageLocations::on_lwLocationCategories_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if(previous) {
        QString oldID = previous->data(Qt::UserRole).toString();

        _folderLocationsPaths[oldID].clear();
        QStringList newPaths;

        if(_folderLocations[oldID].multiple)
            for(int i = 0; i < ui->lwLocationMultipleFolders->count(); i++)
                newPaths << ui->lwLocationMultipleFolders->item(i)->text();
        else
            newPaths = {ui->leLocationSingleFolder->text()};

        _folderLocationsPaths[oldID] = newPaths;
        setUnsaved();
    }

    ui->lwLocationMultipleFolders->clear();
    ui->leLocationSingleFolder->clear();

    if(current) {
        QString id = current->data(Qt::UserRole).toString();
        FolderLocationConfig loc = _folderLocations[id];
        ui->swLocationSelector->setCurrentIndex(_folderLocations[id].multiple ? 1 : 0);

        QStringList paths = _folderLocationsPaths[id];

        // update data
        if(loc.multiple)
            ui->lwLocationMultipleFolders->addItems(paths);
        else
            ui->leLocationSingleFolder->setText(paths.isEmpty() ? "" : paths.first());
    }
}

void WdgPreferencesPageLocations::on_pbBrowseLocationSingleFolder_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "", ui->leLocationSingleFolder->text());
    if(path.isEmpty())
        return;

    ui->leLocationSingleFolder->setText(path);
}

void WdgPreferencesPageLocations::on_pbLocationMultipleFoldersAdd_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "", ui->leLocationSingleFolder->text());
    if(path.isEmpty())
        return;

    ui->lwLocationMultipleFolders->addItem(path);
}

void WdgPreferencesPageLocations::on_pbLocationMultipleFoldersRemove_clicked() {
    QListWidgetItem *item = ui->lwLocationMultipleFolders->currentItem();
    if(!item)
        return;

    ui->lwLocationMultipleFolders->takeItem(ui->lwLocationMultipleFolders->row(item));
}

void WdgPreferencesPageLocations::on_lwLocationMultipleFolders_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->pbLocationMultipleFoldersRemove->setEnabled(current);
}
