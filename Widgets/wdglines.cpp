#include "wdglines.h"
#include "ui_wdglines.h"

#include "Dialogs/DlgLineeditor.h"
#include "Dialogs/DlgDataexporter.h"
#include "Commands/cmdLines.h"
#include "ProjectData/projectdata.h"
#include "App/global.h"

#include <QUndoStack>
#include <QMessageBox>



WdgLines::WdgLines(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgLines),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QObject::connect(ui->pbLineNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbLineEdit, SIGNAL(clicked()), this, SLOT(actionEdit()));
    QObject::connect(ui->twLines, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(actionEdit()));
    QObject::connect(ui->pbLineDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));
    QObject::connect(ui->pbExportList, SIGNAL(clicked()), this, SLOT(actionExportList()));

    QObject::connect(m_actionNew, SIGNAL(triggered()), this, SLOT(actionNew()));
    QObject::connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(actionEdit()));
    QObject::connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(actionDelete()));
    QObject::connect(m_actionExportList, SIGNAL(triggered()), this, SLOT(actionExportList()));
    QObject::connect(m_actionExportListAndRoutes, SIGNAL(triggered()), this, SLOT(actionExportListAndRoutes()));

    m_actionNew->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_L));
    m_actionEdit->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_L));

    ui->twLines->verticalHeader()->setVisible(false);
    ui->twLines->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twLines->setColumnWidth(0, 50);
    ui->twLines->setColumnWidth(1, 400);
}

WdgLines::~WdgLines()
{
    delete ui;
}

void WdgLines::actionNew() {
    lineEditor dlg(this);
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Line *l = new Line(dlg.line());
    if(l->name() == "")
        return;

    undoStack->push(new cmdLineNew(projectData, l));
    refreshLineTable();
}


void WdgLines::actionEdit() {
    Line *l = currentLine();
    if(!l)
        return;

    lineEditor dlg(this, *l);
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Line newL = dlg.line();

    if(newL.name() == "")
        return;

    undoStack->push(new cmdLineEdit(l, newL));
    refreshLineTable();
}

void WdgLines::actionDelete() {
    QModelIndexList selection = ui->twLines->selectionModel()->selectedRows(0);

    QString showList ="<table style=\"border-collapse: collapse;\">";
    QList<Line *> lines;
    for(int i = 0; i < selection.count(); i++) {
        Line *l = tableRefernce[selection[i].row()];
        lines << l;
        QColor color = l->getColor();
        QColor contrastColor = global::getContrastColor(color);
        showList += QString("<tr style=\"color: %1; background-color: %2\"><td style=\"padding: 5px;\">%3</td><td style=\"padding: 5px;\">%4</td>").arg(contrastColor.name(QColor::HexRgb), color.name(QColor::HexRgb), l->name(), l->getDescription());
    }
    showList += "</table>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "Delete line(s)", tr("<p><b>Do you really want to delete these %n line(s)?</b></p><p></p>", "", lines.count()) + showList, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdLinesDelete(projectData, lines));
    refreshLineTable();
}

void WdgLines::actionExportList() {
    QString plainText, csvText, htmlText;

    QFileInfo fi(projectData->filePath());

    csvText = "Name;Description\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All lines of project " + fi.baseName() + "</title></head>\r\n\t<body>\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Description</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QString name = l->name();
        QColor color = l->getColor();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->getDescription();

        plainText += "[" + name + "] " + description + "\r\n";
        csvText += "\"" + name + "\";\"" + description + "\"\r\n";
        htmlText += "\t\t\t\t\t<tr><td style=\"color: " + contrastColor.name(QColor::HexRgb) + "; background-color: " + color.name(QColor::HexRgb) + "\"><b>" + name + "</b></td><td>" + description + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFolderName(fi.baseName());
    dlg.setFileName("all_lines");
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}

