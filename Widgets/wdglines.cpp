#include "wdglines.h"
#include "ui_wdglines.h"

#include "Mainwindow.h"
#include "App/global.h"
#include "Commands/CmdLines.h"
#include "Dialogs/DlgLineeditor.h"
#include "ProjectData/projectdata.h"

#include <QUndoStack>
#include <QMessageBox>

WdgLines::WdgLines(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgLines),
    projectData(((MainWindow *)parent)->projectData()),
    _model(new LineTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _currentLine(nullptr) {
    ui->setupUi(this);

    _model->setProjectData(projectData);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->sort(0, Qt::AscendingOrder);

    ui->twLines->setSortingEnabled(true);
    ui->twLines->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    ui->twLines->setModel(_proxyModel);


    ui->twLines->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twLines->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(ui->twLines->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WdgLines::onSelectionChanged);
    connect(_model, &QAbstractItemModel::rowsInserted, this, &WdgLines::onRowsInserted);

    _actionNew    = ui->twLines->addAction(QIcon(":/icons/Add.ico"),    tr("New"));
    _actionEdit   = ui->twLines->addAction(QIcon(":/icons/Edit.ico"),   tr("Edit"));
    _actionDelete = ui->twLines->addAction(QIcon(":/icons/Delete.ico"), tr("Delete"));

    _actionEdit->setDisabled(true);
    _actionDelete->setDisabled(true);

    _actionEdit->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Return), QKeySequence(Qt::Key_Space)});
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete->setShortcut(QKeySequence::Delete);
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->twLines->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_actionNew,       &QAction::triggered,              this,             &WdgLines::actionNew);
    connect(_actionEdit,      &QAction::triggered,              this,             &WdgLines::actionEdit);
    connect(_actionDelete,    &QAction::triggered,              this,             &WdgLines::actionDelete);

    connect(_actionNew,       &QAction::enabledChanged,         ui->pbLineNew,    &QPushButton::setEnabled);
    connect(_actionEdit,      &QAction::enabledChanged,         ui->pbLineEdit,   &QPushButton::setEnabled);
    connect(_actionDelete,    &QAction::enabledChanged,         ui->pbLineDelete, &QPushButton::setEnabled);

    connect(ui->pbLineNew,    &QPushButton::clicked,             this,             &WdgLines::actionNew);
    connect(ui->pbLineEdit,   &QPushButton::clicked,             this,             &WdgLines::actionEdit);
    connect(ui->twLines,      &QAbstractItemView::doubleClicked, this,             &WdgLines::actionEdit);
    connect(ui->pbLineDelete, &QPushButton::clicked,             this,             &WdgLines::actionDelete);
}

WdgLines::~WdgLines() {
    delete ui;
}

QList<QAction *> WdgLines::actions() {
    return ui->twLines->actions();
}


void WdgLines::refreshUI() {
    int selectionCount = ui->twLines->selectionModel()->selectedRows(0).count();

    _actionEdit->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);
}

Line * WdgLines::currentLine() {
    return _currentLine;
}

void WdgLines::actionNew() {
    Line *l = projectData->newLine();

    DlgLineEditor dlg(this, l, true);
    if(dlg.exec() != QDialog::Accepted)
        return;

    *l = dlg.line();

    projectData->undoStack()->push(new CmdLineNew(projectData, l));
    _currentLine = l;
    emit currentLineChanged(l);
    emit refreshRequested();
}

void WdgLines::actionEdit() {
    Line *l = currentLine();
    if(!l)
        return;

    DlgLineEditor dlg(this, l, false);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Line newL = dlg.line();

    projectData->undoStack()->push(new CmdLineEdit(l, newL));
    emit refreshRequested();
}

void WdgLines::actionDelete() {
    QModelIndexList selection = ui->twLines->selectionModel()->selectedRows(0);

    QString showList ="<table style=\"border-collapse: collapse;\">";
    QList<Line *> lines;
    const int maxShowCount = 15;
    bool hasMore = selection.count() > maxShowCount;
    for(int i = 0; i < selection.count(); i++) {
        Line *l = _model->itemAt(_proxyModel->mapToSource(selection[i]).row());
        lines << l;
        if(i < maxShowCount) {
            QColor color = l->color();
            QColor contrastColor = global::getContrastColor(color);
            showList += QString("<tr><td style=\"color: %1; background-color: %2; padding: 5px\">%3</td><td style=\"padding: 5px;\">%4</td>").arg(contrastColor.name(QColor::HexRgb), color.name(QColor::HexRgb), l->name(), l->description());
        }
    }

    if(hasMore)
        showList += "<tr><td></td><td style=\"padding: 5px;\"><i>" + tr("%n more", "", selection.count() - maxShowCount) + "</i></td></tr>";

    showList += "</table>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete line(s)"), tr("<p><b>Do you really want to delete these %n line(s)?</b></p><p></p>", "", lines.count()) + showList, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdLinesDelete(projectData, lines));
    emit refreshRequested();
}

void WdgLines::onSelectionChanged() {
    const QModelIndex current = ui->twLines->selectionModel()->currentIndex();
    const int selectionCount  = ui->twLines->selectionModel()->selectedRows().count();

    if(selectionCount != 1)
        _currentLine = nullptr;
    else
        _currentLine = _model->itemAt(_proxyModel->mapToSource(current).row());
    refreshUI();
    emit currentLineChanged(_currentLine);
}

void WdgLines::onRowsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);
    ui->twLines->setCurrentIndex(_proxyModel->mapFromSource(_model->index(first, 0)));
    ui->twLines->selectionModel()->select(QItemSelection(_proxyModel->mapFromSource(_model->index(first, 0)), _proxyModel->mapFromSource(_model->index(last, 1))), QItemSelectionModel::ClearAndSelect);
    ui->twLines->setFocus();
}
