#include "WdgGlobalSearchItem.h"
#include "ui_WdgGlobalSearchItem.h"

#include "Global/ActionController.h"

#include <QStyle>

WdgGlobalSearchItem::WdgGlobalSearchItem(const ActionConfig &actionConfig, QWidget *parent) : QWidget(parent), ui(new Ui::WdgGlobalSearchItem) {
    ui->setupUi(this);
    setAction(actionConfig);

    QPalette palette = qApp->style()->standardPalette();
    palette.setBrush(QPalette::Text, palette.placeholderText());
    ui->lBreadcrumb->setPalette(palette);
    ui->lShortcut->setPalette(palette);
}

WdgGlobalSearchItem::~WdgGlobalSearchItem() {
    delete ui;
}

void WdgGlobalSearchItem::setAction(const ActionConfig &actionConfig) {
    ui->lText->setText(actionConfig.description);
    ui->lBreadcrumb->setText(actionConfig.breadcrumb.join(" > "));
    ui->lShortcut->setText(ActionController::globalAction(actionConfig.id()).shortcut.toString(QKeySequence::NativeText));
}

void WdgGlobalSearchItem::setSelected(const bool &selected) {
    setStyleSheet(selected ? "QLabel { color: white; }" : "");
}
