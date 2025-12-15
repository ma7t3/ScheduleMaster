#include "WdgLineFilterPopup.h"
#include "ui_WdgLineFilterPopup.h"

WdgLineFilterPopup::WdgLineFilterPopup(QWidget *parent) :
    WdgFilterPopupContent(parent), ui(new Ui::WdgLineFilterPopup) {
    ui->setupUi(this);
}

WdgLineFilterPopup::~WdgLineFilterPopup() {
    delete ui;
}

bool WdgLineFilterPopup::filterIsEnabled() const {
    return false;
}

void WdgLineFilterPopup::reset() {}
