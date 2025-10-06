#include "DlgRouteEditor.h"
#include "ui_DlgRouteEditor.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QInputDialog>

#include "Global/ActionController.h"
#include "ProjectData/Line.h"
#include "ProjectDataModels/LineDirectionTableModel.h"
#include "ProjectDataModels/BusstopTableModel.h"
#include "ProjectDataModels/RouteBusstopTableModel.h"
#include "ProjectDataModels/TimeProfileTableModel.h"

#include "ApplicationInterface.h"

DlgRouteEditor::DlgRouteEditor(Route *route, QWidget *parent) :
    QDialog(parent), ui(new Ui::DlgRouteEditor), _route(route),
    _directionModel(new LineDirectionTableModel(this)),
    _allBusstopsModel(new BusstopTableModel(this)),
    _allBusstopsProxyModel(new QSortFilterProxyModel(this)),
    _routeBusstopModel(new RouteBusstopTableModel(this)),
    _routeBusstopDelegate(new RouteBusstopTableDelegate(this)),
    _timeProfileModel(new TimeProfileTableModel(this)) {
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

    ActionController::addSyncedActionAndButton(_actionBusstopAdd,
                                               ui->pbBusstopAdd,
                                               "projectData.item.add",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionBusstopRemove,
                                               ui->pbBusstopRemove,
                                               "projectData.item.remove",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionBusstopUp,
                                               ui->pbBusstopUp,
                                               "projectData.item.moveUp",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionBusstopDown,
                                               ui->pbBusstopDown,
                                               "projectData.item.moveDown",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionBusstopsReverse,
                                               ui->pbBusstopsReverse,
                                               "projectDataList.reverse",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionProfileNew,
                                               ui->pbProfileNew,
                                               "projectData.item.new",
                                               ActionController::AllComponents,
                                               ActionController::AllExceptShortcutComponent);

    ActionController::addSyncedActionAndButton(_actionProfileDelete,
                                               ui->pbProfileDelete,
                                               "projectData.item.delete",
                                               ActionController::AllComponents,
                                               ActionController::AllExceptShortcutComponent);

    ActionController::addSyncedActionAndButton(_actionProfileUp,
                                               ui->pbProfileUp,
                                               "projectData.item.moveUp",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    ActionController::addSyncedActionAndButton(_actionProfileDown,
                                               ui->pbProfileDown,
                                               "projectData.item.moveDown",
                                               ActionController::AllComponents,
                                               ActionController::IconComponent
                                                   | ActionController::TooltipComponent);

    connect(_actionBusstopAdd,      &QAction::triggered, this, &DlgRouteEditor::onBusstopAdd);
    connect(_actionBusstopRemove,   &QAction::triggered, this, &DlgRouteEditor::onBusstopRemove);
    connect(_actionBusstopUp,       &QAction::triggered, this, &DlgRouteEditor::onBusstopUp);
    connect(_actionBusstopDown,     &QAction::triggered, this, &DlgRouteEditor::onBusstopDown);
    connect(_actionBusstopsReverse, &QAction::triggered, this, &DlgRouteEditor::onBusstopsReverse);

    connect(_actionProfileNew,      &QAction::triggered, this, &DlgRouteEditor::onProfileNew);
    connect(_actionProfileDelete,   &QAction::triggered, this, &DlgRouteEditor::onProfileDelete);
    connect(_actionProfileUp,       &QAction::triggered, this, &DlgRouteEditor::onProfileUp);
    connect(_actionProfileDown,     &QAction::triggered, this, &DlgRouteEditor::onProfileDown);

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

    _timeProfileModel->setRoute(_route);
    ui->twProfiles->setModel(_timeProfileModel);

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

    const QModelIndex insertIndex = ui->twRouteBusstops->currentIndex();
    const int insertRow = insertIndex.isValid() ? insertIndex.row() + 1 : _routeBusstopModel->rowCount();

    RouteBusstopItem *b = _route->createItem(source);
    _route->insertBusstop(insertRow, b);
    ui->twRouteBusstops->setCurrentIndex(_routeBusstopModel->index(insertRow, 0));
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

void DlgRouteEditor::onProfileNew() {
    bool ok;
    const QString name = QInputDialog::getText(this, tr("Create Time Profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    TimeProfile *p = _route->createTimeProfile();
    p->setName(name);
    _route->appendTimeProfile(p);
    ui->twProfiles->setCurrentIndex(_timeProfileModel->index(_timeProfileModel->rowCount() - 1, 0));
}

void DlgRouteEditor::onProfileDelete() {
    const int row = ui->twProfiles->currentIndex().row();
    TimeProfile *p = _timeProfileModel->itemAt(row);
    if(!p)
        return;

    const QMessageBox::StandardButton msg = QMessageBox::warning(
        this,
        tr("Delete Time Profile"),
        tr("<p><b>Do you really want to delete this time profile?</b><p><ul><li>%1</li></ul>")
            .arg(p->name()),
        QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    _route->removeTimeProfile(p);
}

void DlgRouteEditor::onProfileUp() {
    const int row = ui->twProfiles->currentIndex().row();
    if(row <= 0)
        return;

    _route->moveTimeProfile(row, row - 1);
}

void DlgRouteEditor::onProfileDown() {
    const int row = ui->twProfiles->currentIndex().row();
    if(row >= _timeProfileModel->rowCount() - 1)
        return;

    _route->moveTimeProfile(row, row + 1);
}

void DlgRouteEditor::accept() {
    _route->setComment(ui->pteComment->toPlainText());
    _route->setName(ui->leName->text());
    _route->setCode(ui->sbCode->value());
    _route->setDirection(_directionModel->itemAt(ui->cbDirection->currentIndex()));
    QDialog::accept();
}

void DlgRouteEditor::reject() {
    // TODO
    QDialog::reject();
}
