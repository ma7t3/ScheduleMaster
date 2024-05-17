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
    _currentLine(nullptr),
    refreshing(false) {
    ui->setupUi(this);

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

    connect(ui->pbLineNew,    &QPushButton::clicked,            this,             &WdgLines::actionNew);
    connect(ui->pbLineEdit,   &QPushButton::clicked,            this,             &WdgLines::actionEdit);
    connect(ui->twLines,      &QTableWidget::cellDoubleClicked, this,             &WdgLines::actionEdit);
    connect(ui->pbLineDelete, &QPushButton::clicked,            this,             &WdgLines::actionDelete);

    ui->twLines->verticalHeader()->setVisible(false);
    ui->twLines->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twLines->setColumnWidth(0, 50);
    ui->twLines->setColumnWidth(1, 400);
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

void WdgLines::refreshLineList() {
    qDebug() << "refreshing line list...";

    refreshing = true;

    ui->twLines->setRowCount(0);
    tableReference.clear();

    QFont bold;
    bold.setBold(true);

    QList<Line *> lines = projectData->lines();
    lines = ProjectData::sortItems(lines);

    for(int i = 0; i < lines.count(); i++) {
        Line * l = lines[i];

        tableReference << l;

        QString name = l->name();
        QString description = l->description();
        QColor color = l->color();

        int currentRow = ui->twLines->rowCount();
        ui->twLines->insertRow(currentRow);

        QTableWidgetItem * nameItm = new QTableWidgetItem(name);
        QTableWidgetItem * descriptionItm = new QTableWidgetItem(description);

        nameItm->setBackground(QBrush(color));
        nameItm->setForeground(global::getContrastColor(color));
        nameItm->setFont(bold);

        ui->twLines->setItem(currentRow, 0, nameItm);
        ui->twLines->setItem(currentRow, 1, descriptionItm);

        if(l == _currentLine)
            ui->twLines->setCurrentCell(i, 0);
    }

    for(int i = 0; i < ui->twLines->rowCount(); i++) {
        ui->twLines->setRowHeight(i, 15);
    }

    ui->twLines->resizeColumnsToContents();
    refreshing = false;
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
    emit refreshRequested();
}

void WdgLines::actionEdit() {
    Line *l = currentLine();
    if(!l)
        return;

    qDebug() << l->directionAt(0)->description();

    DlgLineEditor dlg(this, l, false);
    if(dlg.exec() != QDialog::Accepted)
        return;

    qDebug() << l->directionAt(0)->description();

    Line newL = dlg.line();

    qDebug() << l->directionAt(0)->description();

    projectData->undoStack()->push(new CmdLineEdit(l, newL));
    emit refreshRequested();
}

void WdgLines::actionDelete() {
    QModelIndexList selection = ui->twLines->selectionModel()->selectedRows(0);

    QString showList ="<table style=\"border-collapse: collapse;\">";
    QList<Line *> lines;
    for(int i = 0; i < selection.count(); i++) {
        Line *l = tableReference[selection[i].row()];
        lines << l;
        QColor color = l->color();
        QColor contrastColor = global::getContrastColor(color);
        showList += QString("<tr style=\"color: %1; background-color: %2\"><td style=\"padding: 5px;\">%3</td><td style=\"padding: 5px;\">%4</td>").arg(contrastColor.name(QColor::HexRgb), color.name(QColor::HexRgb), l->name(), l->description());
    }
    showList += "</table>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete line(s)"), tr("<p><b>Do you really want to delete these %n line(s)?</b></p><p></p>", "", lines.count()) + showList, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdLinesDelete(projectData, lines));
    emit refreshRequested();
}

void WdgLines::on_twLines_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twLines->currentItem();

    int selectionCount = ui->twLines->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        _currentLine = nullptr;
    else
        _currentLine = tableReference[current->row()];

    refreshUI();
    emit currentLineChanged(_currentLine);
}

void WdgLines::on_twLines_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    Q_UNUSED(previous);
    if(current && current->column() != 0)
        ui->twLines->setCurrentCell(current->row(), 0);
}

/*void WdgLines::actionExportList() {
    QString plainText, csvText, htmlText;

    QFileInfo fi(projectData->filePath());

    csvText = "Name;Description\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All lines of project " + fi.baseName() + "</title></head>\r\n\t<body>\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Description</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QString name = l->name();
        QColor color = l->color();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->description();

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
        QColor color = l->color();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->description();

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
}*/
