#include "WdgPreferencesPagePlugins.h"
#include "ui_WdgPreferencesPluginsPage.h"

WdgPreferencesPagePlugins::WdgPreferencesPagePlugins(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPluginsPage) {
    ui->setupUi(this);
}

WdgPreferencesPagePlugins::~WdgPreferencesPagePlugins() {
    delete ui;
}

void WdgPreferencesPagePlugins::reloadPreferences() {}

void WdgPreferencesPagePlugins::savePreferences() {}

QString WdgPreferencesPagePlugins::id() {
    return "base.plugins";
}

QString WdgPreferencesPagePlugins::name() {
    return tr("Plugins");
}

QIcon WdgPreferencesPagePlugins::icon() {
    return QIcon(":/Icons/Plugin.ico");
}
