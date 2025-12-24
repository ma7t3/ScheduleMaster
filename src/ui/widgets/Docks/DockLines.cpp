#include "DockLines.h"
#include "ui_DockLines.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"

#include "ApplicationInterface.h"
#include "Global/ActionController.h"
#include "src/projectdata/model/LineTableModel.h"

#include "src/ui/dialogs/DlgLineEditor.h"

#include "src/commands/CmdLines.h"

#include <QAction>
#include <QSortFilterProxyModel>
#include <QUndoStack>
#include <QMessageBox>

DockLines::DockLines(QWidget *parent) : DockAbstract(parent), ui(new Ui::DockLines),
    _model(new LineTableModel(this)),
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

    ActionController::add(ui->tbFilter, "projectDataTable.filter.open", ActionController::AllExceptShortcutComponent);
    ActionController::add(_actionFilter, "projectDataTable.filter.open");

    ActionController::add(_actionClearFilter, "projectDataTable.filter.clear");
    ActionController::add(_actionSearch, "projectDataTable.search.focus");

    connect(_actionNew,    &QAction::triggered, this, &DockLines::onLineNew);
    connect(_actionEdit,   &QAction::triggered, this, &DockLines::onLineEdit);
    connect(_actionDelete, &QAction::triggered, this, &DockLines::onLineDelete);
    connect(_actionFilter, &QAction::triggered, this, [this](){ui->tbFilter->click();});

    ui->leSearch->setFocusAction(_actionSearch);


    // CONTEXT MENU

    ui->twLines->setItemContextMenu(globalMenu());
    globalMenu()->addAction(_actionNew);
    globalMenu()->addAction(_actionEdit);
    globalMenu()->addAction(_actionDelete);
    globalMenu()->addSeparator();
    globalMenu()->addAction(_actionFilter);
    globalMenu()->addAction(_actionClearFilter);


    // VIEW/MODEL SETUP

    _proxyModel = new LineTableProxyModel(ui->tbFilter, this);
    _proxyModel->setFilterBanner(ui->filterBanner);
    _proxyModel->setClearFilterAction(_actionClearFilter);
    _proxyModel->setQuickSearchEdit(ui->leSearch);
    _proxyModel->setSourceModel(_model);

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

    _columnVisibilitySelector = new WdgTableColumnVisibilitySelector(ui->twLines, ui->tbColumns, this);
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
        if(bulletList.count() >= SM::SettingsServiceImpl::instance()->value("general.deleteDialog.maxListCount").toInt())
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
