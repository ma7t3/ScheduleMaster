#include "DlgRouteEditor.h"
#include "ui_DlgRouteEditor.h"

#include <QSortFilterProxyModel>

#include "Global/ActionController.h"
#include "ProjectData/Line.h"
#include "ProjectDataModels/LineDirectionTableModel.h"
#include "ProjectDataModels/BusstopTableModel.h"
#include "ProjectDataModels/RouteBusstopTableModel.h"

#include "ApplicationInterface.h"

DlgRouteEditor::DlgRouteEditor(Route *route, QWidget *parent) :
    QDialog(parent), ui(new Ui::DlgRouteEditor), _route(route),
    _directionModel(new LineDirectionTableModel(this)),
    _allBusstopsModel(new BusstopTableModel(this)),
    _allBusstopsProxyModel(new QSortFilterProxyModel(this)),
    _routeBusstopModel(new RouteBusstopTableModel(this)),
    _routeBusstopDelegate(new RouteBusstopTableDelegate(this)) {
    ui->setupUi(this);

    _projectData = ApplicationInterface::projectData();
    _line = dynamic_cast<Line *>(_route->parent());

    setWindowTitle(route->isClone() ? tr("Edit Route") : "Create Route");

    _actionBusstopAdd = ui->lwAllBusstops->addAction("");
    _actionBusstopAdd->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionBusstopRemove = ui->twRouteBusstops->addAction("");
    _actionBusstopRemove->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionBusstopUp = ui->twRouteBusstops->addAction("");
    _actionBusstopUp->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionBusstopDown = ui->twRouteBusstops->addAction("");
    _actionBusstopDown->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionBusstopsReverse = ui->twRouteBusstops->addAction("");
    _actionBusstopsReverse->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionProfileNew = ui->twProfiles->addAction("");
    _actionProfileNew->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionProfileDelete = ui->twProfiles->addAction("");
    _actionProfileDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionProfileUp = ui->twProfiles->addAction("");
    _actionProfileUp->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionProfileDown = ui->twProfiles->addAction("");
    _actionProfileDown->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ActionController::add(ui->pbBusstopAdd,      "projectData.item.add",      ActionController::IconComponent|ActionController::TooltipComponent);
    ActionController::add(ui->pbBusstopRemove,   "projectData.item.remove",   ActionController::IconComponent|ActionController::TooltipComponent);
    ActionController::add(ui->pbBusstopUp,       "projectData.item.moveUp",   ActionController::IconComponent|ActionController::TooltipComponent);
    ActionController::add(ui->pbBusstopDown,     "projectData.item.moveDown", ActionController::IconComponent|ActionController::TooltipComponent);
    ActionController::add(ui->pbBusstopsReverse, "projectDataList.reverse",   ActionController::IconComponent|ActionController::TooltipComponent);

    ActionController::add(_actionBusstopAdd,      "projectData.item.add");
    ActionController::add(_actionBusstopRemove,   "projectData.item.remove");
    ActionController::add(_actionBusstopUp,       "projectData.item.moveUp");
    ActionController::add(_actionBusstopDown,     "projectData.item.moveDown");
    ActionController::add(_actionBusstopsReverse, "projectDataList.reverse");

    ActionController::add(ui->pbProfileNew,       "projectData.item.new",      ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbProfileDelete,    "projectData.item.delete",   ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbProfileUp,        "projectData.item.moveUp",   ActionController::IconComponent|ActionController::TooltipComponent);
    ActionController::add(ui->pbProfileDown,      "projectData.item.moveDown", ActionController::IconComponent|ActionController::TooltipComponent);

    ActionController::add(_actionProfileNew,      "projectData.item.new");
    ActionController::add(_actionProfileDelete,   "projectData.item.delete");
    ActionController::add(_actionProfileUp,       "projectData.item.moveUp");
    ActionController::add(_actionProfileDown,     "projectData.item.moveDown");

    connect(ui->pbBusstopAdd,      &QPushButton::clicked, this, &DlgRouteEditor::onBusstopAdd);
    connect(ui->pbBusstopRemove,   &QPushButton::clicked, this, &DlgRouteEditor::onBusstopRemove);
    connect(ui->pbBusstopUp,       &QPushButton::clicked, this, &DlgRouteEditor::onBusstopUp);
    connect(ui->pbBusstopDown,     &QPushButton::clicked, this, &DlgRouteEditor::onBusstopDown);
    connect(ui->pbBusstopsReverse, &QPushButton::clicked, this, &DlgRouteEditor::onBusstopsReverse);

    connect(_actionBusstopAdd,      &QAction::triggered, this, &DlgRouteEditor::onBusstopAdd);
    connect(_actionBusstopRemove,   &QAction::triggered, this, &DlgRouteEditor::onBusstopRemove);
    connect(_actionBusstopUp,       &QAction::triggered, this, &DlgRouteEditor::onBusstopUp);
    connect(_actionBusstopDown,     &QAction::triggered, this, &DlgRouteEditor::onBusstopDown);
    connect(_actionBusstopsReverse, &QAction::triggered, this, &DlgRouteEditor::onBusstopsReverse);

    connect(ui->lwAllBusstops, &QAbstractItemView::doubleClicked, this, &DlgRouteEditor::onBusstopAdd);

    ui->lwAllBusstops->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->twRouteBusstops->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->twProfiles->setContextMenuPolicy(Qt::ActionsContextMenu);

    _directionModel->setLine(_line);
    ui->cbDirection->setModel(_directionModel);

    _allBusstopsProxyModel->setSourceModel(_allBusstopsModel);
    _allBusstopsProxyModel->sort(0, Qt::AscendingOrder);
    _allBusstopsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->lwAllBusstops->setModel(_allBusstopsProxyModel);

    _routeBusstopModel->setRoute(_route);
    ui->twRouteBusstops->setModel(_routeBusstopModel);

    ui->twRouteBusstops->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twRouteBusstops->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->twRouteBusstops->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    ui->twRouteBusstops->setItemDelegateForColumn(2, _routeBusstopDelegate);

    connect(ui->leBusstopsSearch, &QLineEdit::textChanged, _allBusstopsProxyModel, &QSortFilterProxyModel::setFilterWildcard);

    ui->pteComment->setPlainText(_route->comment());
    ui->leName->setText(route->name());
    ui->sbCode->setValue(route->code());
    ui->cbDirection->setCurrentIndex(_directionModel->indexOfItem(_route->direction()));
}

