#include "Dialogs\DlgLineeditor.h"
#include "ui_DlgLineeditor.h"

#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>


DlgLineEditor::DlgLineEditor(QWidget *parent, Line *line, bool createMode) :
    QDialog(parent),
    ui(new Ui::DlgLineEditor),
    _line(*line),
    _linePtr(line),
    _directionsModel(new LineDirectionListModel(this)) {
    ui->setupUi(this);

    setCreateMode(createMode);
    loadLine();
    ui->lwDirections->setModel(_directionsModel);

    connect(_directionsModel, &QAbstractItemModel::rowsInserted, this, &DlgLineEditor::onDirectionInserted);

    connect(ui->pbDirectionNew,    &QPushButton::clicked,           this, &DlgLineEditor::actionNewDirection);
    connect(ui->pbDirectionRename, &QPushButton::clicked,           this, &DlgLineEditor::actionRenameDirection);
    connect(ui->lwDirections,      &QListWidget::doubleClicked,     this, &DlgLineEditor::actionRenameDirection);
    connect(ui->pbDirectionDelete, &QPushButton::clicked,           this, &DlgLineEditor::actionDeleteDirection);

    connect(ui->pbDirectionUp,     &QPushButton::clicked,           this, &DlgLineEditor::actionDirectionUp);
    connect(ui->pbDirectionDown,   &QPushButton::clicked,           this, &DlgLineEditor::actionDirectionDown);

    ui->leName->setFocus();
}

DlgLineEditor::~DlgLineEditor() {
    delete ui;
}

void DlgLineEditor::setCreateMode(const bool &newCreateMode) {
    if(newCreateMode)
        setWindowTitle(tr("Create line"));
}

Line DlgLineEditor::line() const {
    Line l = _line;
    l.setName(ui->leName->text());
    l.setDescription(ui->leDescription->text());
    l.setColor(QColor(ui->lColorName->text()));
    return l;
}

void DlgLineEditor::loadLine() {
    _line.setDirections(_linePtr->cloneDirections());

    ui->leName->setText(_line.name());
    ui->leDescription->setText(_line.description());
    QString hex = _line.color().name(QColor::HexRgb);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
    ui->lColorName->setText(hex);

    _directionsModel->setLine(&_line);
}

void DlgLineEditor::on_pbColor_clicked() {
    QColor oldColor(ui->lColorName->text());
    QColor newColor(QColorDialog::getColor(oldColor, this));

    if(!newColor.isValid())
        return;

    QString hex = newColor.name(QColor::HexRgb);
    ui->lColorName->setText(hex);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
}

void DlgLineEditor::actionNewDirection() {
    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename direction"), tr("Enter a new direction description:"), QLineEdit::Normal, "", &ok);
    if(!ok || newName.isEmpty())
        return;

    LineDirection *ld = _linePtr->newDirection();
    ld->setDescription(newName);
    _line.addDirection(ld);
}

void DlgLineEditor::actionRenameDirection() {
    if(!ui->lwDirections->currentIndex().isValid())
        return;

    LineDirection *ld = _directionsModel->itemAt(ui->lwDirections->currentIndex().row());

    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename direction"), tr("Enter a new direction description:"), QLineEdit::Normal, ld->description(), &ok);
    if(!ok || newName.isEmpty())
        return;

    ld->setDescription(newName);
}

void DlgLineEditor::actionDeleteDirection() {
    if(!ui->lwDirections->currentIndex().isValid())
        return;

    LineDirection *ld = _directionsModel->itemAt(ui->lwDirections->currentIndex().row());

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete Direction"), tr("<p><b>Do you really want to delete this direction?</b></p><ul><li>%1</li></ul>").arg(ld->description()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _line.removeDirection(ld);
}

void DlgLineEditor::actionDirectionUp() {
    if(!ui->lwDirections->currentIndex().isValid())
        return;

    int row = ui->lwDirections->currentIndex().row();
    if(row == 0)
        return;

    LineDirection *ld = _directionsModel->itemAt(ui->lwDirections->currentIndex().row());

    _line.removeDirection(ld);
    qApp->processEvents();
    _line.insertDirection(row - 1, ld);
}

void DlgLineEditor::actionDirectionDown() {
    if(!ui->lwDirections->currentIndex().isValid())
        return;

    int row = ui->lwDirections->currentIndex().row();
    if(row == _line.directionCount() - 1)
        return;

    LineDirection *ld = _directionsModel->itemAt(ui->lwDirections->currentIndex().row());

    _line.removeDirection(ld);
    qApp->processEvents();
    _line.insertDirection(row + 1, ld);
}

void DlgLineEditor::onDirectionInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);Q_UNUSED(last);
    ui->lwDirections->setCurrentIndex(_directionsModel->index(first, 0));
    ui->lwDirections->selectionModel()->select(_directionsModel->index(first, 0), QItemSelectionModel::ClearAndSelect);
    ui->lwDirections->setFocus();
}
