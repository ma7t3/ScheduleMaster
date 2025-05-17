#include "WdgPreferencesPageUpdates.h"
#include "ui_WdgPreferencesPageUpdates.h"

WdgPreferencesPageUpdates::WdgPreferencesPageUpdates(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageUpdates) {
    ui->setupUi(this);

    reloadPreferences();
}

WdgPreferencesPageUpdates::~WdgPreferencesPageUpdates() {
    delete ui;
}

void WdgPreferencesPageUpdates::reloadPreferences() {
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageUpdates::savePreferences() {
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageUpdates::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageUpdates::id() {
    return "base.updates";
}

QString WdgPreferencesPageUpdates::name() {
    return tr("Updates");
}

QIcon WdgPreferencesPageUpdates::icon() {
    return QIcon(":/Icons/circle-rotate.ico");
}
