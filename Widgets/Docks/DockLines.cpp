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

    // ACTION SETUP

    _actionNew = addAction("");
    _actionNew->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionEdit = addAction("");
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete = addAction("");
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionSearch = addAction("");
    _actionSearch->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ActionController::addSyncedActionAndButton(_actionNew,    ui->pbNew,    "projectData.item.new",    ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionEdit,   ui->pbEdit,   "projectData.item.edit",   ActionController::AllComponents, ActionController::AllExceptShortcutComponent);
    ActionController::addSyncedActionAndButton(_actionDelete, ui->pbDelete, "projectData.item.delete", ActionController::AllComponents, ActionController::AllExceptShortcutComponent);

    ActionController::add(_actionSearch, "projectDataTable.search.focus");

    connect(_actionNew,    &QAction::triggered, this, &DockLines::onLineNew);
    connect(_actionEdit,   &QAction::triggered, this, &DockLines::onLineEdit);
    connect(_actionDelete, &QAction::triggered, this, &DockLines::onLineDelete);
    connect(_actionSearch, &QAction::triggered, ui->leSearch, [this](){ui->leSearch->setFocus();});


    // CONTEXT MENU

    ui->twLines->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twLines, &QWidget::customContextMenuRequested, this, [this](QPoint pos) {
        globalMenu()->popup(ui->twLines->mapToGlobal(pos));
    });

    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDelete);


    // VIEW/MODEL SETUP

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortRole(Qt::DisplayRole);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->setFilterRole(Qt::DisplayRole);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->twLines->setModel(_proxyModel);
    ui->twLines->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twLines->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    connect(ui->twLines, &QTableView::doubleClicked, this, &DockLines::onLineEdit);

    connect(ui->twLines, &WdgProjectDataTableViewSignals::currentItemChanged, this, [this](){
        emit currentLineChanged(currentLine());
    });

    connect(ui->twLines, &WdgProjectDataTableViewSignals::selectedItemsChanged, this, [this](){
        emit selectedLinesChaned(selectedLines());
    });

    ui->twLines->addSelectionDependentAction(_actionEdit,   [](const int &n) { return n == 1; });
    ui->twLines->addSelectionDependentAction(_actionDelete, [](const int &n) { return n > 0; });

    connect(ui->leSearch, &QLineEdit::textChanged, _proxyModel, &QSortFilterProxyModel::setFilterWildcard);


    // COLUMN VISIBILITY SELECTOR

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twLines, this);

    ActionController::add(ui->tbColumns, "projectDataTable.showHideColumns");
    ui->tbColumns->setMenu(_columnVisibilitySelector->menu());

    ui->twLines->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twLines->horizontalHeader(),
            &QHeaderView::customContextMenuRequested,
            this,
            [this](const QPoint &pos) {
                _columnVisibilitySelector->menu()->popup(
                    ui->twLines->horizontalHeader()->mapToGlobal(pos));
            });
}

DockLines::~DockLines() {
    delete ui;
}

Line *DockLines::currentLine() const {
    return ui->twLines->currentItem();
}

PDISet<Line> DockLines::selectedLines() const {
    return ui->twLines->selectedItems();
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
