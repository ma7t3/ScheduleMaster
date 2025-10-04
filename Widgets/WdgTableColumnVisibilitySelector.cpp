#include "WdgTableColumnVisibilitySelector.h"
#include "ui_WdgTableColumnVisibilitySelector.h"

#include "ProjectDataModels/UnorderedProjectDataRowModel.h"
#include "Global/ActionController.h"

#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>

WdgTableColumnVisibilitySelector::WdgTableColumnVisibilitySelector(QTableView *view,
                                                                   QToolButton *triggerButton,
                                                                   QWidget *parent) :
    QDialog(parent), ui(new Ui::WdgTableColumnVisibilitySelector), _view(view),
    _model(view->model()), _menu(new QMenu(this)), _restoreDefaultsAction(addAction("Restore Defaults")), _showInWindowAction(addAction("Show in Window")) {

    ui->setupUi(this);

    ActionController::add(triggerButton, "projectDataTable.showHideColumns");
    triggerButton->setMenu(menu());

    _view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_view->horizontalHeader(),
            &QHeaderView::customContextMenuRequested,
            this,
            [this](const QPoint &pos) {
                menu()->popup(_view->horizontalHeader()->mapToGlobal(pos));
            });

    ActionController::add(_restoreDefaultsAction, "projectDataTable.restoreDefaults");
    ActionController::add(_showInWindowAction, "projectDataTable.showInWindowAction");

    setWindowFlag(Qt::Tool, true);

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &WdgTableColumnVisibilitySelector::onButtonBoxClicked);
    connect(_model, &QAbstractItemModel::modelReset,   this, &WdgTableColumnVisibilitySelector::reset);
    connect(ui->lwColumns, &QListWidget::itemChanged,  this, &WdgTableColumnVisibilitySelector::onItemChanged);

    connect(_restoreDefaultsAction, &QAction::triggered, this, &WdgTableColumnVisibilitySelector::restoreDefaults);
    connect(_showInWindowAction, &QAction::triggered, this, &WdgTableColumnVisibilitySelector::show);

    // TODO: If needed: Handle adding/moving/removing columns

    reset();
}

WdgTableColumnVisibilitySelector::~WdgTableColumnVisibilitySelector() {
    delete ui;
}

QMenu *WdgTableColumnVisibilitySelector::menu() {
    return _menu;
}

void WdgTableColumnVisibilitySelector::reset() {
    ui->lwColumns->clear();
    _menu->clear();
    _actions.clear();

    const int count = _model->columnCount();
    for(int i = 0; i < count; i++) {
        const QString text = _model->headerData(i, Qt::Horizontal).toString();
        ui->lwColumns->addItem(text);

        QAction *action = addAction(text);
        _menu->addAction(action);
        _actions << action;

        connect(action, &QAction::toggled, this, [this, i](const bool &checked) {
            setColumnVisible(i, checked);
        });

        const bool visible = _model->headerData(i, Qt::Horizontal, ColumnVisibleByDefaultRole).toBool();
        setColumnVisible(i, visible);
    }

    _menu->addSeparator();
    _menu->addAction(_restoreDefaultsAction);
    _menu->addAction(_showInWindowAction);
}

void WdgTableColumnVisibilitySelector::setColumnVisible(const int &index, const bool &show) {
    const bool mandatory = columnIsMandatory(index);

    if(QTreeView *view = qobject_cast<QTreeView *>(_view))
        view->setColumnHidden(index, !show && !mandatory);
    else if(QTableView *view = qobject_cast<QTableView *>(_view))
        view->setColumnHidden(index, !show && !mandatory);

    _actions[index]->setCheckable(!mandatory);

    if(mandatory)
        return;

    ui->lwColumns->item(index)->setCheckState(show ? Qt::Checked : Qt::Unchecked);
    _actions[index]->setChecked(show);
}

bool WdgTableColumnVisibilitySelector::columnIsMandatory(const int &index) {
    return _model->headerData(index, Qt::Horizontal, ColumnMandatoryRole).toBool();
}

void WdgTableColumnVisibilitySelector::onButtonBoxClicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    switch(standardButton) {
    case QDialogButtonBox::RestoreDefaults:
        restoreDefaults();
        break;

    case QDialogButtonBox::Close:
        hide();
        break;

    default: break;
    }
}

void WdgTableColumnVisibilitySelector::onItemChanged(QListWidgetItem *item) {
    const int index = ui->lwColumns->row(item);
    const bool checked = item->checkState() == Qt::Checked;

    setColumnVisible(index, checked);
}

void WdgTableColumnVisibilitySelector::restoreDefaults() {
    const int count = _model->columnCount();
    for(int i = 0; i < count; i++) {
        const bool mandatory = _model->headerData(i, Qt::Horizontal, ColumnMandatoryRole).toBool();
        if(mandatory)
            continue;

        const bool visible = _model->headerData(i, Qt::Horizontal, ColumnVisibleByDefaultRole).toBool();
        ui->lwColumns->item(i)->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
    }
}
