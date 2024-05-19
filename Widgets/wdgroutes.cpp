#include "wdgroutes.h"
#include "ui_wdgroutes.h"

#include <QUndoStack>
#include <QMessageBox>

#include <QFileDialog>

#include "Mainwindow.h"
#include "Dialogs/DlgRouteeditor.h"
#include "Commands/CmdRoutes.h"

WdgRoutes::WdgRoutes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgRoutes),
    projectData(((MainWindow *)parent)->projectData()),
    _currentLine(nullptr),
    _currentRoute(nullptr),
    refreshing(false) {
    ui->setupUi(this);

    _actionNew       = ui->twRoutes->addAction(QIcon(":/icons/Add.ico"),       tr("New"));
    _actionEdit      = ui->twRoutes->addAction(QIcon(":/icons/Edit.ico"),      tr("Edit"));
    _actionDuplicate = ui->twRoutes->addAction(QIcon(":/icons/Duplicate.ico"), tr("Duplicate"));
    _actionDelete    = ui->twRoutes->addAction(QIcon(":/icons/Delete.ico"),    tr("Delete"));

    _actionEdit->setDisabled(true);
    _actionDuplicate->setDisabled(true);
    _actionDelete->setDisabled(true);

    _actionEdit->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Return), QKeySequence(Qt::Key_Space)});
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDuplicate->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_D));
    _actionDuplicate->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete->setShortcut(QKeySequence::Delete);
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->twRoutes->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_actionNew,       &QAction::triggered,             this,             &WdgRoutes::actionNew);
    connect(_actionEdit,      &QAction::triggered,             this,             &WdgRoutes::actionEdit);
    connect(_actionDuplicate, &QAction::triggered,             this,             &WdgRoutes::actionDuplicate);
    connect(_actionDelete,    &QAction::triggered,             this,             &WdgRoutes::actionDelete);

    connect(_actionNew,       &QAction::enabledChanged,         ui->pbNew,       &QPushButton::setEnabled);
    connect(_actionEdit,      &QAction::enabledChanged,         ui->pbEdit,      &QPushButton::setEnabled);
    connect(_actionDuplicate, &QAction::enabledChanged,         ui->pbDuplicate, &QPushButton::setEnabled);
    connect(_actionDelete,    &QAction::enabledChanged,         ui->pbDelete,    &QPushButton::setEnabled);

    connect(ui->pbNew,        &QPushButton::clicked,            this,            &WdgRoutes::actionNew);
    connect(ui->pbEdit,       &QPushButton::clicked,            this,            &WdgRoutes::actionEdit);
    connect(ui->twRoutes,     &QTableWidget::cellDoubleClicked, this,            &WdgRoutes::actionEdit);
    connect(ui->pbDuplicate,  &QPushButton::clicked,            this,            &WdgRoutes::actionDuplicate);
    connect(ui->pbDelete,     &QPushButton::clicked,            this,            &WdgRoutes::actionDelete);

    connect(ui->leSearch,     &QLineEdit::textChanged,          this,            &WdgRoutes::refreshRouteList);

    QObject::connect(ui->pbExportProfilesOMSITrips, &QPushButton::clicked, this, &WdgRoutes::omsiExport);

    ui->twRoutes->setColumnWidth(0, 50);
    ui->twRoutes->setColumnWidth(1, 50);
    ui->twRoutes->setColumnWidth(2, 400);
}

WdgRoutes::~WdgRoutes()
{
    delete ui;
}

QList<QAction *> WdgRoutes::actions() {
    return ui->twRoutes->actions();
}

void WdgRoutes::refreshUI() {
    int selectionCount = ui->twRoutes->selectionModel()->selectedRows(0).count();
    _actionNew->setEnabled(selectionCount == 1);
    _actionEdit->setEnabled(selectionCount == 1);
    _actionDuplicate->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);
}

