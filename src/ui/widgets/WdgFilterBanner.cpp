#include "WdgFilterBanner.h"
#include "ui_WdgFilterBanner.h"

#include "namespace.h"
#include "src/core/ActionServiceImpl.h"

#include <QPainter>

WdgFilterBanner::WdgFilterBanner(QWidget *parent) : QWidget(parent), ui(new Ui::WdgFilterBanner) {
    ui->setupUi(this);

    connect(ui->tbClearFilter,   &QAbstractButton::clicked, this, &WdgFilterBanner::clearFilterRequested);
    connect(ui->tbClose,         &QAbstractButton::clicked, this, &WdgFilterBanner::close);
    connect(ui->tbDontShowAgain, &QAbstractButton::clicked, this, &WdgFilterBanner::dontShowAgain);

    SM::ActionServiceImpl::instance()->addButton(ui->tbClearFilter, "projectDataTable.filter.clear", SM::ActionServiceImpl::instance()->TextComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->tbClose,       "projectDataTable.filterBanner.close");
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
