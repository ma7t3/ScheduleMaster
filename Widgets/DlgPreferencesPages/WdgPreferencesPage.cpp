#include "WdgPreferencesPage.h"

WdgPreferencesPage::WdgPreferencesPage(QWidget *parent) : QWidget(parent), _unsavedChanges(false) {
}

void WdgPreferencesPage::reloadPreferences() {
    _unsavedChanges = false;
}

void WdgPreferencesPage::savePreferences() {
    _unsavedChanges = false;
}

void WdgPreferencesPage::discardPreviewPreferences() {}

bool WdgPreferencesPage::unsavedChanges() {
    return _unsavedChanges;
}

void WdgPreferencesPage::setUnsaved() {
    _unsavedChanges = true;
}
