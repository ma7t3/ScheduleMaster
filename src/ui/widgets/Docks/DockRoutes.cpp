#include "DockRoutes.h"
#include "ui_DockRoutes.h"

#include <QDockWidget>
#include <QUndoStack>
#include <QMessageBox>

#include "Global/ActionController.h"
#include "Global/DockController.h"
#include "DockLines.h"
#include "src/projectdata/model/RouteTableModel.h"

#include "src/commands/CmdRoutes.h"

#include "src/projectdata/model/RouteTableModel.h"
#include "src/projectdata/model/RouteTableProxyModel.h"

#include "ApplicationInterface.h"
#include "src/ui/dialogs/DlgRouteEditor.h"

#include "Global/SettingsManager.h"

DockRoutes::DockRoutes(QWidget *parent) :
    DockAbstract(parent), ui(new Ui::DockRoutes), _projectData(ApplicationInterface::projectData()), _line(nullptr),
    _model(new RouteTableModel(this)) {
    ui->setupUi(this);

    DockLines *dockLines = dynamic_cast<DockLines *>(DockController::dock("lines")->widget());
    if(dockLines)
        connect(dockLines, &DockLines::currentLineChanged, this, &DockRoutes::setLine);

    // ACTION SETUP

    _actionNew         = setupAction();
    _actionEdit        = setupAction();
    _actionDuplicate   = setupAction();
    _actionDelete      = setupAction();
    _actionSearch      = setupAction();
    _actionFilter      = setupAction();
    _actionClearFilter = setupAction();

    ActionController::addSyncedActionAndButton(_actionNew,       ui->pbNew,       "projectData.item.new",       ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionEdit,      ui->pbEdit,      "projectData.item.edit",      ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDuplicate, ui->pbDuplicate, "projectData.item.duplicate", ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDelete,    ui->pbDelete,    "projectData.item.delete",    ActionController::AllComponents, ActionController::AllExceptShortcutComponent);

    ActionController::add(ui->tbFilter, "projectDataTable.filter.open", ActionController::AllExceptShortcutComponent);
    ActionController::add(_actionFilter, "projectDataTable.filter.open");

    ActionController::add(_actionClearFilter, "projectDataTable.filter.clear");
    ActionController::add(_actionSearch, "projectDataTable.search.focus");


    connect(_actionNew,       &QAction::triggered, this, &DockRoutes::onRouteNew);
    connect(_actionEdit,      &QAction::triggered, this, &DockRoutes::onRouteEdit);
    connect(_actionDuplicate, &QAction::triggered, this, &DockRoutes::onRouteDuplicate);
    connect(_actionDelete,    &QAction::triggered, this, &DockRoutes::onRouteDelete);
    connect(_actionFilter,    &QAction::triggered, this, [this](){ui->tbFilter->click();});

    ui->leSearch->setFocusAction(_actionSearch);

    // CONTEXT MENU

    ui->twRoutes->setItemContextMenu(globalMenu());
    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDuplicate);
    globalMenu()->addAction(_actionDelete);
    globalMenu()->addSeparator();
    globalMenu()->addAction(_actionFilter);
    globalMenu()->addAction(_actionClearFilter);


    // VIEW/MODEL SETUP

    _proxyModel = new RouteTableProxyModel(ui->tbFilter, this);
    _proxyModel->setFilterBanner(ui->filterBanner);
    _proxyModel->setClearFilterAction(_actionClearFilter);
    _proxyModel->setQuickSearchEdit(ui->leSearch);
    _proxyModel->setSourceModel(_model);

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
    _proxyModel->filterPopup()->setLine(_line);
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
    Route *r = _model->itemAt(_proxyModel->mapToSource(ui->twRoutes->currentIndex()).row());
    if(!r)
        return;

    DlgRouteEditor dlg(r->duplicate());
    if(dlg.exec() != QDialog::Accepted) {
        dlg.route()->deleteLater();
        return;
    }

    _projectData->undoStack()->push(new CmdRouteNew(_line, dlg.route()));
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
