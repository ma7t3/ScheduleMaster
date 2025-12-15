#include "WdgWelcomeRecentProjectEntry.h"
#include "ui_WdgWelcomeRecentProjectEntry.h"

#include "Global/ActionController.h"
#include "Global/IconController.h"

#include <QDateTime>
#include <QStyleHints>
#include <QStyle>
#include <QPainter>

WdgWelcomeRecentProjectEntry::WdgWelcomeRecentProjectEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWelcomeRecentProjectEntry),
    _fileExists(true) {
    ui->setupUi(this);

    setupLastUsedLabel();

    connect(ui->pbOpen,   &QPushButton::clicked, this, [this](){emit open(path());});
    connect(ui->pbRemove, &QPushButton::clicked, this, [this](){emit removeFromList(path());});

    ActionController::add(ui->pbOpen, "project.recentFiles.openItem", ActionController::IconComponent);
    ActionController::add(ui->pbRemove, "project.recentFiles.removeItem", ActionController::IconComponent);

    connect(IconController::instance(), &IconController::currentIconSetChanged, this, &WdgWelcomeRecentProjectEntry::updateIcon);
    updateIcon();
}

WdgWelcomeRecentProjectEntry::~WdgWelcomeRecentProjectEntry() {
    delete ui;
}

void WdgWelcomeRecentProjectEntry::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    int margin = (sizeHint().height() - 48) / 2;
    painter.drawPixmap(margin, margin, _icon.pixmap(_icon.actualSize(QSize(48, 48))));
}

void WdgWelcomeRecentProjectEntry::setName(const QString &newName) {
    ui->lName->setText(newName);
}

void WdgWelcomeRecentProjectEntry::setPath(const QString &newPath) {
    ui->lPath->setText(newPath);
}

void WdgWelcomeRecentProjectEntry::setLastUsed(const QDateTime &newLastUsed) {
    ui->lLastUsed->setText(tr("Last used %1").arg(newLastUsed.toString(QLocale().dateTimeFormat(QLocale::ShortFormat))));
}

void WdgWelcomeRecentProjectEntry::setFileMissing() {
    _fileExists = false;
    updateIcon();
    ui->lLastUsed->setText(tr("This file no longer exists."));
    ui->lName->setDisabled(true);
    ui->lPath->setDisabled(true);
    ui->pbOpen->setDisabled(true);
}

QString WdgWelcomeRecentProjectEntry::path() const {
    return ui->lPath->text();
}

void WdgWelcomeRecentProjectEntry::setupLastUsedLabel() {
    QFont lastUsedFont = ui->lLastUsed->font();
    lastUsedFont.setItalic(true);
    ui->lLastUsed->setFont(lastUsedFont);
    QPalette palette = qApp->style()->standardPalette();
    palette.setBrush(QPalette::Text, palette.placeholderText());
    ui->lLastUsed->setPalette(palette);

    // update if the colorscheme changes at runtime
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this](){setupLastUsedLabel();});
}

void WdgWelcomeRecentProjectEntry::updateIcon() {
    _icon = IconController::icon(_fileExists ? "file" : "file-close");
    update();
}
