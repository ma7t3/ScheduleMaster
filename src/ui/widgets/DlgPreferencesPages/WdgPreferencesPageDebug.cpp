#include "WdgPreferencesPageDebug.h"
#include "ui_WdgPreferencesPageDebug.h"

#include "src/namespace.h"
#include "src/core/IconServiceImpl.h"

WdgPreferencesPageDebug::WdgPreferencesPageDebug(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageDebug) {
    ui->setupUi(this);

    reloadPreferences();
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

void WdgPreferencesPageDebug::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageDebug::id() {
    return "base.debug";
}

QString WdgPreferencesPageDebug::name() {
    return tr("Debug");
}

QIcon WdgPreferencesPageDebug::icon() {
    return SM::IconServiceImpl::instance()->icon("bug");
}
