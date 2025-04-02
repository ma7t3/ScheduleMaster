#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

DlgPreferences::DlgPreferences(QWidget *parent) : QDialog(parent),
    ui(new Ui::DlgPreferences) {
    ui->setupUi(this);

    for(int i = 0; i < ui->lwList->count(); i++)
        ui->lwList->item(i)->setSizeHint(QSize(0, 32));

    ui->lwLocationMultipleFolders->setSortingEnabled(true);

    loadPreferences();

    ui->lwList->setFocus();
    ui->lwList->setCurrentRow(0);

    ui->lwLocationCategories->setCurrentRow(0);
}

DlgPreferences::~DlgPreferences() {
    delete ui;
}

void DlgPreferences::loadPreferences() {
    qInfo() << "Loading preferences...";

    // available languages:
    QList<QLocale::Language> languages = LocalConfig::supportedLanguages();
    for(QLocale::Language language : std::as_const(languages)) {
        QLocale locale(language);
        ui->cbLanguage->addItem(locale.nativeLanguageName(), locale.name());
        if(language == LocalConfig::language())
            ui->cbLanguage->setCurrentIndex(ui->cbLanguage->count() - 1);
    }

    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());

    // locations
    QList<FolderLocation> locations = LocalConfig::folderLocations();
    for(FolderLocation &loc : locations) {
        QListWidgetItem *item = new QListWidgetItem(loc.name);
        item->setData(Qt::UserRole, loc.id);
        item->setIcon(QIcon(loc.icon));
        ui->lwLocationCategories->addItem(item);

        _folderLocations.insert(loc.id, loc);
    }
}

void DlgPreferences::savePreferences() {
    qInfo() << "Saving preferences...";

    for(FolderLocation &loc : _folderLocations)
        LocalConfig::updateFolderLocation(loc);
}

void DlgPreferences::on_lwList_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->swContent->setCurrentIndex(ui->lwList->row(current));
    ui->lTitle->setText(current->text());
    ui->lTitleIcon->setPixmap(current->icon().pixmap(28, 28));
}

void DlgPreferences::on_pbReset_clicked() {
    QMessageBox::warning(this, tr("Not available"), tr("Reset preferences ist not available now."));
}

void DlgPreferences::accept() {
    ui->lwLocationCategories->setCurrentItem(nullptr);
    savePreferences();
    QDialog::accept();
}

void DlgPreferences::on_cbLogfileMode_currentIndexChanged(int index) {
    QString infoText;

    switch(index) {
    case LocalConfig::LogfileMode::NoLog:
        infoText = tr("No log file will be created.");
        break;
    case LocalConfig::LogfileMode::DefaultLog:
        infoText = tr("A normal logfile is generated, containing normal infos, warnings and error messages.");
        break;
    case LocalConfig::LogfileMode::DebugLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers.");
        break;
    case LocalConfig::LogfileMode::DebugDetailLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers. Also including exact code file names and line numers where the message was sent. Only works in debug builds.");
        break;
    }

    ui->lLogfileModeInfo->setText(infoText);
}

void DlgPreferences::on_lwLocationCategories_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
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

void DlgPreferences::on_pbBrowseLocationSingleFolder_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "", ui->leLocationSingleFolder->text());
    if(path.isEmpty())
        return;

    ui->leLocationSingleFolder->setText(path);
}

void DlgPreferences::on_pbLocationMultipleFoldersAdd_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, "", ui->leLocationSingleFolder->text());
    if(path.isEmpty())
        return;

    ui->lwLocationMultipleFolders->addItem(path);
}

void DlgPreferences::on_pbLocationMultipleFoldersRemove_clicked() {
    QListWidgetItem *item = ui->lwLocationMultipleFolders->currentItem();
    if(!item)
        return;

    ui->lwLocationMultipleFolders->takeItem(ui->lwLocationMultipleFolders->row(item));
}

void DlgPreferences::on_lwLocationMultipleFolders_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->pbLocationMultipleFoldersRemove->setEnabled(current);
}
