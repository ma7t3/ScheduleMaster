#include "DockWelcome.h"
#include "ui_DockWelcome.h"

#include "namespace.h"
#include "core/LastUsedFilesServiceImpl.h"
#include "core/ActionServiceImpl.h"
#include "ui/widgets/WdgWelcomeRecentProjectEntry.h"


#include "../ApplicationInterface.h"

#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QScrollBar>

DockWelcome::DockWelcome(QWidget *parent) :
    DockAbstract(parent),
    ui(new Ui::DockWelcome) {
    ui->setupUi(this);

    ui->lIcon->setPixmap(QPixmap(":/icons/ScheduleMaster_64px.ico"));

    _recentFileOpen         = ui->lwRecentProjects->addAction("");
    _recentFileOpenLocation = ui->lwRecentProjects->addAction("");
    _recentFileRemove       = ui->lwRecentProjects->addAction("");
    ui->lwRecentProjects->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_recentFileOpen,         &QAction::triggered,   this, &DockWelcome::onRecentFileOpen);
    connect(ui->pbOpen,              &QPushButton::clicked, this, &DockWelcome::onRecentFileOpen);
    connect(_recentFileOpenLocation, &QAction::triggered,   this, &DockWelcome::onRecentFileOpenLocation);
    connect(_recentFileRemove,       &QAction::triggered,   this, &DockWelcome::onRecentFileRemove);
    updateRecentProjectsList();

    connect(ui->clbNewProject,  &QCommandLinkButton::clicked, this, &DockWelcome::newProject);
    connect(ui->clbOpenProject, &QCommandLinkButton::clicked, this, &DockWelcome::openProject);
    connect(ui->clbPlugins,     &QCommandLinkButton::clicked, this, &DockWelcome::openPlugins);
    connect(ui->clbPreferences, &QCommandLinkButton::clicked, this, &DockWelcome::openPreferences);
    connect(ui->clbQuit,        &QCommandLinkButton::clicked, this, &DockWelcome::quitApplication);

    SM::ActionServiceImpl::instance()->addButton(ui->clbNewProject,       "project.new",                          SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->clbOpenProject,      "project.open",                         SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->clbPlugins,          "application.preferences.plugins.open", SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->clbPreferences,      "application.preferences.open",         SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->clbHelp,             "application.help.open",                SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);
    SM::ActionServiceImpl::instance()->addButton(ui->clbQuit,             "application.quit",                     SM::ActionServiceImpl::instance()->AllExceptShortcutComponent);

    SM::ActionServiceImpl::instance()->addAction(_recentFileOpen,         "project.recentFiles.openItem");
    SM::ActionServiceImpl::instance()->addAction(_recentFileOpenLocation, "project.recentFiles.openItemDirectory");
    SM::ActionServiceImpl::instance()->addAction(_recentFileRemove,       "project.recentFiles.removeItem");

    connect(SM::LastUsedFilesServiceImpl::instance(), &SM::LastUsedFilesServiceImpl::lastUsedFilesChanged, this, &DockWelcome::updateRecentProjectsList);

    connect(this, &DockWelcome::newProject,            ApplicationInterface::instance(), &ApplicationInterface::newProject);
    connect(this, &DockWelcome::openProject,           ApplicationInterface::instance(), &ApplicationInterface::openProject);
    connect(this, &DockWelcome::openProjectFromFile,   ApplicationInterface::instance(), &ApplicationInterface::openProjectFromFile);
    connect(this, &DockWelcome::openPlugins,           ApplicationInterface::instance(), &ApplicationInterface::openPlugins);
    connect(this, &DockWelcome::openPreferences,       ApplicationInterface::instance(), &ApplicationInterface::openPreferences);
    connect(this, &DockWelcome::quitApplication,       ApplicationInterface::instance(), &ApplicationInterface::quitApplication);

    connect(this, &DockWelcome::removeProjectFromList, ApplicationInterface::instance(), &ApplicationInterface::removeProjectFromRecentList);
}

DockWelcome::~DockWelcome() {
    delete ui;
}

void DockWelcome::updateRecentProjectsList() {
    const QStringList lastUsedFiles = SM::LastUsedFilesServiceImpl::instance()->lastUsedFiles();

    int scrollbarPos = ui->lwRecentProjects->verticalScrollBar()->value();
    ui->lwRecentProjects->clear();

    for(QString current : lastUsedFiles) {
        QListWidgetItem *itm = new QListWidgetItem(ui->lwRecentProjects);
        ui->lwRecentProjects->addItem(itm);
        WdgWelcomeRecentProjectEntry *wdg = new WdgWelcomeRecentProjectEntry(ui->lwRecentProjects);

        QFileInfo fi(current);
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

    emit removeProjectFromList(path);
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
