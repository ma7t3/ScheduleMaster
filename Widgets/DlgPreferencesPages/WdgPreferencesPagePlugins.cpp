#include "WdgPreferencesPagePlugins.h"
#include "ui_WdgPreferencesPagePlugins.h"

WdgPreferencesPagePlugins::WdgPreferencesPagePlugins(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPluginsPage) {
    ui->setupUi(this);

    reloadPreferences();
}

WdgPreferencesPagePlugins::~WdgPreferencesPagePlugins() {
    delete ui;
}

void WdgPreferencesPagePlugins::reloadPreferences() {
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPagePlugins::savePreferences() {
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPagePlugins::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPagePlugins::id() {
    return "base.plugins";
}

QString WdgPreferencesPagePlugins::name() {
    return tr("Plugins");
}

QIcon WdgPreferencesPagePlugins::icon() {
    return QIcon(":/Icons/puzzle-piece.ico");
}
