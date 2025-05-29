#include "WdgGlobalSearchItem.h"
#include "ui_WdgGlobalSearchItem.h"

WdgGlobalSearchItem::WdgGlobalSearchItem(const ActionConfig &actionConfig, QWidget *parent) : QWidget(parent), ui(new Ui::WdgGlobalSearchItem) {
    ui->setupUi(this);
    setAction(actionConfig);
}

WdgGlobalSearchItem::~WdgGlobalSearchItem() {
    delete ui;
}

void WdgGlobalSearchItem::setAction(const ActionConfig &actionConfig) {
    ui->lText->setText(actionConfig.description);
    ui->lBreadcrumb->setText(actionConfig.breadcrumb.join(" > "));
    ui->lShortcut->setText(ActionManager::keyboardShortcut(actionConfig.id()).toString(QKeySequence::NativeText));
}
