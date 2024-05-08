#include "DlgChangelog.h"
#include "ui_DlgChangelog.h"

#include "AppInfo.h"
#include "localconfig.h"

DlgChangelog::DlgChangelog(QWidget *parent) : QDialog(parent), ui(new Ui::DlgChangelog) {
    ui->setupUi(this);

    for(int i = 0; i < AppInfo::versionCount(); i++)
        ui->cbEarlierVersions->addItem(AppInfo::olderVersion(i)->name() + (i == 0 ? tr(" (current)") : ""));

    connect(ui->cbEarlierVersions, &QComboBox::currentIndexChanged, this, &DlgChangelog::refreshChangelog);
    refreshChangelog();
}

DlgChangelog::~DlgChangelog() {
    delete ui;
}

void DlgChangelog::refreshChangelog() {
    AppInfo::AppVersion *v = AppInfo::olderVersion(ui->cbEarlierVersions->currentIndex());
    if(!v)
        return;

    ui->tbContent->setVersion(v);
}

void DlgChangelog::on_pbCheckForUpdates_clicked() {
    emit updateCheckRequested();
    accept();
}

