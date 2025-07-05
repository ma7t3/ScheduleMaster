#include "DockLines.h"
#include "ui_DockLines.h"

#include "ApplicationInterface.h"
#include "Global/ActionController.h"
#include "Global/SettingsManager.h"
#include "ProjectDataModels/LineTableModel.h"

#include "Dialogs/DlgLineEditor.h"

#include "Commands/CmdLines.h"

#include <QAction>
#include <QSortFilterProxyModel>
#include <QUndoStack>
#include <QMessageBox>

DockLines::DockLines(QWidget *parent) : DockAbstract(parent), ui(new Ui::DockLines),
    _model(new LineTableModel(this)),
    _proxyModel(new LineTableProxyModel(this)),
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

    ActionController::add(ui->pbNew,    "projectData.item.new",    ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbEdit,   "projectData.item.edit",   ActionController::AllExceptShortcutComponent);
    ActionController::add(ui->pbDelete, "projectData.item.delete", ActionController::AllExceptShortcutComponent);

    connect(_newAction,    &QAction::enabledChanged, ui->pbNew,    &QPushButton::setEnabled);
    connect(_editAction,   &QAction::enabledChanged, ui->pbEdit,   &QPushButton::setEnabled);
    connect(_deleteAction, &QAction::enabledChanged, ui->pbDelete, &QPushButton::setEnabled);

    connect(ui->pbNew,    &QPushButton::clicked, _newAction,    &QAction::trigger);
    connect(ui->pbEdit,   &QPushButton::clicked, _editAction,   &QAction::trigger);
    connect(ui->pbDelete, &QPushButton::clicked, _deleteAction, &QAction::trigger);

    connect(_newAction, &QAction::triggered,    this, &DockLines::onLineNew);
    connect(_editAction, &QAction::triggered,   this, &DockLines::onLineEdit);
    connect(_deleteAction, &QAction::triggered, this, &DockLines::onLineDelete);

    ui->twLines->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->twLines, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twLines->mapToGlobal(pos));
    });

    globalMenu()->addAction(_newAction);
    globalMenu()->addAction(_editAction);
    globalMenu()->addAction(_deleteAction);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortRole(Qt::DisplayRole);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->setFilterRole(Qt::DisplayRole);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->twLines->setModel(_proxyModel);
    ui->twLines->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twLines->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    connect(ui->twLines, &QTableView::doubleClicked, this, &DockLines::onLineEdit);
    connect(ui->twLines->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DockLines::onSelectionChanged);

    connect(_model, &UnorderedProjectDataRowModelSignals::multipleRowsInserted, this, &DockLines::onRowsAdded);
    connect(_model, &QAbstractItemModel::modelReset, this, &DockLines::onSelectionChanged);

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(_searchAction, &QAction::triggered, ui->leSearch, [this](){ui->leSearch->setFocus();});

    onSelectionChanged();
}

DockLines::~DockLines() {
    delete ui;
}

Line *DockLines::currentLine() const {
    return _currentLine;
}

PDISet<Line> DockLines::selectedLines() const {
    const QModelIndexList list = ui->twLines->selectionModel()->selectedRows();
    PDISet<Line> lines;
    for(const QModelIndex &index : list) {
        Line *l = _model->itemAt(_proxyModel->mapToSource(index).row());
        lines.add(l);
    }
    return lines;
}

void DockLines::onLineNew() {
    Line *l = _projectData->createLine();
    DlgLineEditor dlg(l);
    if(dlg.exec() != QDialog::Accepted) {
        l->deleteLater();
        return;
    }

    _projectData->undoStack()->push(new CmdLineNew(_projectData, l));
}

void DockLines::onLineEdit() {
    Line *l = _model->itemAt(_proxyModel->mapToSource(ui->twLines->currentIndex()).row());
    if(!l)
        return;

    DlgLineEditor dlg(l->clone());
    if(dlg.exec() != QDialog::Accepted)
        return;

    _projectData->undoStack()->push(new CmdLineEdit(l, dlg.line()));
}

void DockLines::onLineDelete() {
    const PDISet<Line> lines = selectedLines();
    QStringList bulletList;
    for(Line *l : lines) {
        if(bulletList.count() >= SettingsManager::value("general.deleteDialog.maxListCount").toInt())
            break;
        bulletList << QString("<li>%1</li>").arg(l->name());
    }

    bulletList.sort();

    QMessageBox::StandardButton msg = QMessageBox::warning(
        this,
        tr("Delete line(s)"),
        tr("<p><b>Do you really want to delete these %n line(s)?</b></p><ul>%1</ul>%2",
           "",
           lines.count())
            .arg(bulletList.join(""))
            .arg(bulletList.count() < lines.count()
                     ? tr("<i>%n more</i>", "", lines.count() - bulletList.count())
                     : ""),
        QMessageBox::Yes | QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _projectData->undoStack()->push(new CmdLinesRemove(_projectData, lines));
}

void DockLines::onSelectionChanged() {
    const QModelIndex current = ui->twLines->currentIndex();
    const QModelIndexList list = ui->twLines->selectionModel()->selectedRows();
    const int count = list.count();

    _editAction->setEnabled(count == 1);
    _deleteAction->setEnabled(count > 0);

    if(current.isValid() && count == 1)
        _currentLine = _model->itemAt(_proxyModel->mapToSource(current).row());
    else
        _currentLine = nullptr;

    emit currentLineChanged(_currentLine);
    emit selectedLinesChaned(selectedLines());
}

void DockLines::onRowsAdded(const QList<QPersistentModelIndex> &indexes) {
    // This is a quick and dirty fix to prevent selecting one random item when loading a file.....
    if(indexes.count() == _projectData->lines().count())
        return;

    ui->twLines->clearSelection();
    for(const QPersistentModelIndex &index : indexes)
        ui->twLines->selectRow(_proxyModel->mapFromSource(index).row());
}
