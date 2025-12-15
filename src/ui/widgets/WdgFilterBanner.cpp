#include "WdgFilterBanner.h"
#include "ui_WdgFilterBanner.h"

#include "Global/ActionController.h"

#include <QPainter>

WdgFilterBanner::WdgFilterBanner(QWidget *parent) : QWidget(parent), ui(new Ui::WdgFilterBanner) {
    ui->setupUi(this);

    connect(ui->tbClearFilter,   &QAbstractButton::clicked, this, &WdgFilterBanner::clearFilterRequested);
    connect(ui->tbClose,         &QAbstractButton::clicked, this, &WdgFilterBanner::close);
    connect(ui->tbDontShowAgain, &QAbstractButton::clicked, this, &WdgFilterBanner::dontShowAgain);

    ActionController::add(ui->tbClearFilter, "projectDataTable.filter.clear", ActionController::TextComponent);
    ActionController::add(ui->tbClose,       "projectDataTable.filterBanner.close");
}

WdgFilterBanner::~WdgFilterBanner() {
    delete ui;
}

void WdgFilterBanner::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(64, 128, 255, 96));
}

void WdgFilterBanner::close() {
    hide();
    emit closed();
}

void WdgFilterBanner::dontShowAgain() {
    // TODO
    close();
}