void WdgLines::actionExportListAndRoutes() {
    QString plainText, csvText, htmlText;

    QFileInfo fi(projectData->filePath());

    csvText = "Name;Description\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All lines of project " + fi.baseName() + "</title></head>\r\n<body>\r\n";
    htmlText+= "<style>        * {        font-family: sans-serif;    }        table {    border: 1px solid black;        border-collapse: collapse;}        td, th {    border: 1px solid black;    padding: 5px 10px;        text-align: left;    }    </style>";
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QString name = l->name();
        QColor color = l->getColor();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->getDescription();

        plainText += "[" + name + "] " + description + "\r\n";
        csvText += "\"" + name + "\";\"" + description + "\"\r\n";
        htmlText += "\t<h2 style=\"color: " + contrastColor.name(QColor::HexRgb) + "; background-color: " + color.name(QColor::HexRgb) + "\"><b>" + name + "</b> (" + description + ")</h2>\r\n";
        htmlText += "\t<table style=\"border: 1px solid black; border-collapse: collapse;\">\r\n\t\t\t<thead><tr><th>Code</th><th>Name</th><th>First busstop</th><th>Last busstop</th></tr></thead>";

        QList<Route *> routes = l->routes();
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = routes[j];
            plainText += "\t\t[" + QString::number(r->code()) + "] " + r->name() + "\r\n";
            csvText += ";" + QString::number(r->code()) + ";\"" + r->name() + "\"\r\n";

            htmlText += "\t\t<tr><td>" + QString::number(r->code()) + "</td><td><b>" + r->name() + "</b></td><td>" + r->firstBusstop()->name() + "</td><td>" + r->lastBusstop()->name() + "</td></tr>\r\n";
        }

        htmlText += "\t\t</thead>\r\n\t</table>\r\n";
    }

    htmlText += "\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFolderName(fi.baseName());
    dlg.setFileName("all_lines_with_routes");
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}

void WdgLines::setMenubarActions(QAction *actionNew, QAction *actionEdit, QAction *actionDelete) {
    m_actionNew = actionNew;
    m_actionEdit = actionEdit;
    m_actionDelete = actionDelete;

    refreshUI();
}

void WdgLines::refreshUI() {
    int selectionCount = ui->twLines->selectionModel()->selectedRows(0).count();

    if(selectionCount == 0) {
        ui->twLines->setCurrentItem(nullptr);
        ui->pbLineEdit->setEnabled(false);
        ui->pbLineDelete->setEnabled(false);
        m_actionEdit->setEnabled(false);
        m_actionDelete->setEnabled(false);
    } else if(selectionCount == 1) {
        ui->pbLineEdit->setEnabled(true);
        ui->pbLineDelete->setEnabled(true);
        m_actionEdit->setEnabled(true);
        m_actionDelete->setEnabled(true);
    } else {
        ui->pbLineEdit->setEnabled(false);
        ui->pbLineDelete->setEnabled(true);
        m_actionEdit->setEnabled(false);
        m_actionDelete->setEnabled(true);
    }
}


Line * WdgLines::currentLine() {
    return m_currentLine;
}

void WdgLines::refreshLineTable() {
    refreshing = true;

    ui->twLines->setRowCount(0);
    tableRefernce.clear();

    QFont bold;
    bold.setBold(true);

    QList<Line *> lines = projectData->lines();
    lines = ProjectData::sortLines(lines);
    
    for(int i = 0; i < lines.count(); i++) {
        Line * l = lines[i];

        tableRefernce << l;

        QString name = l->name();
        QString description = l->getDescription();
        QColor color = l->getColor();

        int currentRow = ui->twLines->rowCount();
        ui->twLines->insertRow(currentRow);

        QTableWidgetItem * nameItm = new QTableWidgetItem(name);
        QTableWidgetItem * descriptionItm = new QTableWidgetItem(description);

        nameItm->setBackground(QBrush(color));
        nameItm->setForeground(global::getContrastColor(color));
        nameItm->setFont(bold);

        ui->twLines->setItem(currentRow, 0, nameItm);
        ui->twLines->setItem(currentRow, 1, descriptionItm);

        if(l == m_currentLine)
            ui->twLines->setCurrentCell(i, 0);
    }

    for(int i = 0; i < ui->twLines->rowCount(); i++) {
        ui->twLines->setRowHeight(i, 15);
    }

    ui->twLines->resizeColumnToContents(0);
    ui->twLines->resizeColumnToContents(1);

    refreshing = false;
}


void WdgLines::on_twLines_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twLines->currentItem();

    int selectionCount = ui->twLines->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        m_currentLine = nullptr;
    else
        m_currentLine = tableRefernce[current->row()];

    refreshUI();

    emit currentLineChanged(m_currentLine);
}


void WdgLines::on_twLines_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    Q_UNUSED(previous);
    if(current && current->column() != 0)
        ui->twLines->setCurrentCell(current->row(), 0);
}

