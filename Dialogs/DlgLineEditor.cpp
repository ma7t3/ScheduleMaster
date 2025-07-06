#include "DlgLineEditor.h"
#include "ui_DlgLineEditor.h"

#include "Global/ActionController.h"

#include <QMessageBox>
#include <QInputDialog>

DlgLineEditor::DlgLineEditor(Line *line, QWidget *parent) :
    QDialog(parent), ui(new Ui::DlgLineEditor), _directionMenu(new QMenu(this)), _line(line), _model(new LineDirectionTableModel(this)) {
    ui->setupUi(this);

    setWindowTitle(line->isClone() ? tr("Edit Line") : tr("Create Line"));

    ActionController::add(ui->pbDirectionsNew,      "projectData.item.new",      ActionController::IconComponent);
    ActionController::add(ui->pbDirectionsEdit,     "projectData.item.edit",     ActionController::IconComponent);
    ActionController::add(ui->pbDirectionsDelete,   "projectData.item.delete",   ActionController::IconComponent);
    ActionController::add(ui->pbDirectionsMoveUp,   "projectData.item.moveUp",   ActionController::IconComponent);
    ActionController::add(ui->pbDirectionsMoveDown, "projectData.item.moveDown", ActionController::IconComponent);

    _directionNew      = ui->twDirections->addAction("");
    _directionEdit     = ui->twDirections->addAction("");
    _directionDelete   = ui->twDirections->addAction("");
    _directionMoveUp   = ui->twDirections->addAction("");
    _directionMoveDown = ui->twDirections->addAction("");

    ActionController::add(_directionNew,      "projectData.item.new");
    ActionController::add(_directionEdit,     "projectData.item.edit");
    ActionController::add(_directionDelete,   "projectData.item.delete");
    ActionController::add(_directionMoveUp,   "projectData.item.moveUp");
    ActionController::add(_directionMoveDown, "projectData.item.moveDown");

    _directionNew->setShortcutContext(Qt::WidgetShortcut);
    _directionEdit->setShortcutContext(Qt::WidgetShortcut);
    _directionDelete->setShortcutContext(Qt::WidgetShortcut);
    _directionMoveUp->setShortcutContext(Qt::WidgetShortcut);
    _directionMoveDown->setShortcutContext(Qt::WidgetShortcut);

    connect(_directionNew,      &QAction::enabledChanged, ui->pbDirectionsNew,      &QPushButton::setEnabled);
    connect(_directionEdit,     &QAction::enabledChanged, ui->pbDirectionsEdit,     &QPushButton::setEnabled);
    connect(_directionDelete,   &QAction::enabledChanged, ui->pbDirectionsDelete,   &QPushButton::setEnabled);
    connect(_directionMoveUp,   &QAction::enabledChanged, ui->pbDirectionsMoveUp,   &QPushButton::setEnabled);
    connect(_directionMoveDown, &QAction::enabledChanged, ui->pbDirectionsMoveDown, &QPushButton::setEnabled);

    connect(_directionNew,      &QAction::triggered, this, &DlgLineEditor::onDirectionNew);
    connect(_directionEdit,     &QAction::triggered, this, &DlgLineEditor::onDirectionEdit);
    connect(_directionDelete,   &QAction::triggered, this, &DlgLineEditor::onDirectionDelete);
    connect(_directionMoveUp,   &QAction::triggered, this, &DlgLineEditor::onDirectionMoveUp);
    connect(_directionMoveDown, &QAction::triggered, this, &DlgLineEditor::onDirectionMoveDown);

    connect(ui->pbDirectionsNew,      &QPushButton::clicked, _directionNew,      &QAction::trigger);
    connect(ui->pbDirectionsEdit,     &QPushButton::clicked, _directionEdit,     &QAction::trigger);
    connect(ui->pbDirectionsDelete,   &QPushButton::clicked, _directionDelete,   &QAction::trigger);
    connect(ui->pbDirectionsMoveUp,   &QPushButton::clicked, _directionMoveUp,   &QAction::trigger);
    connect(ui->pbDirectionsMoveDown, &QPushButton::clicked, _directionMoveDown, &QAction::trigger);

    connect(ui->twDirections, &QListView::doubleClicked, this, &DlgLineEditor::onDirectionEdit);

    _directionMenu->addActions({_directionNew, _directionEdit, _directionDelete});
    _directionMenu->addSeparator();
    _directionMenu->addActions({_directionMoveUp, _directionMoveDown});

    ui->twDirections->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->twDirections, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        _directionMenu->popup(ui->twDirections->mapToGlobal(pos));
    });

    _model->setLine(_line);
    ui->twDirections->setModel(_model);

    connect(ui->twDirections->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgLineEditor::onCurrentDirectionChanged);

    ui->leName->setText(line->name());
    ui->leDescription->setText(line->description());
    ui->wdgColorSelector->setColor(line->color());
    ui->cbLineCardShape->setCurrentIndex(line->cardShape());

    ui->pteComment->setPlainText(line->comment());

    onCurrentDirectionChanged();
}

DlgLineEditor::~DlgLineEditor() {
    delete ui;
}

Line *DlgLineEditor::line() {
    return _line;
}

void DlgLineEditor::onDirectionNew() {
    bool ok;
    const QString description = QInputDialog::getText(this, tr("Create Direction"), tr("Description:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    LineDirection *ld = _line->createDirection();
    ld->setDescription(description);
    _line->appendDirection(ld);
    ui->twDirections->setCurrentIndex(_model->index(_model->rowCount() - 1, 0));
}

void DlgLineEditor::onDirectionEdit() {
    QModelIndex index = ui->twDirections->currentIndex();
    LineDirection *ld = _model->itemAt(index.row());

    if(!ld)
        return;

    bool ok;
    const QString description = QInputDialog::getText(this, tr("Edit Direction"), tr("Description:"), QLineEdit::Normal, ld->description(), &ok);
    if(!ok)
        return;

    ld->setDescription(description);
}

void DlgLineEditor::onDirectionDelete() {
    QModelIndex index = ui->twDirections->currentIndex();
    LineDirection *ld = _model->itemAt(index.row());
    if(!ld)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete direction"), tr("<p>Do you really want to delete this direction?</p><p><b>%1</b></p>").arg(ld->description()), QMessageBox::Yes | QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _line->removeDirection(ld);
}

void DlgLineEditor::onDirectionMoveUp() {
    QModelIndex index = ui->twDirections->currentIndex();
    _line->moveDirection(index.row(), index.row() - 1);
    onCurrentDirectionChanged();
}

void DlgLineEditor::onDirectionMoveDown() {
    QModelIndex index = ui->twDirections->currentIndex();
    _line->moveDirection(index.row(), index.row() + 1);
    onCurrentDirectionChanged();
}

void DlgLineEditor::onCurrentDirectionChanged() {
    const QModelIndex current = ui->twDirections->currentIndex();
    _directionEdit->setEnabled(current.isValid());
    _directionDelete->setEnabled(current.isValid());

    const int row = current.row();
    _directionMoveUp->setEnabled(row > 0);
    _directionMoveDown->setEnabled(row < _model->rowCount() - 1 && row > -1);
}

void DlgLineEditor::accept() {
    _line->setName(ui->leName->text());
    _line->setDescription(ui->leDescription->text());
    _line->setColor(ui->wdgColorSelector->color());
    _line->setCardShape(static_cast<LineCardShape>(ui->cbLineCardShape->currentIndex()));

    _line->setComment(ui->pteComment->toPlainText());
    QDialog::accept();
}
