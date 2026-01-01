#include "WdgAccentColorSelector.h"
#include "ui_WdgAccentColorSelector.h"

#include "src/namespace.h"
#include "src/core/AppearanceServiceImpl.h"

#include <QToolButton>

WdgAccentColorSelector::WdgAccentColorSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgAccentColorSelector),
    _currentColor(-1) {
    ui->setupUi(this);

    _accentColorIDs << SM::AppearanceServiceImpl::instance()->accentColors().keys();

    _buttons << ui->tbSystem;
    _buttons << ui->tbBlue;
    _buttons << ui->tbGreen;
    _buttons << ui->tbLightBlue;
    _buttons << ui->tbOrange;
    _buttons << ui->tbPink;
    _buttons << ui->tbPurple;
    _buttons << ui->tbRed;

    setAccentColor(0);
    for(int i = 0; i < _buttons.size(); i++) {
        QToolButton *button = _buttons[i];
        const QString id = i == 0 ? "system" : _accentColorIDs.at(i - 1);
        connect(button, &QToolButton::clicked, this, [this, id](){setAccentColor(id);});
    }
}

WdgAccentColorSelector::~WdgAccentColorSelector() {
    delete ui;
}

QString WdgAccentColorSelector::accentColorID() const {
    return _currentColor == -1 ? "system" : _accentColorIDs.at(_currentColor);
}

QColor WdgAccentColorSelector::accentColor() const {
    return _currentColor == -1 ? QColor() : SM::AppearanceServiceImpl::instance()->accentColor(_accentColorIDs.at(_currentColor));
}

void WdgAccentColorSelector::setAccentColor(const QString &id) {
    _currentColor = _accentColorIDs.indexOf(id);
    updateButtons();
    emit currentAccentColorChanged(id);
}

QString WdgAccentColorSelector::generateStyleSheet(const QString &id, const bool &selected) const {
    QString styleSheetTemplate = "background-color: %1; border-radius: 16px";
    const QColor color     = id == "system" ? QColor(128, 128, 128) : SM::AppearanceServiceImpl::instance()->accentColor(id);
    const QColor darkColor = QColor(color.red() * 0.5, color.green() * 0.5, color.blue() * 0.5);
    return styleSheetTemplate.arg(color.name()) + (selected ?  QString("; border: 3px solid %1;").arg(darkColor.name()) : "");
}

void WdgAccentColorSelector::updateButtons() {
    for(int i = 0; i < _buttons.size(); i++) {
        QToolButton *button = _buttons[i];
        const QString id = i == 0 ? "system" : _accentColorIDs.at(i - 1);
        button->setStyleSheet(generateStyleSheet(id, _currentColor == i - 1));
    }
}
