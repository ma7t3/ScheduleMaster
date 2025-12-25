#include "DockNews.h"
#include "ui_DockNews.h"

#include "src/namespace.h"
#include "src/core/IconServiceImpl.h"

DockNews::DockNews(QWidget *parent) :
    DockAbstract(parent),
    ui(new Ui::DockNews) {
    ui->setupUi(this);

    connect(SM::IconServiceImpl::instance(), &SM::IconServiceImpl::currentIconSetChanged, this, &DockNews::updateIcon);
    updateIcon();
}

DockNews::~DockNews() {
    delete ui;
}

void DockNews::updateIcon() {
    QIcon icon = SM::IconServiceImpl::instance()->icon("newspaper");
    QSize size = icon.actualSize(QSize(26, 26));
    ui->lIcon->setPixmap(icon.pixmap(size));
}
