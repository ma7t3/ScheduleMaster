#include "wdgbusstops.h"
#include "ui_wdgbusstops.h"

#include <QMessageBox>
#include <QUndoStack>
#include <QToolBar>

#include "Mainwindow.h"
#include "Commands/CmdBusstops.h"
#include "Dialogs/DlgBusstopeditor.h"


WdgBusstops::WdgBusstops(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgBusstops),
    projectData(((MainWindow *)parent)->projectData()),
    _model(new BusstopTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _currentBusstop(nullptr) {
    ui->setupUi(this);


    _model->setProjectData(projectData);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxyModel->setFilterKeyColumn(0);
    _proxyModel->setSortRole(0x0100);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->sort(0, Qt::AscendingOrder);

    ui->twBusstops->setSortingEnabled(true);
    ui->twBusstops->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    ui->twBusstops->setModel(_proxyModel);


    ui->twBusstops->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twBusstops->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(ui->twBusstops->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WdgBusstops::onSelectionChanged);

    _actionNew          = ui->twBusstops->addAction(QIcon(":/icons/Add.ico"),             tr("New"));
    _actionEdit         = ui->twBusstops->addAction(QIcon(":/icons/Edit.ico"),            tr("Edit"));
    _actionDelete       = ui->twBusstops->addAction(QIcon(":/icons/Delete.ico"),          tr("Delete"));
    _actionViewSchedule = ui->twBusstops->addAction(QIcon(":/icons/BusstopSchedule.ico"), tr("View Schedule"));

    _actionEdit->setDisabled(true);
    _actionDelete->setDisabled(true);

    _actionEdit->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Space), QKeySequence(Qt::Key_Return)});
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete->setShortcut(QKeySequence::Delete);
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionViewSchedule->setShortcuts({QKeySequence(tr("Alt+Enter")), QKeySequence(tr("Alt+Return"))});
    _actionViewSchedule->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->twBusstops->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_actionNew,          &QAction::triggered,               this,                &WdgBusstops::actionNew);
    connect(_actionEdit,         &QAction::triggered,               this,                &WdgBusstops::actionEdit);
    connect(_actionDelete,       &QAction::triggered,               this,                &WdgBusstops::actionDelete);

    connect(_actionNew,          &QAction::enabledChanged,          ui->pbBusstopNew,    &QPushButton::setEnabled);
    connect(_actionEdit,         &QAction::enabledChanged,          ui->pbBusstopEdit,   &QPushButton::setEnabled);
    connect(_actionDelete,       &QAction::enabledChanged,          ui->pbBusstopDelete, &QPushButton::setEnabled);

    connect(ui->pbBusstopNew,    &QPushButton::clicked,             this,                &WdgBusstops::actionNew);
    connect(ui->pbBusstopEdit,   &QPushButton::clicked,             this,                &WdgBusstops::actionEdit);
    connect(ui->twBusstops,      &QAbstractItemView::doubleClicked, this,                &WdgBusstops::actionEdit);
    connect(ui->pbBusstopDelete, &QPushButton::clicked,             this,                &WdgBusstops::actionDelete);
    connect(ui->leBusstopSearch, &QLineEdit::textChanged,           _proxyModel,         &QSortFilterProxyModel::setFilterFixedString);

    connect(_actionViewSchedule, &QAction::triggered,               this,                [this](){
        if(_currentBusstop && projectData->projectSettings()->dayTypeCount() > 0)
            emit busstopScheduleRequested(_currentBusstop, {}, projectData->projectSettings()->dayTypeAt(0));
    });
}

WdgBusstops::~WdgBusstops() {
    delete ui;
}

QList<QAction *> WdgBusstops::actions() {
    return ui->twBusstops->actions();
}

void WdgBusstops::refreshUI() {
    const int selectionCount = ui->twBusstops->selectionModel()->selectedRows().count();

    _actionEdit->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);
}

Busstop *WdgBusstops::currentBusstop() {
    return _currentBusstop;
}

void WdgBusstops::actionNew() {
    Busstop *b = projectData->newBusstop();

    busstopEditor dlg(this, *b, true);
    if(dlg.exec() != QDialog::Accepted) {
        delete b;
        return;
    }

    *b = dlg.busstop();

    projectData->undoStack()->push(new CmdBusstopNew(projectData, b));
    emit refreshRequested();
}

void WdgBusstops::actionEdit() {
    if(!_currentBusstop)
        return;

    busstopEditor dlg(this, *_currentBusstop, false);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Busstop newB = dlg.busstop();

    projectData->undoStack()->push(new CmdBusstopEdit(_currentBusstop, newB));
    emit refreshRequested();
}

void WdgBusstops::actionDelete() {
    QModelIndexList selection = ui->twBusstops->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Busstop *> busstops;
    const int maxShowCount = 15;
    bool hasMore = selection.count() > maxShowCount;
    for(int i = 0; i < selection.count(); i++) {
        Busstop *b = _model->itemAt(_proxyModel->mapToSource(selection[i]).row());
        busstops << b;
        if(i < maxShowCount)
            showList += QString("<li>%1</li>").arg(b->name());
    }
    if(hasMore)
        showList += "<li style=\"list-style-type: none\"><i>" + tr("%n more", "", selection.count() - maxShowCount) + "</i></li>";
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete %n busstop(s)", "", busstops.count()), tr("<p><b>Do you really want to delete these %n busstop(s)?</b></p>%1", "", busstops.count()).arg(showList), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdBusstopsDelete(projectData, busstops));
    emit refreshRequested();
}

void WdgBusstops::onSelectionChanged() {
    const QModelIndex current = ui->twBusstops->selectionModel()->currentIndex();
    const int selectionCount = ui->twBusstops->selectionModel()->selectedRows().count();

    if(selectionCount != 1)
        _currentBusstop = nullptr;
    else
        _currentBusstop = _model->itemAt(_proxyModel->mapToSource(current).row());
    refreshUI();
    emit currentBusstopChanged(_currentBusstop);
}
