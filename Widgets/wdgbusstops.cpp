#include "wdgbusstops.h"
#include "ui_wdgbusstops.h"

#include <QMessageBox>
#include <QUndoStack>
#include <QToolBar>

#include "Commands/CmdBusstops.h"

#include "Dialogs/DlgBusstopeditor.h"
#include "Dialogs/DlgDataexporter.h"
#include "App/global.h"

WdgBusstops::WdgBusstops(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgBusstops),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QAction *actionEdit = new QAction(ui->twBusstops);
    actionEdit->setShortcuts({QKeySequence(Qt::Key_Space), QKeySequence(Qt::Key_Return), QKeySequence(Qt::Key_Enter)});
    actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->twBusstops->addAction(actionEdit);

    QAction *actionDelete = new QAction(ui->twBusstops);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->twBusstops->addAction(actionDelete);

    QObject::connect(actionEdit, SIGNAL(triggered()), this, SLOT(actionEdit()));
    QObject::connect(actionDelete, SIGNAL(triggered()), this, SLOT(actionDelete()));

    QObject::connect(ui->pbBusstopNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbBusstopEdit, SIGNAL(clicked()), this, SLOT(actionEdit()));
    QObject::connect(ui->twBusstops, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(actionEdit()));
    QObject::connect(ui->pbBusstopDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));
    QObject::connect(ui->leBusstopSearch, SIGNAL(textChanged(QString)), this, SLOT(actionSearch()));
    QObject::connect(ui->pbExportList, SIGNAL(clicked()), this, SLOT(actionExportList()));
    //QObject::connect(this, SIGNAL(currentBusstopChanged(Busstop*)), this, SLOT(refreshUI()));


    ui->twBusstops->verticalHeader()->setVisible(false);
    ui->twBusstops->setEditTriggers(QTableWidget::NoEditTriggers);
}

WdgBusstops::~WdgBusstops()
{
    delete ui;
}

void WdgBusstops::actionNew() {
    busstopEditor dlg;
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    QString name = dlg.name();
    bool important = dlg.isImportant();

    if(name == "")
        return;

    Busstop *b = projectData->newBusstop();
    b->setName(name);
    b->setImportant(important);
    undoStack->push(new CmdBusstopNew(projectData, b));
    emit refreshRequested();
}

void WdgBusstops::actionEdit() {
    if(!_currentBusstop)
        return;
    
    QString name = _currentBusstop->name();
    bool important = _currentBusstop->isImportant();

    busstopEditor dlg(this, false, name, important);
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    QString newName = dlg.name();
    bool newImportant = dlg.isImportant();

    if(newName == "")
        return;
    
    Busstop newB = *_currentBusstop;

    newB.setName(newName);
    newB.setImportant(newImportant);
    
    undoStack->push(new CmdBusstopEdit(_currentBusstop, newB));
    emit refreshRequested();
}

void WdgBusstops::actionDelete() {
    QModelIndexList selection = ui->twBusstops->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Busstop *> busstops;
    for(int i = 0; i < selection.count(); i++) {
        Busstop *b = tableReference[selection[i].row()];
        busstops << b;
        showList += QString("<li>%1</li>").arg(b->name());
    }
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete busstop(s)"), tr("<p><b>Do you really want to delete these %n busstop(s)?</b></p>%1", "", busstops.count()).arg(showList), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;
    
    undoStack->push(new CmdBusstopsDelete(projectData, busstops));
    emit refreshRequested();
}

void WdgBusstops::actionSearch() {
    refresh();
}

void WdgBusstops::setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDelete) {
    _actionNew = actionNew;
    _actionEdit = actionEdit;
    _actionDelete = actionDelete;

    refreshUI();
}

