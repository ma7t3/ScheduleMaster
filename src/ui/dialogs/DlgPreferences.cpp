#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"
#include "src/core/IconServiceImpl.h"

#include "src/ui/dialogs/DlgConfigEditor.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageHome.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageGeneral.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageAppearance.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageLocations.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageUpdates.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageKeyboardShortcuts.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPagePlugins.h"
#include "src/ui/widgets/DlgPreferencesPages/WdgPreferencesPageDebug.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QLocale>
#include <QProcess>

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

    connect(SM::IconServiceImpl::instance(), &SM::IconServiceImpl::currentIconSetChanged, this, &DlgPreferences::onIconSetChanged);
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
    if(!SM::SettingsServiceImpl::instance()->restartRequired())
    return;

    const QStringList items = SM::SettingsServiceImpl::instance()->modifiedRestartRequiredSettings();

    QStringList itemNames;
    for(const QString &item : items)
        itemNames << SM::SettingsServiceImpl::instance()->settingMetaData(item).description.split("\n").first();

    QString itemList = "<ul><li>" + itemNames.join("</li><li>") + "</li></ul>";

    const QMessageBox::StandardButton msg = QMessageBox::information(this,
                             tr("Restart required"),
                             tr("<p>Some settings require an application restart to be "
                                "applied:</p>%1<p>Do you want to restart now?</p>")
                                 .arg(itemList),
                             QMessageBox::Yes | QMessageBox::No);

    if(msg == QMessageBox::Yes) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void DlgPreferences::on_lwList_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->swContent->setCurrentIndex(ui->lwList->row(current));
    ui->lTitle->setText(current->text());
    ui->lTitleIcon->setPixmap(current->icon().pixmap(28, 28));
}

void DlgPreferences::onIconSetChanged() {
    for(int i = 0; i < _pages.count(); i++)
        ui->lwList->item(i)->setIcon(_pages[i]->icon());

    ui->lTitleIcon->setPixmap(ui->lwList->currentItem()->icon().pixmap(28, 28));
}

void DlgPreferences::accept() {
    savePreferences();
    QDialog::accept();
}

void DlgPreferences::reject() {
    if(unsavedChanges()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Unsaved changes"), tr("<p><b>There are some changes in your preferences that aren't save now!</b></p><p>Do you want to save or discard them?</p>"), QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Save);

        switch(msg) {
        case QMessageBox::Save:    savePreferences(); accept(); return;
        case QMessageBox::Discard: discardPreviewPreferences(); break;
        default: return;
        }
    }

    QDialog::reject();
}
