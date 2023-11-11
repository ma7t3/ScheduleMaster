#include "Dialogs\DlgLineeditor.h"
#include "ui_DlgLineeditor.h"

#include <QColorDialog>
#include <QInputDialog>

lineEditor::lineEditor(QWidget *parent, Line l) :
    QDialog(parent),
    ui(new Ui::lineEditor),
    m_line(l)
{
    ui->setupUi(this);

    QObject::connect(ui->lwDirections, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(actionRenameDirection()));
    QObject::connect(ui->pbDirectionRename, SIGNAL(clicked()), this, SLOT(actionRenameDirection()));

    ui->leName->setText(l.name());
    ui->leDescription->setText(l.getDescription());
    QString hex = l.getColor().name(QColor::HexRgb);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
    ui->lColorName->setText(hex);

    refreshDirections();
}

lineEditor::~lineEditor()
{
    delete ui;
}

Line lineEditor::line() {
    m_line.setName(ui->leName->text());
    m_line.setDescription(ui->leDescription->text());
    m_line.setColor(QColor(ui->lColorName->text()));

    return m_line;
}

void lineEditor::on_pbColor_clicked() {
    QColor oldColor(ui->lColorName->text());
    QColor newColor(QColorDialog::getColor(oldColor, this));

    if(!newColor.isValid())
        return;

    QString hex = newColor.name(QColor::HexRgb);
    ui->lColorName->setText(hex);
    ui->lColor->setStyleSheet("background-color: " + hex + ";");
}

void lineEditor::actionRenameDirection() {
    if(!ui->lwDirections->currentItem())
        return;

    LineDirection *ld = directionTableReference[ui->lwDirections->currentRow()];

    bool ok = false;
    QString newName = QInputDialog::getText(this, "Rename direction", "Enter a new direction description:", QLineEdit::Normal, ld->description(), &ok);
    if(!ok || newName.isEmpty())
        return;

    ld->setDescription(newName);
    ui->lwDirections->currentItem()->setText(newName);
}

void lineEditor::refreshDirections() {
    for(int i = 0; i < m_line.directionCount(); i++) {
        directionTableReference << m_line.directionAt(i);
        ui->lwDirections->addItem(m_line.directionAt(i)->description());
    }
}






















