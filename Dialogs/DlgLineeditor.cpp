#include "Dialogs\DlgLineeditor.h"
#include "ui_DlgLineeditor.h"

#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>


DlgLineEditor::DlgLineEditor(QWidget *parent, Line l) :
    QDialog(parent),
    ui(new Ui::DlgLineEditor),
    _line(l)
{
    ui->setupUi(this);

    QObject::connect(ui->lwDirections, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(actionRenameDirection()));
    QObject::connect(ui->pbDirectionNew, SIGNAL(clicked()), this, SLOT(actionNewDirection()));
    QObject::connect(ui->pbDirectionRename, SIGNAL(clicked()), this, SLOT(actionRenameDirection()));
    QObject::connect(ui->pbDirectionDelete, SIGNAL(clicked()), this, SLOT(actionDeleteDirection()));

    QObject::connect(ui->pbDirectionUp, SIGNAL(clicked()), this, SLOT(actionDirectionUp()));
    QObject::connect(ui->pbDirectionDown, SIGNAL(clicked()), this, SLOT(actionDirectionDown()));

    QList<LineDirection *> directions;
    for(int i = 0; i < _line.directionCount(); i++) {
        LineDirection *ld = new LineDirection(*_line.directionAt(i));
        directions << ld;
    }

    _line.setDirections(directions);

    ui->leName->setText(l.name());
    ui->leDescription->setText(l.description());
    QString hex = l.color().name(QColor::HexRgb);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
    ui->lColorName->setText(hex);

    refreshDirections();
}

DlgLineEditor::~DlgLineEditor()
{
    delete ui;
}

Line DlgLineEditor::line() {
    _line.setName(ui->leName->text());
    _line.setDescription(ui->leDescription->text());
    _line.setColor(QColor(ui->lColorName->text()));
    return _line;
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

    LineDirection *ld = new LineDirection(global::getNewID(), newName);
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






