void WdgBusstops::refreshUI() {
    int selectionCount = ui->twBusstops->selectionModel()->selectedRows(0).count();

    if(selectionCount == 0) {
        ui->twBusstops->setCurrentItem(nullptr);
        ui->pbBusstopEdit->setEnabled(false);
        ui->pbBusstopDelete->setEnabled(false);
        _actionEdit->setEnabled(false);
        _actionDelete->setEnabled(false);
    } else if(selectionCount == 1) {
        ui->pbBusstopEdit->setEnabled(true);
        ui->pbBusstopDelete->setEnabled(true);
        _actionEdit->setEnabled(true);
        _actionDelete->setEnabled(true);
    } else {
        ui->pbBusstopEdit->setEnabled(false);
        ui->pbBusstopDelete->setEnabled(true);
        _actionEdit->setEnabled(false);
        _actionDelete->setEnabled(true);
    }
}

void WdgBusstops::actionExportList() {
    QString plainText, csvText, htmlText;

    QFileInfo fi(projectData->filePath());

    csvText = "Name;Important;used by lines\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All busstops of project " + fi.baseName() + "</title></head>\r\n\t<body>\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Used by lines</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->busstopCount(); i++) {
        Busstop *b = projectData->busstopAt(i);
        QString name = b->name();
        bool important = b->isImportant();
        QString importantStr = important ? "Yes" : "No";
        QStringList usedLinesStrList;
        QList<Line *> usedLines = projectData->linesAtBusstop(b);
        for(int i = 0; i < usedLines.count(); i++)
            usedLinesStrList << usedLines[i]->name();
        QString usedLinesStr = usedLinesStrList.join(", ");
        plainText += name + "\r\n";

        csvText += "\"" + name + "\";" + importantStr + ";" + usedLinesStr + "\r\n";
        if(important)
            htmlText += "\t\t\t\t\t<tr><td><b>" + name + "</b></td><td>" + usedLinesStr + "</td></tr>\r\n";
        else
            htmlText += "\t\t\t\t\t<tr><td>" + name + "</td><td>" + usedLinesStr + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFileName("all_busstops");
    dlg.setFolderName(fi.baseName());
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}

Busstop *WdgBusstops::currentBusstop()
{
    return _currentBusstop;
}

void WdgBusstops::refresh()
{
    qDebug() << "refreshing busstop list...";
    refreshing = true;

    QString filter = ui->leBusstopSearch->text();
    ui->twBusstops->setRowCount(0);
    tableReference.clear();

    QList<Busstop *> busstops = projectData->busstops();
    busstops = ProjectData::sortItems(busstops);
    
    for(int i = 0; i < busstops.count(); i++) {
        Busstop * b = busstops[i];

        //QString id = b->id();
        QString name = b->name();
        bool important = b->isImportant();

        if(!name.contains(filter, Qt::CaseInsensitive))
            continue;

        int currentRow = ui->twBusstops->rowCount();
        ui->twBusstops->insertRow(currentRow);

        QStringList usedLinesStrList;
        QList<Line *> usedLines = projectData->linesAtBusstop(b);
        for(int i = 0; i < usedLines.count(); i++)
            usedLinesStrList << usedLines[i]->name();
            //usedLinesStrList << "[" + usedLines[i]->name() + "]";
        QString usedLinesStr = usedLinesStrList.join(", ");

        QTableWidgetItem * nameItm = new QTableWidgetItem(name);
        QTableWidgetItem * usedLinesItm = new QTableWidgetItem(usedLinesStr);

        QFont bold;
        bold.setBold(true);
        if(important)
            nameItm->setFont(bold);

        ui->twBusstops->setItem(currentRow, 0, nameItm);
        ui->twBusstops->setItem(currentRow, 1, usedLinesItm);

        tableReference << b;
        
        if(b == _currentBusstop)
            ui->twBusstops->setCurrentItem(nameItm);
    }

    if(ui->leBusstopSearch->text().isEmpty()) {
        ui->twBusstops->resizeColumnToContents(0);
        ui->twBusstops->resizeColumnToContents(1);
    }

    for(int i = 0; i < ui->twBusstops->rowCount(); i++) {
        ui->twBusstops->setRowHeight(i, 15);
    }

    refreshing = false;
}

void WdgBusstops::on_twBusstops_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twBusstops->currentItem();
    int selectionCount = ui->twBusstops->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        _currentBusstop = nullptr;
    else
        _currentBusstop = tableReference[current->row()];

    refreshUI();

    emit currentBusstopChanged(_currentBusstop);
}
