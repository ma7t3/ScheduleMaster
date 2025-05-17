#include "WdgWelcomeRecentProjectEntry.h"
#include "ui_WdgWelcomeRecentProjectEntry.h"

#include <QDateTime>
#include <QStyleHints>
#include <QStyle>

WdgWelcomeRecentProjectEntry::WdgWelcomeRecentProjectEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWelcomeRecentProjectEntry) {
    ui->setupUi(this);

    ui->lIcon->setScaledContents(true);

    setupLastUsedLabel();

    connect(ui->pbOpen,   &QPushButton::clicked, this, [this](){emit open(path());});
    connect(ui->pbRemove, &QPushButton::clicked, this, [this](){emit removeFromList(path());});
}

WdgWelcomeRecentProjectEntry::~WdgWelcomeRecentProjectEntry() {
    delete ui;
}

void WdgWelcomeRecentProjectEntry::setIcon(const QPixmap &pixmap) {
    ui->lIcon->setPixmap(pixmap);
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
    ui->lIcon->setPixmap(QPixmap(":/Icons/classic/file-close.ico"));
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
