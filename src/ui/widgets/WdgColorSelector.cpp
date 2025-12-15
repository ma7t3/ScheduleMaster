#include "WdgColorSelector.h"
#include "ui_WdgColorSelector.h"

#include <QColorDialog>

WdgColorSelector::WdgColorSelector(QWidget *parent) :
    QWidget(parent), ui(new Ui::WdgColorSelector) {
    ui->setupUi(this);
    setColor(Qt::black);
}

WdgColorSelector::~WdgColorSelector() {
    delete ui;
}

QColor WdgColorSelector::color() const {
    return _color;
}

void WdgColorSelector::setColor(const QColor &newColor) {
    _color = newColor;
    ui->lPreview->setStyleSheet("background-color: " + _color.name());
    ui->lHex->setText(_color.name());

    emit colorChanged(_color);
}

void WdgColorSelector::on_pbSelect_clicked() {
    const QColor color = QColorDialog::getColor(_color);
    if(!color.isValid())
        return;

    setColor(color);
}
