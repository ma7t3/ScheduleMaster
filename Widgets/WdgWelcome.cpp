#include "WdgWelcome.h"
#include "ui_WdgWelcome.h"

#include <QDateTime>
#include <QFileInfo>

#include "Global/LocalConfig.h"
#include "WdgWelcomeRecentProjectEntry.h"

WdgWelcome::WdgWelcome(QWidget *parent) :
    WdgAbstract(parent),
    ui(new Ui::WdgWelcome) {
    ui->setupUi(this);

    ui->lIcon->setPixmap(QPixmap(":/Icons/ScheduleMaster_64px.ico"));

    _recentFileOpen   = ui->lwRecentProjects->addAction(QPixmap(":/Icons/FileOpen.ico"), tr("Open"));
    _recentFileRemove = ui->lwRecentProjects->addAction(QPixmap(":/Icons/Quit.ico"),     tr("Remove from list"));
    ui->lwRecentProjects->setContextMenuPolicy(Qt::ActionsContextMenu);

    _lastUsedFiles = LocalConfig::lastUsedFiles();

    updateRecentFilesList();

    connect(ui->clbNewProject,  &QCommandLinkButton::clicked, this, &WdgWelcome::newProject);
    connect(ui->clbOpenProject, &QCommandLinkButton::clicked, this, &WdgWelcome::openProject);
    connect(ui->clbPreferences, &QCommandLinkButton::clicked, this, &WdgWelcome::openPreferences);
    connect(ui->clbQuit,        &QCommandLinkButton::clicked, this, &WdgWelcome::quitApplication);
}

WdgWelcome::~WdgWelcome() {
    delete ui;
}

void WdgWelcome::updateRecentFilesList() {
    ui->lwRecentProjects->clear();

    for(QString current : _lastUsedFiles) {
        QListWidgetItem *itm = new QListWidgetItem(ui->lwRecentProjects);
        ui->lwRecentProjects->addItem(itm);
        WdgWelcomeRecentProjectEntry *wdg = new WdgWelcomeRecentProjectEntry(ui->lwRecentProjects);

        QFileInfo fi(current);
        wdg->setIcon(QPixmap(":/Icons/File.ico"));
        wdg->setName(fi.baseName());
        wdg->setPath(current);
        wdg->setLastUsed(fi.lastModified());

        if(!QFile::exists(current)) {
            wdg->setFileMissing();
            itm->setFlags(itm->flags() & ~Qt::ItemIsEnabled);
        }

        ui->lwRecentProjects->setItemWidget(itm, wdg);
        itm->setSizeHint(wdg->sizeHint());
    }
}

void WdgWelcome::on_pbToggleNews_clicked() {
    bool visible = ui->gbNews->isVisible();
    ui->pbToggleNews->setText(visible ? tr("Show News") : tr("Hide News"));
    ui->gbNews->setVisible(!visible);
}
