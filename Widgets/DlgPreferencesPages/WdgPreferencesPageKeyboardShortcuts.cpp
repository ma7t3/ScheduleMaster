#include "WdgPreferencesPageKeyboardShortcuts.h"
#include "ui_WdgPreferencesPageKeyboardShortcuts.h"

WdgPreferencesPageKeyboardShortcuts::WdgPreferencesPageKeyboardShortcuts(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageKeyboardShortcuts) {
    ui->setupUi(this);
}

WdgPreferencesPageKeyboardShortcuts::~WdgPreferencesPageKeyboardShortcuts()
{
    delete ui;
}

void WdgPreferencesPageKeyboardShortcuts::reloadPreferences() {

}

void WdgPreferencesPageKeyboardShortcuts::savePreferences() {

}

void WdgPreferencesPageKeyboardShortcuts::discardPreviewPreferences() {

}

QString WdgPreferencesPageKeyboardShortcuts::id() {
    return "base.keyboardShortcuts";
}

QString WdgPreferencesPageKeyboardShortcuts::name() {
    return tr("Keyboard Shortcuts");
}

QIcon WdgPreferencesPageKeyboardShortcuts::icon() {
    return QIcon(":/Icons/Keyboard.ico");
}