DlgRouteEditor::~DlgRouteEditor() {
    delete ui;
}

Route *DlgRouteEditor::route() {
    return _route;
}

void DlgRouteEditor::onBusstopAdd() {
    Busstop *source = _allBusstopsModel->itemAt(_allBusstopsProxyModel->mapToSource(ui->lwAllBusstops->currentIndex()).row());
    if(!source)
        return;

    RouteBusstopItem *b = _route->createItem(source);
    _route->appendBusstop(b);
    ui->twRouteBusstops->setCurrentIndex(_routeBusstopModel->index(_routeBusstopModel->rowCount() - 1, 0));
}

void DlgRouteEditor::onBusstopRemove() {
    const int row = ui->twRouteBusstops->currentIndex().row();
    RouteBusstopItem *b = _routeBusstopModel->itemAt(row);
    if(!b)
        return;

    _route->removeBusstop(b);
}

void DlgRouteEditor::onBusstopUp() {
    const int row = ui->twRouteBusstops->currentIndex().row();
    if(row <= 0)
        return;

    _route->moveBusstop(row, row - 1);
}

void DlgRouteEditor::onBusstopDown() {
    const int row = ui->twRouteBusstops->currentIndex().row();
    if(row >= _routeBusstopModel->rowCount() - 1)
        return;

    _route->moveBusstop(row, row + 1);
}

void DlgRouteEditor::onBusstopsReverse() {
    _route->reverseBusstopOrder();
}

void DlgRouteEditor::accept() {
    _route->setComment(ui->pteComment->toPlainText());
    _route->setName(ui->leName->text());
    _route->setCode(ui->sbCode->value());
    _route->setDirection(_directionModel->itemAt(ui->cbDirection->currentIndex()));
    QDialog::accept();
}