void WdgRoutes::refreshRouteList() {
    qDebug() << "refreshing route list...";

    refreshing = true;

    ui->twRoutes->setRowCount(0);
    tableReference.clear();

    if(!_currentLine)
        return;

    QList<Route *> routes = _currentLine->routes();
    routes = ProjectData::sortItems(routes);

    bool searchCodeEnabled;
    QString search = ui->leSearch->text();
    int searchCode = search.toInt(&searchCodeEnabled);

    int counter = 0;
    QList<LineDirection *> directions = _currentLine->directions();

    // append nullptr to routes to also get all routes with no direction assigned
    foreach(LineDirection *ld, directions << nullptr) {
        QList<Route *> routes = _currentLine->routesToDirection(ld);
        routes = ProjectData::sortItems(routes);

        foreach(Route *r, routes) {
            if(!r->name().contains(search, Qt::CaseInsensitive))
                if(!searchCodeEnabled || r->code() != searchCode)
                    continue;

            tableReference << r;

            QString code = QString::number(r->code());

            QString firstBusstop, lastBusstop;
            if(r->busstopCount() != 0) {
                firstBusstop = r->firstBusstop()->name();
                lastBusstop = r->lastBusstop()->name();
            }

            if(code.length() == 1)
                code = "0" + code;

            QFont bold;
            bold.setBold(true);

            ui->twRoutes->insertRow(counter);
            ui->twRoutes->setItem(counter, 0, new QTableWidgetItem(code));
            if(ld) ui->twRoutes->setItem(counter, 1, new QTableWidgetItem(ld->description()));
            ui->twRoutes->setItem(counter, 2, new QTableWidgetItem(r->name()));
            ui->twRoutes->setItem(counter, 3, new QTableWidgetItem(firstBusstop));
            ui->twRoutes->setItem(counter, 4, new QTableWidgetItem(lastBusstop));
            ui->twRoutes->setItem(counter, 5, new QTableWidgetItem(QString::number(r->busstopCount())));

            ui->twRoutes->item(counter, 2)->setFont(bold);

            if(_currentRoute == r)
                ui->twRoutes->setCurrentCell(counter, 2);

            counter++;
        }
    }

    for(int i = 0; i < ui->twRoutes->rowCount(); i++) {
        ui->twRoutes->setRowHeight(i, 15);
    }

    ui->twRoutes->resizeColumnsToContents();

    refreshing = false;
}

void WdgRoutes::setCurrentLine(Line *l) {
    if(!l) {
        ui->pbNew->setEnabled(false);
        _actionNew->setEnabled(false);
        _currentLine = nullptr;
    }

    ui->pbNew->setEnabled(true);
    _actionNew->setEnabled(true);

    _currentLine = l;
    refreshRouteList();
}

Route * WdgRoutes::currentRoute() const {
    return _currentRoute;
}

void WdgRoutes::actionNew() {
    if(!_currentLine)
        return;

    Route *r = _currentLine->newRoute();
    r->setDirection(_currentLine->directionAt(0));

    DlgRouteEditor dlg(this, r, true);

    if(dlg.exec() != QDialog::Accepted)
        return;

    *r = dlg.route();

    projectData->undoStack()->push(new CmdRouteNew(_currentLine, r));
    _currentRoute = r;
    emit currentRouteChanged(r);
    emit refreshRequested();
}

void WdgRoutes::actionEdit() {
    if(!_currentLine || !_currentRoute)
        return;
    
    DlgRouteEditor dlg(this, _currentRoute);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Route newR = dlg.route();

    projectData->undoStack()->push(new CmdRouteEdit(_currentRoute, newR));
    emit refreshRequested();
}

void WdgRoutes::actionDuplicate() {
    if(!_currentLine || !_currentRoute)
        return;
    
    DlgRouteEditor dlg(this, _currentRoute);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Route *newR = _currentLine->newRoute(dlg.route());

    projectData->undoStack()->push(new CmdRouteNew(_currentLine, newR));
    emit refreshRequested();
}

