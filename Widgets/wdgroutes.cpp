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
    _model(new RouteTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _currentLine(nullptr),
    _currentRoute(nullptr) {
    ui->setupUi(this);

    ui->twRoutes->setModel(_model);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxyModel->setFilterKeyColumn(-1);
    _proxyModel->setSortRole(0x0100);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->sort(1, Qt::AscendingOrder);

    ui->twRoutes->setSortingEnabled(true);
    ui->twRoutes->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    ui->twRoutes->setModel(_proxyModel);


    ui->twRoutes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->twRoutes->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);


    connect(ui->twRoutes->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WdgRoutes::onSelectionChanged);
    connect(_proxyModel, &QAbstractItemModel::rowsInserted, this, &WdgRoutes::onRowsInserted);

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

    connect(_actionNew,       &QAction::triggered,          this,            &WdgRoutes::actionNew);
    connect(_actionEdit,      &QAction::triggered,          this,            &WdgRoutes::actionEdit);
    connect(_actionDuplicate, &QAction::triggered,          this,            &WdgRoutes::actionDuplicate);
    connect(_actionDelete,    &QAction::triggered,          this,            &WdgRoutes::actionDelete);

    connect(_actionNew,       &QAction::enabledChanged,     ui->pbNew,       &QPushButton::setEnabled);
    connect(_actionEdit,      &QAction::enabledChanged,     ui->pbEdit,      &QPushButton::setEnabled);
    connect(_actionDuplicate, &QAction::enabledChanged,     ui->pbDuplicate, &QPushButton::setEnabled);
    connect(_actionDelete,    &QAction::enabledChanged,     ui->pbDelete,    &QPushButton::setEnabled);

    connect(ui->pbNew,        &QPushButton::clicked,        this,            &WdgRoutes::actionNew);
    connect(ui->pbEdit,       &QPushButton::clicked,        this,            &WdgRoutes::actionEdit);
    connect(ui->twRoutes,     &QTableWidget::doubleClicked, this,            &WdgRoutes::actionEdit);
    connect(ui->pbDuplicate,  &QPushButton::clicked,        this,            &WdgRoutes::actionDuplicate);
    connect(ui->pbDelete,     &QPushButton::clicked,        this,            &WdgRoutes::actionDelete);

    connect(ui->leSearch,     &QLineEdit::textChanged,      _proxyModel,     &QSortFilterProxyModel::setFilterFixedString);

    QObject::connect(ui->pbExportProfilesOMSITrips, &QPushButton::clicked, this, &WdgRoutes::omsiExport);
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
    _actionNew->setEnabled(_currentLine);
    _actionEdit->setEnabled(selectionCount == 1);
    _actionDuplicate->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);
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
    _model->setLine(l);
    refreshUI();
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
        Route *r = _model->itemAt(_proxyModel->mapToSource(selection[i]).row());
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

    for(int i = 0; i < _model->itemCount(); i++) {
        Route *r = _model->itemAt(i);
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

void WdgRoutes::onSelectionChanged() {
    const QModelIndex current = ui->twRoutes->selectionModel()->currentIndex();
    int selectionCount        = ui->twRoutes->selectionModel()->selectedRows().count();

    if(selectionCount != 1)
        _currentRoute = nullptr;
    else
        _currentRoute = _model->itemAt(_proxyModel->mapToSource(current).row());
    refreshUI();
    emit currentRouteChanged(_currentRoute);
}

void WdgRoutes::onRowsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);
    ui->twRoutes->setCurrentIndex(_proxyModel->index(first, 0));
    ui->twRoutes->selectionModel()->select(QItemSelection(_proxyModel->index(first, 0), _proxyModel->index(last, 5)), QItemSelectionModel::ClearAndSelect);
    ui->twRoutes->setFocus();
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
