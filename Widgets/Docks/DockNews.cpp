#include "DockNews.h"
#include "ui_DockNews.h"

#include "Global/IconController.h"

DockNews::DockNews(QWidget *parent) :
    DockAbstract(parent),
    ui(new Ui::DockNews) {
    ui->setupUi(this);

    connect(IconController::instance(), &IconController::currentIconSetChanged, this, &DockNews::updateIcon);
    updateIcon();
}

DockNews::~DockNews() {
    delete ui;
}

void DockNews::updateIcon() {
    QIcon icon = IconController::icon("newspaper");
    QSize size = icon.actualSize(QSize(26, 26));
    ui->lIcon->setPixmap(icon.pixmap(size));
}
