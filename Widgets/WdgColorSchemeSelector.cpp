#include "WdgColorSchemeSelector.h"
#include "ui_WdgColorSchemeSelector.h"

WdgColorSchemeSelector::WdgColorSchemeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgColorSchemeSelector) {
    ui->setupUi(this);

    connect(ui->rbAuto,  &QRadioButton::clicked, this, &WdgColorSchemeSelector::onColorSchemeChanged);
    connect(ui->rbLight, &QRadioButton::clicked, this, &WdgColorSchemeSelector::onColorSchemeChanged);
    connect(ui->rbDark,  &QRadioButton::clicked, this, &WdgColorSchemeSelector::onColorSchemeChanged);
}

WdgColorSchemeSelector::~WdgColorSchemeSelector() {
    delete ui;
}

Qt::ColorScheme WdgColorSchemeSelector::colorScheme() const {
    return ui->rbAuto->isChecked()  ? Qt::ColorScheme::Unknown
         : ui->rbLight->isChecked() ? Qt::ColorScheme::Light
         : ui->rbDark->isChecked()  ? Qt::ColorScheme::Dark
                                    : Qt::ColorScheme::Unknown;
}

void WdgColorSchemeSelector::setColorScheme(const Qt::ColorScheme &colorScheme) {
    switch(colorScheme) {
        case Qt::ColorScheme::Unknown: ui->rbAuto->setChecked(true); break;
        case Qt::ColorScheme::Light:   ui->rbLight->setChecked(true);break;
        case Qt::ColorScheme::Dark:    ui->rbDark->setChecked(true); break;
    }
}

void WdgColorSchemeSelector::onColorSchemeChanged() {
    emit colorSchemeChanged(colorScheme());
}
