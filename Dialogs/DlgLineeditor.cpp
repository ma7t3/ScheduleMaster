#include "Dialogs\DlgLineeditor.h"
#include "ui_DlgLineeditor.h"

#include <QColorDialog>
#include <QInputDialog>

DlgLineEditor::DlgLineEditor(QWidget *parent, Line l) :
    QDialog(parent),
    ui(new Ui::DlgLineEditor),
    m_line(l)
{
    ui->setupUi(this);

    QObject::connect(ui->lwDirections, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(actionRenameDirection()));
    QObject::connect(ui->pbDirectionRename, SIGNAL(clicked()), this, SLOT(actionRenameDirection()));

    QList<LineDirection *> directions;
    for(int i = 0; i < m_line.directionCount(); i++) {
        LineDirection *ld = new LineDirection(*m_line.directionAt(i));
        directions << ld;
    }

    m_line.setDirections(directions);

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
    m_line.setName(ui->leName->text());
    m_line.setDescription(ui->leDescription->text());
    m_line.setColor(QColor(ui->lColorName->text()));
    return m_line;
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

void DlgLineEditor::actionRenameDirection() {
    if(!ui->lwDirections->currentItem())
        return;

    LineDirection *ld = m_line.directionAt(ui->lwDirections->currentRow());

    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename direction"), tr("Enter a new direction description:"), QLineEdit::Normal, ld->description(), &ok);
    if(!ok || newName.isEmpty())
        return;

    ld->setDescription(newName);
    ui->lwDirections->currentItem()->setText(newName);
}

void DlgLineEditor::refreshDirections() {
    for(int i = 0; i < m_line.directionCount(); i++) {
        ui->lwDirections->addItem(m_line.directionAt(i)->description());
    }
}






















