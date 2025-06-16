#include "DockBusstops.h"
#include "ui_DockBusstops.h"

#include "Global/ActionController.h"

#include <QInputDialog>
#include "ApplicationInterface.h"

DockBusstops::DockBusstops(QWidget *parent) :
    DockAbstract(parent), ui(new Ui::DockBusstops),
    _model(new BusstopTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _projectData(ApplicationInterface::projectData()) {
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
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->twBusstops->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(_searchAction, &QAction::triggered, ui->leSearch, [this](){ui->leSearch->setFocus();});
}

DockBusstops::~DockBusstops() {
    delete ui;
}

void DockBusstops::onBusstopNew() {
    const QString name = QInputDialog::getText(this, "TEST - New Busstop", "Name: ");
    Busstop *b = new Busstop(_projectData);
    b->setName(name);
    _projectData->addBusstop(b);
}

void DockBusstops::onBusstopEdit() {
    Busstop *b = _model->itemAt(_proxyModel->mapToSource(ui->twBusstops->currentIndex()).row());
    if(!b)
        return;

    const QString newName = QInputDialog::getText(this, "TEST - New Busstop", "Name: ", QLineEdit::Normal, b->name());
    b->setName(newName);
}

void DockBusstops::onBusstopDelete() {
    const QModelIndexList list = ui->twBusstops->selectionModel()->selectedRows();
    QList<Busstop *> busstops;
    for(const QModelIndex &index : list) {
        Busstop *b = _model->itemAt(_proxyModel->mapToSource(index).row());
        if(!b)
            return;

        busstops << b;
    }
    for(Busstop *b : std::as_const(busstops)) {
        _projectData->removeBusstop(b);
    }
}
