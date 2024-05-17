#include "Dialogs\DlgLineeditor.h"
#include "ui_DlgLineeditor.h"

#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>


DlgLineEditor::DlgLineEditor(QWidget *parent, Line *line, bool createMode) :
    QDialog(parent),
    ui(new Ui::DlgLineEditor),
    _line(*line),
    _linePtr(line) {
    ui->setupUi(this);

    setCreateMode(createMode);
    setLine(*line);

    connect(ui->pbDirectionNew,    &QPushButton::clicked,           this, &DlgLineEditor::actionNewDirection);
    connect(ui->pbDirectionRename, &QPushButton::clicked,           this, &DlgLineEditor::actionRenameDirection);
    connect(ui->lwDirections,      &QListWidget::itemDoubleClicked, this, &DlgLineEditor::actionRenameDirection);
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

void DlgLineEditor::setLine(const Line &l) {
    _line = l;
    _line.setDirections(_linePtr->cloneDirections());

    ui->leName->setText(_line.name());
    ui->leDescription->setText(_line.description());
    QString hex = _line.color().name(QColor::HexRgb);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
    ui->lColorName->setText(hex);

    refreshDirections();
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
    ui->lwDirections->addItem(newName);
}

void DlgLineEditor::actionRenameDirection() {
    if(!ui->lwDirections->currentItem())
        return;

    LineDirection *ld = _line.directionAt(ui->lwDirections->currentRow());

    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename direction"), tr("Enter a new direction description:"), QLineEdit::Normal, ld->description(), &ok);
    if(!ok || newName.isEmpty())
        return;

    ld->setDescription(newName);
    ui->lwDirections->currentItem()->setText(newName);
}

void DlgLineEditor::actionDeleteDirection() {
    if(!ui->lwDirections->currentItem())
        return;

    LineDirection *ld = _line.directionAt(ui->lwDirections->currentRow());

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete Direction"), tr("<p><b>Do you really want to delete this direction?</b></p><ul><li>%1</li></ul>").arg(ld->description()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    _line.removeDirection(ld);
    ui->lwDirections->takeItem(ui->lwDirections->currentRow());
}

void DlgLineEditor::actionDirectionUp() {
    if(!ui->lwDirections->currentItem())
        return;

    int row = ui->lwDirections->currentRow();
    if(row == 0)
        return;

    QList<LineDirection *> directions = _line.directions();
    LineDirection *ld = directions[row];
    directions.remove(row);
    directions.insert(row - 1, ld);
    _line.setDirections(directions);
    ui->lwDirections->setCurrentRow(row - 1);

    refreshDirections();
}

void DlgLineEditor::actionDirectionDown() {
    if(!ui->lwDirections->currentItem())
        return;

    int row = ui->lwDirections->currentRow();
    if(row == _line.directionCount() - 1)
        return;

    QList<LineDirection *> directions = _line.directions();
    LineDirection *ld = directions[row];
    directions.remove(row);
    directions.insert(row + 1, ld);
    _line.setDirections(directions);

    ui->lwDirections->setCurrentRow(row + 1);

    refreshDirections();
}

void DlgLineEditor::refreshDirections() {
    int row = ui->lwDirections->currentRow();
    ui->lwDirections->clear();
    for(int i = 0; i < _line.directionCount(); i++) {
        ui->lwDirections->addItem(_line.directionAt(i)->description());
    }
    ui->lwDirections->setCurrentRow(row);
}






















