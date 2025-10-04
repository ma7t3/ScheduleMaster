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
    _delegate(new BusstopTableModelDelegate(this)),
    _projectData(ApplicationInterface::projectData()) {
    ui->setupUi(this);

    // ACTION SETUP

    _actionNew         = setupAction();
    _actionEdit        = setupAction();
    _actionDelete      = setupAction();
    _actionSearch      = setupAction();
    _actionFilter      = setupAction();
    _actionClearFilter = setupAction();

    ActionController::addSyncedActionAndButton(_actionNew,    ui->pbNew,    "projectData.item.new",    ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionEdit,   ui->pbEdit,   "projectData.item.edit",   ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDelete, ui->pbDelete, "projectData.item.delete", ActionController::AllComponents, ActionController::AllExceptShortcutComponent);

    ActionController::add(ui->pbFilter, "projectDataTable.filter.open", ActionController::AllExceptShortcutComponent);
    ActionController::add(_actionFilter, "projectDataTable.filter.open");

    ActionController::add(_actionClearFilter, "projectDataTable.filter.clear");
    ActionController::add(_actionSearch, "projectDataTable.search.focus");

    connect(_actionNew,    &QAction::triggered, this, &DockBusstops::onBusstopNew);
    connect(_actionEdit,   &QAction::triggered, this, &DockBusstops::onBusstopEdit);
    connect(_actionDelete, &QAction::triggered, this, &DockBusstops::onBusstopDelete);
    connect(_actionFilter, &QAction::triggered, this, [this](){ui->pbFilter->click();});

    ui->leSearch->setFocusAction(_actionSearch);

    // CONTEXT MENU

    ui->twBusstops->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twBusstops, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twBusstops->mapToGlobal(pos));
    });

    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDelete);
    globalMenu()->addSeparator();
    globalMenu()->addAction(_actionFilter);
    globalMenu()->addAction(_actionClearFilter);


    // VIEW/MODEL SETUP

    _proxyModel = new BusstopTableProxyModel(ui->pbFilter, this);
    _proxyModel->setFilterBanner(ui->filterBanner);
    _proxyModel->setClearFilterAction(_actionClearFilter);
    _proxyModel->setQuickSearchEdit(ui->leSearch);
    _proxyModel->setSourceModel(_model);

    ui->twBusstops->setModel(_proxyModel);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    ui->twBusstops->setItemDelegateForColumn(1, _delegate);
    ui->twBusstops->setItemDelegateForColumn(2, _delegate);
    ui->twBusstops->setItemDelegateForColumn(3, _delegate);

    connect(ui->twBusstops, &QTableView::doubleClicked, this, &DockBusstops::onBusstopEdit);

    connect(ui->twBusstops, &WdgProjectDataTableViewSignals::currentItemChanged, this, [this](){
        emit currentBusstopChanged(currentBusstop());
    });

    connect(ui->twBusstops, &WdgProjectDataTableViewSignals::selectedItemsChanged, this, [this](){
        emit selectedBusstopsChaned(selectedBusstops());
    });

    ui->twBusstops->addSelectionDependentAction(_actionEdit,   [](const int &n) { return n == 1; });
    ui->twBusstops->addSelectionDependentAction(_actionDelete, [](const int &n) { return n > 0; });

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twBusstops, ui->pbColumns, this);
}

DockBusstops::~DockBusstops() {
    delete ui;
}

Busstop *DockBusstops::currentBusstop() const {
    return ui->twBusstops->currentItem();
}

PDISet<Busstop> DockBusstops::selectedBusstops() const {
    return ui->twBusstops->selectedItems();
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
    Busstop *b = currentBusstop();
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
