#include "DockWelcome.h"
#include "ui_DockWelcome.h"

#include "Global/LastUsedFilesManager.h"
#include "MainWindowInterface.h"
#include "Widgets/WdgWelcomeRecentProjectEntry.h"

#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QScrollBar>

DockWelcome::DockWelcome(QWidget *parent) :
    DockAbstract(parent),
    ui(new Ui::DockWelcome) {
    ui->setupUi(this);

    ui->lIcon->setPixmap(QPixmap(":/Icons/classic/ScheduleMaster_64px.ico"));

    _recentFileOpen         = ui->lwRecentProjects->addAction(QPixmap(":/Icons/classic/folder-open.ico"), tr("Open"));
    _recentFileOpenLocation = ui->lwRecentProjects->addAction(QPixmap(":/Icons/classic/folder-open.ico"), tr("Open Directory"));
    _recentFileRemove       = ui->lwRecentProjects->addAction(QPixmap(":/Icons/classic/circle-xmark.ico"),     tr("Remove from list"));
    ui->lwRecentProjects->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_recentFileOpen,         &QAction::triggered, this, &DockWelcome::onRecentFileOpen);
    connect(_recentFileOpenLocation, &QAction::triggered, this, &DockWelcome::onRecentFileOpenLocation);
    connect(_recentFileRemove,       &QAction::triggered, this, &DockWelcome::onRecentFileRemove);
    updateRecentProjectsList();

    connect(ui->clbNewProject,  &QCommandLinkButton::clicked, this, &DockWelcome::newProject);
    connect(ui->clbOpenProject, &QCommandLinkButton::clicked, this, &DockWelcome::openProject);
    connect(ui->clbPlugins,     &QCommandLinkButton::clicked, this, &DockWelcome::openPlugins);
    connect(ui->clbPreferences, &QCommandLinkButton::clicked, this, &DockWelcome::openPreferences);
    connect(ui->clbQuit,        &QCommandLinkButton::clicked, this, &DockWelcome::quitApplication);

    connect(LastUsedFilesManager::instance(), &LastUsedFilesManager::lastUsedFilesChanged, this, &DockWelcome::updateRecentProjectsList);

    connect(this, &DockWelcome::newProject,            MainWindowInterface::instance(), &MainWindowInterface::newProject);
    connect(this, &DockWelcome::openProject,           MainWindowInterface::instance(), &MainWindowInterface::openProject);
    connect(this, &DockWelcome::openProjectFromFile,   MainWindowInterface::instance(), &MainWindowInterface::openProjectFromFile);
    connect(this, &DockWelcome::openPlugins,           MainWindowInterface::instance(), &MainWindowInterface::openPlugins);
    connect(this, &DockWelcome::openPreferences,       MainWindowInterface::instance(), &MainWindowInterface::openPreferences);
    connect(this, &DockWelcome::quitApplication,       MainWindowInterface::instance(), &MainWindowInterface::quitApplication);

    connect(this, &DockWelcome::removeProjectFromList, MainWindowInterface::instance(), &MainWindowInterface::removeProjectFromRecentList);
}

DockWelcome::~DockWelcome() {
    delete ui;
}

void DockWelcome::updateRecentProjectsList() {
    QStringList lastUsedFiles = LastUsedFilesManager::lastUsedFiles();

    int scrollbarPos = ui->lwRecentProjects->verticalScrollBar()->value();
    ui->lwRecentProjects->clear();

    for(QString current : lastUsedFiles) {
        QListWidgetItem *itm = new QListWidgetItem(ui->lwRecentProjects);
        ui->lwRecentProjects->addItem(itm);
        WdgWelcomeRecentProjectEntry *wdg = new WdgWelcomeRecentProjectEntry(ui->lwRecentProjects);

        QFileInfo fi(current);
        wdg->setIcon(QPixmap(":/Icons/classic/file.ico"));
        wdg->setName(fi.baseName());
        wdg->setPath(current);
        wdg->setLastUsed(fi.lastModified());

        if(!QFile::exists(current)) {
            wdg->setFileMissing();
            itm->setFlags(itm->flags() & ~Qt::ItemIsEnabled);
        }

        ui->lwRecentProjects->setItemWidget(itm, wdg);
        itm->setSizeHint(wdg->sizeHint());

        connect(wdg, &WdgWelcomeRecentProjectEntry::open,           this, &DockWelcome::openProjectFromFile);
        connect(wdg, &WdgWelcomeRecentProjectEntry::removeFromList, this, &DockWelcome::removeProjectFromList);
    }

    ui->lwRecentProjects->verticalScrollBar()->setValue(scrollbarPos);
}

void DockWelcome::on_pbToggleNews_clicked() {
    bool visible = ui->gbNews->isVisible();
    ui->pbToggleNews->setText(visible ? tr("Show News") : tr("Hide News"));
    ui->gbNews->setVisible(!visible);
}

void DockWelcome::onRecentFileOpen() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    emit openProjectFromFile(path);
}

void DockWelcome::onRecentFileOpenLocation() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    QFileInfo fi(path);
    QString dirPath = fi.dir().path();
    QDesktopServices::openUrl(QUrl(dirPath));
}

void DockWelcome::onRecentFileRemove() {
    QString path = currentRecentFilePath();
    if(path.isEmpty())
        return;

    // TODO
}

QString DockWelcome::currentRecentFilePath() const {
    if(!ui->lwRecentProjects->currentItem())
        return "";

    return qobject_cast<WdgWelcomeRecentProjectEntry *>(ui->lwRecentProjects->itemWidget(ui->lwRecentProjects->currentItem()))->path();
}

void DockWelcome::on_lwRecentProjects_itemDoubleClicked(QListWidgetItem *item) {
    if(!item || !item->flags().testFlag(Qt::ItemIsEnabled))
        return;

    QString path = qobject_cast<WdgWelcomeRecentProjectEntry *>(ui->lwRecentProjects->itemWidget(item))->path();

    emit openProjectFromFile(path);
}
