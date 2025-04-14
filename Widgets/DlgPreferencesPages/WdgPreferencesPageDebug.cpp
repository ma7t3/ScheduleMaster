#include "WdgPreferencesPageDebug.h"
#include "ui_WdgPreferencesPageDebug.h"

WdgPreferencesPageDebug::WdgPreferencesPageDebug(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageDebug) {
    ui->setupUi(this);
}

WdgPreferencesPageDebug::~WdgPreferencesPageDebug() {
    delete ui;
}

void WdgPreferencesPageDebug::reloadPreferences() {
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageDebug::savePreferences() {
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageDebug::discardPreviewPreferences() {}

QString WdgPreferencesPageDebug::id() {
    return "base.debug";
}

QString WdgPreferencesPageDebug::name() {
    return tr("Debug");
}

QIcon WdgPreferencesPageDebug::icon() {
    return QIcon(":/Icons/Bug.ico");
}
