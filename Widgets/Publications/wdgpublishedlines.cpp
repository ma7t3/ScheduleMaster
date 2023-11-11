#include "wdgpublishedlines.h"
#include "ui_wdgpublishedlines.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

#include "App/global.h"

#include "Commands/CmdPublications.h"

WdgPublishedLines::WdgPublishedLines(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgPublishedLines),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QObject::connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));

    QObject::connect(ui->leTitle, SIGNAL(editingFinished()), this, SLOT(actionEdit()));
    QObject::connect(ui->leSubTitle, SIGNAL(editingFinished()), this, SLOT(actionEdit()));

    QObject::connect(ui->pbDirectionNew, SIGNAL(clicked()), this, SLOT(actionDirectionNew()));
    QObject::connect(ui->pbDirectionDelete, SIGNAL(clicked()), this, SLOT(actionDirectionDelete()));

    QObject::connect(ui->leDirectionsName, SIGNAL(editingFinished()), this, SLOT(actionDirectionEdit()));
}

WdgPublishedLines::~WdgPublishedLines()
{
    delete ui;
}

PublishedLine *WdgPublishedLines::currentLine() const {
    return m_currentLine;
}

void WdgPublishedLines::actionNew() {
    bool ok = false;
    QString name = QInputDialog::getText(this, "New line", "Enter line name:", QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLine *l = new PublishedLine(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineNew(projectData, l));
    m_currentLine = l;
    refreshLineList();
    refreshCurrentLine();
}

void WdgPublishedLines::actionEdit() {
    if(!m_currentLine)
        return;

    PublishedLine newL = *m_currentLine;

    newL.setTitle(ui->leTitle->text());
    newL.setSubTitle(ui->leSubTitle->text());
    newL.setFilePath(ui->leFilePath->text());

    undoStack->push(new cmdPublishedLineEdit(m_currentLine, newL));
    refreshLineList();
}

void WdgPublishedLines::actionDelete() {
    if(!m_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete published line"), tr("<p><b>Do you really want to delete this published line?</b></p><p>%1</p>").arg(m_currentLine->title()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdPublishedLineDelete(projectData, m_currentLine));
    refreshLineList();
}

void WdgPublishedLines::actionDirectionNew() {
    bool ok = false;
    QString name = QInputDialog::getText(this, "New direction", "Enter direction name:", QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLineDirection *ld = new PublishedLineDirection(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineDirectionNew(m_currentLine, ld));
    m_currentLineDirection = ld;
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionEdit() {
    if(!m_currentLineDirection)
        return;

    PublishedLineDirection newLd = *m_currentLineDirection;

    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionDelete() {

}

void WdgPublishedLines::setCurrentLine(PublishedLine *newCurrentLine) {
    m_currentLine = newCurrentLine;
}

void WdgPublishedLines::refreshLineList() {
    refreshing = true;
    ui->lwLines->clear();
    m_listReference.clear();

    QList<PublishedLine *> publishedLines = projectData->publications()->lines();

    for(int i = 0; i < publishedLines.count(); i++) {
        PublishedLine *l = publishedLines[i];
        m_listReference << l;
        ui->lwLines->addItem(l->title());

        if(m_currentLine == l)
            ui->lwLines->setCurrentRow(i);
    }

    refreshing = false;
}

void WdgPublishedLines::refreshCurrentLine() {
    ui->lwDirections->clear();
    m_directionsListReference.clear();

    if(!m_currentLine)
        return;

    refreshingCurrentLine = true;

    ui->leTitle->setText(m_currentLine->title());
    ui->leSubTitle->setText(m_currentLine->subTitle());
    ui->leFilePath->setText(m_currentLine->filePath());

    QList<PublishedLineDirection *> directions = m_currentLine->directions();
    for(int i = 0; i < directions.count(); i++) {
        PublishedLineDirection *ld = directions[i];
        ui->lwDirections->addItem(ld->name());
        m_directionsListReference << ld;
    }

    ui->leDirectionsName->clear();

    if(!m_currentLineDirection)
        return;

    ui->leDirectionsName->setText(m_currentLineDirection->name());

    refreshingCurrentLine = false;
}

void WdgPublishedLines::on_lwLines_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshing)
        return;

    if(!current)
        m_currentLine = nullptr;

    m_currentLine = m_listReference[ui->lwLines->currentRow()];

    emit currentLineChanged(m_currentLine);
    refreshCurrentLine();
}

void WdgPublishedLines::on_lwDirections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshingCurrentLine)
        return;

    if(!current)
        m_currentLineDirection = nullptr;

    m_currentLineDirection = m_directionsListReference[ui->lwDirections->currentRow()];

    //emit currentLineChanged(m_currentLineDirection);
    refreshCurrentLine();
}






void WdgPublishedLines::on_pbFilePathBrowse_clicked() {
    if(m_currentLine)
        return;

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Publications/";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString fileName = QFileDialog::getSaveFileName(this, "", dir.path(), "Portable document format (*.pdf)");
    if(fileName == "")
        return;

    ui->leFilePath->setText(fileName);
    actionEdit();
}



