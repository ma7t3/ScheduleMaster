#include "DockWelcome.h"
#include "ui_DockWelcome.h"

#include "Global/ActionController.h"
#include "Global/LastUsedFilesManager.h"
#include "ApplicationInterface.h"
#include "src/ui/widgets/WdgWelcomeRecentProjectEntry.h"

#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QScrollBar>

DockWelcome::DockWelcome(QWidget *parent) :
    DockAbstract(parent),
    ui(new Ui::DockWelcome) {
    ui->setupUi(this);

    ui->lIcon->setPixmap(QPixmap(":/Icons/ScheduleMaster_64px.ico"));

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

    ActionController::add(ui->clbNewProject,       "project.new",                          ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->clbOpenProject,      "project.open",                         ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->clbPlugins,          "application.preferences.plugins.open", ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->clbPreferences,      "application.preferences.open",         ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->clbHelp,             "application.help.open",                ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->clbQuit,             "application.quit",                     ActionController::AllExceptShortcutComponent);

    ActionController::add(_recentFileOpen,         "project.recentFiles.openItem");
    ActionController::add(_recentFileOpenLocation, "project.recentFiles.openItemDirectory");
    ActionController::add(_recentFileRemove,       "project.recentFiles.removeItem");

    connect(LastUsedFilesManager::instance(), &LastUsedFilesManager::lastUsedFilesChanged, this, &DockWelcome::updateRecentProjectsList);

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
    QStringList lastUsedFiles = LastUsedFilesManager::lastUsedFiles();

    int scrollbarPos = ui->lwRecentProjects->verticalScrollBar()->value();
    ui->lwRecentProjects->clear();

    for(QString current : std::as_const(lastUsedFiles)) {
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
