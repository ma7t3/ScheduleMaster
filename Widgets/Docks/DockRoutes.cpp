#include "DockRoutes.h"
#include "ui_DockRoutes.h"

#include <QDockWidget>

#include "Global/ActionController.h"
#include "Global/DockController.h"
#include "DockLines.h"

#include "ProjectDataModels/RouteTableModel.h"
#include "ProjectDataModels/RouteTableProxyModel.h"

DockRoutes::DockRoutes(QWidget *parent) : DockAbstract(parent), ui(new Ui::DockRoutes), _line(nullptr),
    _model(new RouteTableModel(this)), _proxyModel(new RouteTableProxyModel(this)) {
    ui->setupUi(this);

    DockLines *dockLines = dynamic_cast<DockLines *>(DockController::dock("lines")->widget());
    if(dockLines)
        connect(dockLines, &DockLines::currentLineChanged, this, &DockRoutes::setLine);

    _actionNew = ui->twRoutes->addAction("");
    _actionNew->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionEdit = ui->twRoutes->addAction("");
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDuplicate = ui->twRoutes->addAction("");
    _actionDuplicate->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete = ui->twRoutes->addAction("");
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ActionController::add(_actionNew,       "projectData.item.new");
    ActionController::add(_actionEdit,      "projectData.item.edit");
    ActionController::add(_actionDuplicate, "projectData.item.duplicate");
    ActionController::add(_actionDelete,    "projectData.item.delete");

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

    ui->twRoutes->setContextMenuPolicy(Qt::ActionsContextMenu);

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
}

void DockRoutes::onRouteNew() {}

void DockRoutes::onRouteEdit() {}

void DockRoutes::onRouteDuplicate() {}

void DockRoutes::onRouteDelete() {}
