#include "DlgBusstopEditor.h"
#include "ui_DlgBusstopEditor.h"

#include "Global/ActionController.h"
#include "Global/ActionController.h"

#include <QInputDialog>

DlgBusstopEditor::DlgBusstopEditor(Busstop *busstop, QWidget *parent) :
    QDialog(parent), ui(new Ui::DlgBusstopEditor), _platformMenu(new QMenu(this)),
    _busstop(busstop),
    _model(new BusstopPlatformTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)) {
    ui->setupUi(this);

    setWindowTitle(busstop->isClone() ? tr("Edit Busstop") : tr("Create Busstop"));

    ui->gbPlatformEditor->setVisible(false);

    _actionNewPlatform        = ui->twPlatforms->addAction("");
    _actionDeletePlatform     = ui->twPlatforms->addAction("");
    _actionSetDefaultPlatform = ui->twPlatforms->addAction("");

    connect(_actionNewPlatform,        &QAction::triggered, this, &DlgBusstopEditor::onPlatformNew);
    connect(_actionDeletePlatform,     &QAction::triggered, this, &DlgBusstopEditor::onPlatformDelete);
    connect(_actionSetDefaultPlatform, &QAction::triggered, this, &DlgBusstopEditor::onPlatformSetDefault);

    ActionController::addSyncedActionAndButton(_actionNewPlatform,        ui->pbPlatformNew, "projectData.item.new",                 ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDeletePlatform,     ui->pbPlatformDelete, "projectData.item.delete",           ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionSetDefaultPlatform, ui->pbPlatformSetDefault, "projectData.item.setAsDefault", ActionController::AllComponents, ActionController::AllExceptShortcutComponent);

    _platformMenu->addActions({_actionNewPlatform, _actionDeletePlatform});
    _platformMenu->addSeparator();
    _platformMenu->addAction(_actionSetDefaultPlatform);

    _model->setBusstop(_busstop);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortRole(Qt::DisplayRole);
    _proxyModel->setSortLocaleAware(true);

    ui->twPlatforms->setModel(_proxyModel);
    ui->twPlatforms->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twPlatforms->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->twPlatforms->header()->setSortIndicator(0, Qt::AscendingOrder);

    ui->twPlatforms->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twPlatforms, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        _platformMenu->popup(ui->twPlatforms->mapToGlobal(pos));
    });

    connect(ui->twPlatforms->selectionModel(), &QItemSelectionModel::currentChanged, this, &DlgBusstopEditor::onCurrentPlatformChanged);

    ui->leName->setText(_busstop->name());

    ui->wdgBusstopFlagEditor->setFlags(_busstop->flags());

    ui->pteComment->setPlainText(_busstop->comment());
}

DlgBusstopEditor::~DlgBusstopEditor() {
    delete ui;
}

Busstop *DlgBusstopEditor::busstop() const {
    return _busstop;
}

void DlgBusstopEditor::onPlatformNew() {
    BusstopPlatform *newPlatform = _busstop->createPlatform();
    _busstop->addPlatform(newPlatform);
    QModelIndex current = _proxyModel->mapFromSource(_model->index(_model->indexOfItem(newPlatform), 0));
    ui->twPlatforms->setCurrentIndex(current);
    ui->wdgPlatformEditor->setFocus();
    ui->wdgPlatformEditor->focusName();
}

void DlgBusstopEditor::onPlatformDelete() {
    QModelIndex index = ui->twPlatforms->currentIndex();
    BusstopPlatform *platform = _model->itemAt(_proxyModel->mapToSource(index).row());
    _busstop->removePlatform(platform);
}

void DlgBusstopEditor::onPlatformSetDefault() {
    QModelIndex index = ui->twPlatforms->currentIndex();
    BusstopPlatform *platform = _model->itemAt(_proxyModel->mapToSource(index).row());
    _busstop->setDefaultPlatform(platform);
}

void DlgBusstopEditor::onCurrentPlatformChanged() {
    const int row = _proxyModel->mapToSource(ui->twPlatforms->currentIndex()).row();
    BusstopPlatform *platform = _model->itemAt(row);
    if(!platform) {
        ui->gbPlatformEditor->setVisible(false);
        return;
    }

    ui->gbPlatformEditor->setVisible(true);
    ui->wdgPlatformEditor->setPlatform(platform);
}

void DlgBusstopEditor::accept() {
    _busstop->setName(ui->leName->text());
    _busstop->setFlags(ui->wdgBusstopFlagEditor->flags());
    _busstop->setComment(ui->pteComment->toPlainText());
    QDialog::accept();
}
