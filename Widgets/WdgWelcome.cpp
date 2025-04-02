#include "WdgWelcome.h"
#include "ui_WdgWelcome.h"

#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QScrollBar>

#include "Global/LocalConfig.h"
#include "WdgWelcomeRecentProjectEntry.h"

WdgWelcome::WdgWelcome(QWidget *parent) :
    WdgAbstract(parent),
    ui(new Ui::WdgWelcome) {
    ui->setupUi(this);

    ui->lIcon->setPixmap(QPixmap(":/Icons/ScheduleMaster_64px.ico"));

    _recentFileOpen         = ui->lwRecentProjects->addAction(QPixmap(":/Icons/FileOpen.ico"), tr("Open"));
    _recentFileOpenLocation = ui->lwRecentProjects->addAction(QPixmap(":/Icons/FileOpen.ico"), tr("Open Directory"));
    _recentFileRemove       = ui->lwRecentProjects->addAction(QPixmap(":/Icons/Quit.ico"),     tr("Remove from list"));
    ui->lwRecentProjects->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_recentFileOpen,         &QAction::triggered, this, &WdgWelcome::onRecentFileOpen);
    connect(_recentFileOpenLocation, &QAction::triggered, this, &WdgWelcome::onRecentFileOpenLocation);
    connect(_recentFileRemove,       &QAction::triggered, this, &WdgWelcome::onRecentFileRemove);
    updateRecentProjectsList();

    connect(ui->clbNewProject,  &QCommandLinkButton::clicked, this, &WdgWelcome::newProject);
    connect(ui->clbOpenProject, &QCommandLinkButton::clicked, this, &WdgWelcome::openProject);
    connect(ui->clbPlugins,     &QCommandLinkButton::clicked, this, &WdgWelcome::openPlugins);
    connect(ui->clbPreferences, &QCommandLinkButton::clicked, this, &WdgWelcome::openPreferences);
    connect(ui->clbQuit,        &QCommandLinkButton::clicked, this, &WdgWelcome::quitApplication);
}

WdgWelcome::~WdgWelcome() {
    delete ui;
}

void WdgWelcome::updateRecentProjectsList() {
    QStringList lastUsedFiles = LocalConfig::lastUsedFiles();

    int scrollbarPos = ui->lwRecentProjects->verticalScrollBar()->value();
    ui->lwRecentProjects->clear();

    for(QString current : lastUsedFiles) {
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

        connect(wdg, &WdgWelcomeRecentProjectEntry::open,           this, &WdgWelcome::openProjectFromFile);
        connect(wdg, &WdgWelcomeRecentProjectEntry::removeFromList, this, &WdgWelcome::removeProjectFromList);
    }

    ui->lwRecentProjects->verticalScrollBar()->setValue(scrollbarPos);
}

void WdgWelcome::on_pbToggleNews_clicked() {
    bool visible = ui->gbNews->isVisible();
    ui->pbToggleNews->setText(visible ? tr("Show News") : tr("Hide News"));
    ui->gbNews->setVisible(!visible);
}

void WdgWelcome::onRecentFileOpen() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    emit openProjectFromFile(path);
}

void WdgWelcome::onRecentFileOpenLocation() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    QFileInfo fi(path);
    QString dirPath = fi.dir().path();
    QDesktopServices::openUrl(QUrl(dirPath));
}

void WdgWelcome::onRecentFileRemove() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    // TODO
}

QString WdgWelcome::currentRecentFilePath() const {
    if(!ui->lwRecentProjects->currentItem())
        return "";

    return qobject_cast<WdgWelcomeRecentProjectEntry *>(ui->lwRecentProjects->itemWidget(ui->lwRecentProjects->currentItem()))->path();
}

void WdgWelcome::on_lwRecentProjects_itemDoubleClicked(QListWidgetItem *item) {
    if(!item || !item->flags().testFlag(Qt::ItemIsEnabled))
        return;

    QString path = qobject_cast<WdgWelcomeRecentProjectEntry *>(ui->lwRecentProjects->itemWidget(item))->path();

    emit openProjectFromFile(path);
}
