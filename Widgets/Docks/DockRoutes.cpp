#include "DockRoutes.h"
#include "ui_DockRoutes.h"

#include <QDockWidget>
#include <QUndoStack>
#include <QMessageBox>

#include "Global/ActionController.h"
#include "Global/DockController.h"
#include "DockLines.h"
#include "ProjectDataModels/RouteTableModel.h"

#include "Commands/CmdRoutes.h"

#include "ProjectDataModels/RouteTableModel.h"
#include "ProjectDataModels/RouteTableProxyModel.h"

#include "ApplicationInterface.h"
#include "Dialogs/DlgRouteEditor.h"

#include "Global/SettingsManager.h"

DockRoutes::DockRoutes(QWidget *parent) :
    DockAbstract(parent), ui(new Ui::DockRoutes), _projectData(ApplicationInterface::projectData()), _line(nullptr),
    _model(new RouteTableModel(this)), _proxyModel(new RouteTableProxyModel(this)) {
    ui->setupUi(this);

    DockLines *dockLines = dynamic_cast<DockLines *>(DockController::dock("lines")->widget());
    if(dockLines)
        connect(dockLines, &DockLines::currentLineChanged, this, &DockRoutes::setLine);

    // ACTION SETUP

    _actionNew = addAction("");
    _actionNew->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionEdit = addAction("");
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDuplicate = addAction("");
    _actionDuplicate->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete = addAction("");
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionSearch = addAction("");
    _actionSearch->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ActionController::addSyncedActionAndButton(_actionNew,       ui->pbNew,       "projectData.item.new",       ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionEdit,      ui->pbEdit,      "projectData.item.edit",      ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDuplicate, ui->pbDuplicate, "projectData.item.duplicate", ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDelete,    ui->pbDelete,    "projectData.item.delete",    ActionController::AllComponents, ActionController::AllExceptShortcutComponent);

    ActionController::add(_actionSearch,    "projectDataTable.search.focus");

    connect(_actionNew,       &QAction::triggered, this, &DockRoutes::onRouteNew);
    connect(_actionEdit,      &QAction::triggered, this, &DockRoutes::onRouteEdit);
    connect(_actionDuplicate, &QAction::triggered, this, &DockRoutes::onRouteDuplicate);
    connect(_actionDelete,    &QAction::triggered, this, &DockRoutes::onRouteDelete);
    connect(_actionSearch,    &QAction::triggered, ui->leSearch, [this]() { ui->leSearch->setFocus(); });

    // CONTEXT MENU

    connect(ui->twRoutes, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twRoutes->mapToGlobal(pos));
    });

    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDuplicate);
    globalMenu()->addAction(_actionDelete);


    // VIEW/MODEL SETUP

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortRole(Qt::DisplayRole);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->setFilterRole(Qt::DisplayRole);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->twRoutes->setModel(_proxyModel);
    ui->twRoutes->sortByColumn(1, Qt::AscendingOrder);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    ui->twRoutes->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->twRoutes, &QTableView::doubleClicked, this, &DockRoutes::onRouteEdit);

    connect(ui->twRoutes, &WdgProjectDataTableViewSignals::currentItemChanged, this, [this](){
        emit currentRouteChanged(currentRoute());
    });

    connect(ui->twRoutes, &WdgProjectDataTableViewSignals::selectedItemsChanged, this, [this](){
        emit selectedRoutesChaned(selectedRoutes());
    });

    ui->twRoutes->addSelectionDependentAction(_actionEdit,      [](const int &n) { return n == 1; });
    ui->twRoutes->addSelectionDependentAction(_actionDuplicate, [](const int &n) { return n == 1; });
    ui->twRoutes->addSelectionDependentAction(_actionDelete,    [](const int &n) { return n > 0; });

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterWildcard);

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twRoutes, ui->tbColumns, this);

    setLine(nullptr);
}

DockRoutes::~DockRoutes() {
    delete ui;
}

Line *DockRoutes::currentLine() const {
    return _line;
}

void DockRoutes::setLine(Line *line) {
    _line = line;
    _model->setLine(line);
    _actionNew->setEnabled(_line);
}

Route *DockRoutes::currentRoute() const {
    return ui->twRoutes->currentItem();
}

PDISet<Route> DockRoutes::selectedRoutes() const {
    return ui->twRoutes->selectedItems();
}

void DockRoutes::onRouteNew() {
    if(!_line)
        return;

    Route *r = _line->createRoute();
    DlgRouteEditor dlg(r);
    if(dlg.exec() != QDialog::Accepted) {
        r->deleteLater();
        return;
    }

    _projectData->undoStack()->push(new CmdRouteNew(_line, r));
}

void DockRoutes::onRouteEdit() {
    Route *r = _model->itemAt(_proxyModel->mapToSource(ui->twRoutes->currentIndex()).row());
    if(!r)
        return;

    DlgRouteEditor dlg(r->clone());
    if(dlg.exec() != QDialog::Accepted)
        return;

    _projectData->undoStack()->push(new CmdRouteEdit(r, dlg.route()));
}

void DockRoutes::onRouteDuplicate() {
    //TODO
}

void DockRoutes::onRouteDelete() {
    const PDISet<Route> routes = selectedRoutes();
    QStringList bulletList;
    for(Route *r : routes) {
        if(bulletList.count() >= SettingsManager::value("general.deleteDialog.maxListCount").toInt())
            break;
        bulletList << QString("<li>%1</li>").arg(r->name());
    }

    bulletList.sort();

    QMessageBox::StandardButton msg = QMessageBox::warning(
        this,
        tr("Delete routes(s)"),
        tr("<p><b>Do you really want to delete these %n routes(s)?</b></p><ul>%1</ul>%2",
           "",
           routes.count())
            .arg(bulletList.join(""))
            .arg(bulletList.count() < routes.count()
                     ? tr("<i>%n more</i>", "", routes.count() - bulletList.count())
                     : ""),
        QMessageBox::Yes | QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _projectData->undoStack()->push(new CmdRoutesRemove(_line, routes));
}
