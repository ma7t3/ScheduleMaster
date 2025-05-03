#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "Dialogs/DlgConfigEditor.h"
#include "Global/SettingsManager.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageHome.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageLocations.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.h"
#include "Widgets/DlgPreferencesPages/WdgPreferencesPageDebug.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QLocale>

DlgPreferences::DlgPreferences(QWidget *parent) : QDialog(parent),
    ui(new Ui::DlgPreferences) {
    ui->setupUi(this);

    qInfo() << "Loading preferences...";
    WdgPreferencesPageHome       *home       = new WdgPreferencesPageHome(this);       addPage(home);
    WdgPreferencesPageGeneral    *general    = new WdgPreferencesPageGeneral(this);    addPage(general);
    WdgPreferencesPageAppearance *appearance = new WdgPreferencesPageAppearance(this); addPage(appearance);
    addPage(new WdgPreferencesPageLocations(this));
    addPage(new WdgPreferencesPageUpdates(this));
    addPage(new WdgPreferencesPageKeyboardShortcuts(this));
    addPage(new WdgPreferencesPagePlugins(this));
    addPage(new WdgPreferencesPageDebug(this));

    connect(home,    &WdgPreferencesPageHome::languageIndexChanged,         general, &WdgPreferencesPageGeneral::setLanguageIndex);
    connect(general, &WdgPreferencesPageGeneral::languageIndexChanged,      home,    &WdgPreferencesPageHome::setLanguageIndex);

    connect(home,       &WdgPreferencesPageHome::styleIndexChanged,       appearance, &WdgPreferencesPageAppearance::setStyleIndex);
    connect(appearance, &WdgPreferencesPageAppearance::styleIndexChanged, home,       &WdgPreferencesPageHome::setStyleIndex);

    connect(home,    &WdgPreferencesPageHome::openLogfileLocationRequested, general, &WdgPreferencesPageGeneral::openLogfileLocation);

    for(int i = 0; i < ui->lwList->count(); i++)
        ui->lwList->item(i)->setSizeHint(QSize(0, 32));

    ui->lwList->setFocus();
    ui->lwList->setCurrentRow(0);

    connect(ui->pbConfigEditor, &QPushButton::clicked,                 this, &DlgPreferences::openConfigEditor);
    connect(home, &WdgPreferencesPageHome::openPageRequested,          this, &DlgPreferences::setCurrentPage);
    connect(home, &WdgPreferencesPageHome::openConfigEditorRequested,  this, &DlgPreferences::openConfigEditor);
}

DlgPreferences::~DlgPreferences() {
    delete ui;
}

void DlgPreferences::addPage(WdgPreferencesPage *page) {
    _pages << page;

    QListWidgetItem *item = new QListWidgetItem(page->icon(), page->name());
    ui->lwList->addItem(item);

    ui->swContent->addWidget(page);
}

void DlgPreferences::setCurrentPageIndex(const int &index) {
    if(index < 0 || index >= ui->lwList->count())
        return;

    ui->lwList->setCurrentRow(index);
}

void DlgPreferences::setCurrentPage(const QString &id) {
    for(int i = 0; i < _pages.count(); i++)
        if(_pages[i]->id() == id)
            ui->lwList->setCurrentRow(i);
}

void DlgPreferences::openConfigEditor() {
    if(unsavedChanges()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Unsaved changes"), tr("<p><b>There are some changes in your preferences that aren't save now!</b></p><p>Do you want to save them before opening the config editor?</p>"), QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Save);

        switch(msg) {
        case QMessageBox::Save: savePreferences();
        case QMessageBox::Discard: break;
        default: return;
        }
    }

    qInfo() << "opening config editor...";

    DlgConfigEditor dlg(this);
    dlg.exec();
    checkRestartRequired();
    reloadPreferences();
}

void DlgPreferences::reloadPreferences() {
    qInfo() << "Reloading preferences...";

    for(WdgPreferencesPage *page : std::as_const(_pages))
        page->reloadPreferences();
}

void DlgPreferences::savePreferences() {
    qInfo() << "Saving preferences...";

    for(WdgPreferencesPage *page : std::as_const(_pages))
        page->savePreferences();

    checkRestartRequired();
}

void DlgPreferences::discardPreviewPreferences() {
    for(WdgPreferencesPage *page : std::as_const(_pages))
        page->discardPreviewPreferences();
}

bool DlgPreferences::unsavedChanges() {
    bool unsaved = false;
    for(WdgPreferencesPage *page : std::as_const(_pages))
        unsaved |= page->unsavedChanges();

    return unsaved;
}

void DlgPreferences::checkRestartRequired() {
    if(SettingsManager::restartRequired()) {
        const QStringList items = SettingsManager::modifiedRestartRequiredSettings();

        QStringList itemNames;
        for(const QString &item : items)
            itemNames << SettingsManager::item(item).description.split("\n").first();

        QString itemList = "<ul><li>" + itemNames.join("</li><li>") + "</li></ul>";

        QMessageBox::information(this, tr("Restart required"), tr("<p>Some settings require an application restart to be applied:</p>") + itemList);
    }
}

void DlgPreferences::on_lwList_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->swContent->setCurrentIndex(ui->lwList->row(current));
    ui->lTitle->setText(current->text());
    ui->lTitleIcon->setPixmap(current->icon().pixmap(28, 28));
}

void DlgPreferences::accept() {
    savePreferences();
    QDialog::accept();
}

void DlgPreferences::reject() {
    if(unsavedChanges()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Unsaved changes"), tr("<p><b>There are some changes in your preferences that aren't save now!</b></p><p>Do you want to save or discard them?</p>"), QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Save);

        switch(msg) {
        case QMessageBox::Save: savePreferences(); QDialog::accept();
        case QMessageBox::Discard: break;
        default: return;
        }
    }

    discardPreviewPreferences();
    QDialog::reject();
}
