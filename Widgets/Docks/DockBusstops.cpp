#include "DockBusstops.h"
#include "ui_DockBusstops.h"

#include <QMessageBox>

#include "Global/ActionController.h"
#include "Global/SettingsManager.h"
#include "ApplicationInterface.h"
#include "Dialogs/DlgBusstopEditor.h"

#include "Commands/CmdBusstops.h"

DockBusstops::DockBusstops(QWidget *parent) :
    DockAbstract(parent), ui(new Ui::DockBusstops),
    _model(new BusstopTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _delegate(new BusstopTableModelDelegate(this)),
    _projectData(ApplicationInterface::projectData()),
    _currentBusstop(nullptr) {
    ui->setupUi(this);

    _newAction = addAction("");
    _newAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _editAction = addAction("");
    _editAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _deleteAction = addAction("");
    _deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _searchAction = addAction("");
    _searchAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ActionController::add(_newAction,    "projectData.item.new");
    ActionController::add(_editAction,   "projectData.item.edit");
    ActionController::add(_deleteAction, "projectData.item.delete");
    ActionController::add(_searchAction, "application.preferences.keyboardShortcuts.focusSearch");

    ActionController::add(ui->tbNew,    "projectData.item.new", ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->tbEdit,   "projectData.item.edit", ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->tbDelete, "projectData.item.delete", ActionController::AllExceptShortcutComponent);

    connect(_newAction,    &QAction::enabledChanged, ui->tbNew,    &QPushButton::setEnabled);
    connect(_editAction,   &QAction::enabledChanged, ui->tbEdit,   &QPushButton::setEnabled);
    connect(_deleteAction, &QAction::enabledChanged, ui->tbDelete, &QPushButton::setEnabled);

    connect(ui->tbNew,    &QPushButton::clicked, _newAction,    &QAction::trigger);
    connect(ui->tbEdit,   &QPushButton::clicked, _editAction,   &QAction::trigger);
    connect(ui->tbDelete, &QPushButton::clicked, _deleteAction, &QAction::trigger);

    connect(_newAction, &QAction::triggered,    this, &DockBusstops::onBusstopNew);
    connect(_editAction, &QAction::triggered,   this, &DockBusstops::onBusstopEdit);
    connect(_deleteAction, &QAction::triggered, this, &DockBusstops::onBusstopDelete);

    ui->twBusstops->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->twBusstops, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twBusstops->mapToGlobal(pos));
    });

    globalMenu()->addAction(_newAction);
    globalMenu()->addAction(_editAction);
    globalMenu()->addAction(_deleteAction);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortRole(Qt::DisplayRole);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->setFilterRole(Qt::DisplayRole);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->twBusstops->setModel(_proxyModel);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    ui->twBusstops->setItemDelegateForColumn(1, _delegate);
    ui->twBusstops->setItemDelegateForColumn(2, _delegate);
    ui->twBusstops->setItemDelegateForColumn(3, _delegate);

    connect(ui->twBusstops, &QTableView::doubleClicked, this, &DockBusstops::onBusstopEdit);
    connect(ui->twBusstops->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DockBusstops::onSelectionChanged);

    connect(_model, &UnorderedProjectDataRowModelSignals::multipleRowsInserted, this, &DockBusstops::onRowsAdded);
    connect(_model, &QAbstractItemModel::modelReset, this, &DockBusstops::onSelectionChanged);

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(_searchAction, &QAction::triggered, ui->leSearch, [this](){ui->leSearch->setFocus();});

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twBusstops, this);

    ActionController::add(ui->tbColumns, "projectDataTable.showHideColumns");
    ui->tbColumns->setMenu(_columnVisibilitySelector->menu());

    ui->twBusstops->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twBusstops->horizontalHeader(),
            &QHeaderView::customContextMenuRequested,
            this,
            [this](const QPoint &pos) {
                _columnVisibilitySelector->menu()->popup(
                    ui->twBusstops->horizontalHeader()->mapToGlobal(pos));
            });

    onSelectionChanged();
}

DockBusstops::~DockBusstops() {
    delete ui;
}

Busstop *DockBusstops::currentBusstop() const {
    return _currentBusstop;
}

PDISet<Busstop> DockBusstops::selectedBusstops() const {
    const QModelIndexList list = ui->twBusstops->selectionModel()->selectedRows();
    PDISet<Busstop> busstops;
    for(const QModelIndex &index : list) {
        Busstop *b = _model->itemAt(_proxyModel->mapToSource(index).row());
        busstops.add(b);
    }
    return busstops;
}

void DockBusstops::onBusstopNew() {
    Busstop *b = _projectData->createBusstop();
    DlgBusstopEditor dlg(b);
    if(dlg.exec() != QDialog::Accepted) {
        b->deleteLater();
        return;
    }

    _projectData->undoStack()->push(new CmdBusstopNew(_projectData, b));
}

void DockBusstops::onBusstopEdit() {
    Busstop *b = _model->itemAt(_proxyModel->mapToSource(ui->twBusstops->currentIndex()).row());
    if(!b)
        return;

    DlgBusstopEditor dlg(b->clone());
    if(dlg.exec() != QDialog::Accepted)
        return;

    _projectData->undoStack()->push(new CmdBusstopEdit(b, dlg.busstop()));
}

void DockBusstops::onBusstopDelete() {
    const PDISet<Busstop> busstops = selectedBusstops();
    QStringList bulletList;
    for(Busstop *b : busstops) {
        if(bulletList.count() >= SettingsManager::value("general.deleteDialog.maxListCount").toInt())
            break;
        bulletList << QString("<li>%1</li>").arg(b->name());
    }

    bulletList.sort();

    QMessageBox::StandardButton msg = QMessageBox::warning(
        this,
        tr("Delete busstop(s)"),
        tr("<p><b>Do you really want to delete these %n busstop(s)?</b></p><ul>%1</ul>%2",
           "",
           busstops.count())
            .arg(bulletList.join(""))
            .arg(bulletList.count() < busstops.count()
                     ? tr("<i>%n more</i>", "", busstops.count() - bulletList.count())
                     : ""),
        QMessageBox::Yes | QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _projectData->undoStack()->push(new CmdBusstopsRemove(_projectData, busstops));
}

void DockBusstops::onSelectionChanged() {
    const QModelIndex current = ui->twBusstops->currentIndex();
    const QModelIndexList list = ui->twBusstops->selectionModel()->selectedRows();
    const int count = list.count();

    _editAction->setEnabled(count == 1);
    _deleteAction->setEnabled(count > 0);

    if(current.isValid() && count == 1)
        _currentBusstop = _model->itemAt(_proxyModel->mapToSource(current).row());
    else
        _currentBusstop = nullptr;

    emit currentBusstopChanged(_currentBusstop);
    emit selectedBusstopsChaned(selectedBusstops());
}

void DockBusstops::onRowsAdded(const QList<QPersistentModelIndex> &indexes) {
    // This is a quick and dirty fix to prevent selecting one random item when loading a file.....
    if(indexes.count() == _projectData->busstops().count())
        return;

    ui->twBusstops->clearSelection();
    for(const QPersistentModelIndex &index : indexes)
        ui->twBusstops->selectRow(_proxyModel->mapFromSource(index).row());
}