void WdgRoutes::actionDelete() {
    QModelIndexList selection = ui->twRoutes->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Route *> routes;
    for(int i = 0; i < selection.count(); i++) {
        Route *r = tableReference[selection[i].row()];
        routes << r;
        showList += QString("<li>%1</li>").arg(r->name());
    }
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("delete route(s)"), tr("<p><b>Do you really want to delete these %n route(s)?</b></p>", "", routes.count()) + showList, QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdRoutesDelete(_currentLine, routes));
    emit refreshRequested();
}

void WdgRoutes::omsiExport() {
    QString path = QFileDialog::getExistingDirectory(this, "", "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2/maps/Region Grundorf V5/TTData", QFileDialog::ShowDirsOnly);
    if(path == "")
        return;

    QDir dir(path);
    if(!dir.exists())
        return;

    for(int i = 0; i < tableReference.count(); i++) {
        Route *r = tableReference[i];
        QFile f(dir.path() + "/" + r->name() + ".ttp");
        if(!f.exists()) {
            qDebug() << "not found: " << r->name();
            continue;
        }

        f.open(QIODevice::ReadOnly);
        QTextStream s(&f);
        s.setEncoding(QStringConverter::Latin1);

        QString resultStr;
        while(!s.atEnd()) {
            QString line = s.readLine();
            if(line != "[profile]")
                resultStr += line + "\r\n";
            else
                break;
        }

        // export
        QString result = "";

        QList<TimeProfile *> profiles = r->timeProfiles();
        for(int i = 0; i < profiles.count(); i++) {
            TimeProfile *p = profiles[i];
            result += "[profile]\r\n" + p->name() + "\r\n" + QString::number(p->duration()) + "\r\n\r\n";

            for(int j = 0; j < r->busstopCount(); j++) {
                Busstop *b = r->busstopAt(j);
                TimeProfileItem *itm = p->busstop(b);
                if(!itm)
                    continue;

                result += "[profile_man_arr_time]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->depValue()) + "\r\n";
                if(itm->busstopMode() != TimeProfileItem::BusstopModeNormal) {
                    result += "[profile_otherstopping]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->busstopMode()) + "\r\n";
                }
            }
        }

        resultStr += result;

        QFile f2(dir.path() + "/" + r->name() + ".ttp");
        f2.open(QIODevice::WriteOnly);
        QTextStream s2(&f2);
        s2.setEncoding(QStringConverter::Latin1);
        s2 << resultStr;
        f2.close();
        f.close();
    }
}

void WdgRoutes::on_twRoutes_itemSelectionChanged() {
    if(refreshing)
        return;

    QTableWidgetItem *current = ui->twRoutes->currentItem();

    int selectionCount = ui->twRoutes->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        _currentRoute = nullptr;
    else
        _currentRoute = tableReference[current->row()];

    refreshUI();

    emit currentRouteChanged(_currentRoute);
}

/*void WdgRoutes::actionExportProfiles() {
    if(!_currentRoute)
        return;

    Route *r = _currentRoute;

    QString result = "";

    QList<TimeProfile *> profiles = r->timeProfiles();
    for(int i = 0; i < profiles.count(); i++) {
        TimeProfile *p = profiles[i];
        result += "[profile]\r\n" + p->name() + "\r\n" + QString::number(p->duration()) + "\r\n\r\n";

        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);
            TimeProfileItem *itm = p->busstop(b);
            if(!itm)
                continue;

            result += "[profile_man_arr_time]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->depValue()) + "\r\n";
            if(itm->busstopMode() != TimeProfileItem::BusstopModeNormal) {
                result += "[profile_otherstopping]\r\n" + QString::number(j) + "\r\n" + QString::number(itm->busstopMode()) + "\r\n";
            }
        }
    }
    DataExporter dlg;
    dlg.setFileName(r->name());
    dlg.setFolderName("profiles");
    dlg.setText(result, "", "");
    dlg.exec();
}*/
