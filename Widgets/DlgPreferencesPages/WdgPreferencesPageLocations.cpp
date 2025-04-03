#include "WdgPreferencesPageLocations.h"
#include "ui_WdgPreferencesPageLocations.h"

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
    QList<FolderLocation> locations = LocalConfig::folderLocations();
    for(FolderLocation &loc : locations) {
        QListWidgetItem *item = new QListWidgetItem(loc.name);
        item->setData(Qt::UserRole, loc.id);
        item->setIcon(QIcon(loc.icon));
        item->setSizeHint(QSize(0, 28));
        ui->lwLocationCategories->addItem(item);

        _folderLocations.insert(loc.id, loc);
    }
}

void WdgPreferencesPageLocations::savePreferences() {
    ui->lwLocationCategories->setCurrentItem(nullptr);

    for(FolderLocation &loc : _folderLocations)
        LocalConfig::updateFolderLocation(loc);
}

QString WdgPreferencesPageLocations::id() {
    return "base.locations";
}

QString WdgPreferencesPageLocations::name() {
    return tr("Folders & Locations");
}

QIcon WdgPreferencesPageLocations::icon() {
    return QIcon(":/Icons/FileOpen.ico");
}

void WdgPreferencesPageLocations::on_lwLocationCategories_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if(previous) {
        QString oldID = previous->data(Qt::UserRole).toString();

        FolderLocation oldLoc = _folderLocations[oldID];
        oldLoc.paths.clear();
        if(oldLoc.multiple)
            for(int i = 0; i < ui->lwLocationMultipleFolders->count(); i++)
                oldLoc.paths << ui->lwLocationMultipleFolders->item(i)->text();
        else
            oldLoc.paths = {ui->leLocationSingleFolder->text()};

        _folderLocations[oldID] = oldLoc;
    }

    ui->lwLocationMultipleFolders->clear();
    ui->leLocationSingleFolder->clear();

    if(current) {
        QString id = current->data(Qt::UserRole).toString();
        FolderLocation loc = _folderLocations[id];
        ui->swLocationSelector->setCurrentIndex(_folderLocations[id].multiple ? 1 : 0);

        // update data
        if(loc.multiple)
            ui->lwLocationMultipleFolders->addItems(loc.paths);
        else
            ui->leLocationSingleFolder->setText(loc.paths.isEmpty() ? "" : loc.paths.first());
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
