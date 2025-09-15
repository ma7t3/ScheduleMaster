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
    _currentRoute(nullptr), _model(new RouteTableModel(this)), _proxyModel(new RouteTableProxyModel(this)) {
    ui->setupUi(this);

    DockLines *dockLines = dynamic_cast<DockLines *>(DockController::dock("lines")->widget());
    if(dockLines)
        connect(dockLines, &DockLines::currentLineChanged, this, &DockRoutes::setLine);

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

    ActionController::add(_actionNew,       "projectData.item.new");
    ActionController::add(_actionEdit,      "projectData.item.edit");
    ActionController::add(_actionDuplicate, "projectData.item.duplicate");
    ActionController::add(_actionDelete,    "projectData.item.delete");
    ActionController::add(_actionSearch,    "projectDataTable.focusSearch");

    ActionController::add(ui->pbNew,        "projectData.item.new",       ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbEdit,       "projectData.item.edit",      ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbDuplicate,  "projectData.item.duplicate", ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbDelete,     "projectData.item.delete",    ActionController::AllExceptShortcutComponent);

    connect(_actionNew,       &QAction::enabledChanged, ui->pbNew,       &QPushButton::setEnabled);
    connect(_actionEdit,      &QAction::enabledChanged, ui->pbEdit,      &QPushButton::setEnabled);
    connect(_actionDuplicate, &QAction::enabledChanged, ui->pbDuplicate, &QPushButton::setEnabled);
    connect(_actionDelete,    &QAction::enabledChanged, ui->pbDelete,    &QPushButton::setEnabled);

    connect(ui->pbNew,       &QPushButton::clicked, _actionNew,       &QAction::trigger);
    connect(ui->pbEdit,      &QPushButton::clicked, _actionEdit,      &QAction::trigger);
    connect(ui->pbDuplicate, &QPushButton::clicked, _actionDuplicate, &QAction::trigger);
    connect(ui->pbDelete,    &QPushButton::clicked, _actionDelete,    &QAction::trigger);

    connect(_actionNew,       &QAction::triggered, this, &DockRoutes::onRouteNew);
    connect(_actionEdit,      &QAction::triggered, this, &DockRoutes::onRouteEdit);
    connect(_actionDuplicate, &QAction::triggered, this, &DockRoutes::onRouteDuplicate);
    connect(_actionDelete,    &QAction::triggered, this, &DockRoutes::onRouteDelete);

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

    connect(ui->twRoutes, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twRoutes->mapToGlobal(pos));
    });

    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDuplicate);
    globalMenu()->addAction(_actionDelete);

    connect(ui->twRoutes, &QTableView::doubleClicked, this, &DockRoutes::onRouteEdit);
    connect(ui->twRoutes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DockRoutes::onSelectionChanged);

    connect(_model, &UnorderedProjectDataRowModelSignals::multipleRowsInserted, this, &DockRoutes::onRowsAdded);
    connect(_model, &QAbstractItemModel::modelReset, this, &DockRoutes::onSelectionChanged);

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterWildcard);

    connect(_actionSearch, &QAction::triggered, ui->leSearch, [this](){ui->leSearch->setFocus();});

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twRoutes, this);

    ActionController::add(ui->tbColumns, "projectDataTable.showHideColumns");
    ui->tbColumns->setMenu(_columnVisibilitySelector->menu());

    ui->twRoutes->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twRoutes->horizontalHeader(),
            &QHeaderView::customContextMenuRequested,
            this,
            [this](const QPoint &pos) {
                _columnVisibilitySelector->menu()->popup(
                    ui->twRoutes->horizontalHeader()->mapToGlobal(pos));
            });

    setLine(nullptr);
    onSelectionChanged();
}

DockRoutes::~DockRoutes() {
    delete ui;
}

Line *DockRoutes::currentLine() const {
    return _line;
}

Route *DockRoutes::currentRoute() const {
    return _currentRoute;
}

PDISet<Route> DockRoutes::selectedRoutes() const {
    const QModelIndexList list = ui->twRoutes->selectionModel()->selectedRows();
    PDISet<Route> routes;
    for(const QModelIndex &index : list) {
        Route *r = _model->itemAt(_proxyModel->mapToSource(index).row());
        routes.add(r);
    }
    return routes;
}

void DockRoutes::setLine(Line *line) {
    _line = line;
    _model->setLine(line);
    _actionNew->setEnabled(_line);
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

void DockRoutes::onSelectionChanged() {
    const QModelIndex current = ui->twRoutes->currentIndex();
    const QModelIndexList list = ui->twRoutes->selectionModel()->selectedRows();
    const int count = list.count();

    _actionEdit->setEnabled(count == 1);
    _actionDuplicate->setEnabled(count == 1);
    _actionDelete->setEnabled(count > 0);

    if(current.isValid() && count == 1)
        _currentRoute = _model->itemAt(_proxyModel->mapToSource(current).row());
    else
        _currentRoute = nullptr;

    emit currentRouteChanged(_currentRoute);
    emit selectedRoutesChaned(selectedRoutes());
}

void DockRoutes::onRowsAdded(const QList<QPersistentModelIndex> &indexes) {
    // This is a quick and dirty fix to prevent selecting one random item when loading a file.....
    if(indexes.count() == _projectData->lines().count())
        return;

    ui->twRoutes->clearSelection();
    for(const QPersistentModelIndex &index : indexes)
        ui->twRoutes->selectRow(_proxyModel->mapFromSource(index).row());
}
