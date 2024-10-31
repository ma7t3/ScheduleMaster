#include "wdgtours.h"
#include "ui_wdgtours.h"

#include <QMessageBox>
#include <QInputDialog>

#include <QPainter>

#include "Mainwindow.h"
#include "Dialogs/DlgToureditor.h"
#include "Commands/CmdTours.h"
#include "Dialogs/DlgDataexporter.h"

#include "localconfig.h"

WdgTours::WdgTours(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgTours),
    projectData(((MainWindow *)parent)->projectData()),
    _model(new TourTableModel(this)),
    _proxyModel(new QSortFilterProxyModel(this)),
    _currentTour(nullptr) {
    ui->setupUi(this);


    _model->setProjectData(projectData);

    _proxyModel->setSourceModel(_model);
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxyModel->setFilterKeyColumn(0);
    _proxyModel->setSortLocaleAware(true);
    _proxyModel->sort(0, Qt::AscendingOrder);

    ui->twTours->setSortingEnabled(true);
    ui->twTours->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    ui->twTours->setModel(_proxyModel);

    ui->twTours->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->twTours->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->twTours->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->twTours->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->twTours->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->twTours->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

    connect(ui->twTours->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WdgTours::onSelectionChanged);
    connect(_model, &QAbstractItemModel::rowsInserted, this, &WdgTours::onRowsInserted);

    _actionNew          = ui->twTours->addAction(QIcon(":/icons/Add.ico"),             tr("New"));
    _actionEdit         = ui->twTours->addAction(QIcon(":/icons/Edit.ico"),            tr("Edit"));
    _actionDuplicate    = ui->twTours->addAction(QIcon(":/icons/Duplicate.ico"),       tr("Duplicate"));
    _actionDelete       = ui->twTours->addAction(QIcon(":/icons/Delete.ico"),          tr("Delete"));
    _actionExport       = ui->twTours->addAction(QIcon(":/icons/Export.ico"),          tr("Export"));

    _actionEdit->setDisabled(true);
    _actionDuplicate->setDisabled(true);
    _actionDelete->setDisabled(true);
    _actionExport->setDisabled(true);

    _actionEdit->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Space), QKeySequence(Qt::Key_Return)});
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete->setShortcut(QKeySequence::Delete);
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->twTours->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_actionNew,       &QAction::triggered,               this,                &WdgTours::actionNew);
    connect(_actionEdit,      &QAction::triggered,               this,                &WdgTours::actionEdit);
    connect(_actionDuplicate, &QAction::triggered,               this,                &WdgTours::actionDuplicate);
    connect(_actionDelete,    &QAction::triggered,               this,                &WdgTours::actionDelete);
    connect(_actionExport,    &QAction::triggered,               this,                &WdgTours::actionExport);

    connect(_actionNew,       &QAction::enabledChanged,          ui->pbTourNew,       &QPushButton::setEnabled);
    connect(_actionEdit,      &QAction::enabledChanged,          ui->pbTourEdit,      &QPushButton::setEnabled);
    connect(_actionDuplicate, &QAction::enabledChanged,          ui->pbTourDuplicate, &QPushButton::setEnabled);
    connect(_actionDelete,    &QAction::enabledChanged,          ui->pbTourDelete,    &QPushButton::setEnabled);
    connect(_actionExport,    &QAction::enabledChanged,          ui->pbExport,        &QPushButton::setEnabled);

    connect(ui->pbTourNew,    &QPushButton::clicked,             this,                &WdgTours::actionNew);
    connect(ui->pbTourNew,    &QPushButton::clicked,             this,                &WdgTours::actionNew);
    connect(ui->pbTourEdit,   &QPushButton::clicked,             this,                &WdgTours::actionEdit);
    connect(ui->twTours,      &QAbstractItemView::doubleClicked, this,                &WdgTours::actionEdit);
    connect(ui->pbTourDelete, &QPushButton::clicked,             this,                &WdgTours::actionDelete);
    connect(ui->pbExport,     &QPushButton::clicked,             this,                &WdgTours::actionExport);
    connect(ui->leSearch,     &QLineEdit::textChanged,           _proxyModel,         [this](const QString &text){_proxyModel->setFilterRegularExpression(text);});
    connect(ui->pbExport,     &QPushButton::clicked,             this,                &WdgTours::actionExport);
}

WdgTours::~WdgTours() {
    delete ui;
}

QList<QAction *> WdgTours::actions() {
    return ui->twTours->actions();
}

void WdgTours::actionNew() {
    TourEditor dlg(this, true, "", WeekDays(), projectData->projectSettings()->dayTypes());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;
    
    Tour *o = projectData->newTour();
    o->setName(dlg.name());
    o->setWeekDays(dlg.weekDays());
    projectData->undoStack()->push(new CmdTourNew(projectData, o));
    emit refreshRequested();
}

// TODO: Rework TourEditor window to new system
void WdgTours::actionEdit() {
    if(!_currentTour)
        return;
    
    TourEditor dlg(this, false, _currentTour->name(), _currentTour->weekDays(), projectData->projectSettings()->dayTypes());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Tour newO = *_currentTour;

    newO.setName(dlg.name());
    newO.setWeekDays(dlg.weekDays());
    projectData->undoStack()->push(new CmdTourEdit(_currentTour, newO));
    emit refreshRequested();
}

void WdgTours::actionDuplicate() {
    Tour *o = currentTour();
    if(!o)
        return;
    
    TourEditor dlg(this, false, o->name(), o->weekDays(), projectData->projectSettings()->dayTypes());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;
    
    Tour *nO = projectData->newTour();
    nO->setName(dlg.name());
    nO->setWeekDays(dlg.weekDays());
    projectData->undoStack()->push(new CmdTourNew(projectData, nO));
    emit refreshRequested();
}

void WdgTours::actionDelete() {
    QModelIndexList selection = ui->twTours->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Tour *> tours;
    for(int i = 0; i < selection.count(); i++) {
        Tour *o = _model->itemAt(_proxyModel->mapToSource(selection[i]).row());
        tours << o;
        showList += QString("<li>%1</li>").arg(o->name());
    }
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete tour(s)"), tr("<p><b>Do you really want to delete these %n tour(s)?</b></p><p></p>", "", tours.count()) + showList, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdToursDelete(projectData, tours));
    emit refreshRequested();
}

void WdgTours::actionExport() {
    bool ok = false;
    QString aiGroupName = QInputDialog::getText(this, "AI-Group", "Enter target AI-Group name", QLineEdit::Normal, "GVB - Gelenk", &ok);
    if(!ok)
        return;

    QString result = "";

    QModelIndexList selection = ui->twTours->selectionModel()->selectedRows(0);

    for(int i = 0; i < selection.count(); i++) {
        Tour *o = _model->itemAt(_proxyModel->mapToSource(selection[i]).row());

        result += "[newtour]\r\n" + o->name() + "\r\n" + aiGroupName + "\r\n799\r\n\r\n";

        QList<Trip *> trips = o->trips();
        for(int j = 0; j < trips.count(); j++) {
            Trip *t = trips[j];
            int timeProfileIndex = 0;
            int time = t->startTime().msecsSinceStartOfDay() / 1000 / 60;
            Route *r = t->route();
            for(int k = 0; k < r->timeProfileCount(); k++) {
                if(r->timeProfileAt(k) == t->timeProfile()) {
                    timeProfileIndex = k;
                    break;
                }
            }
            result += "  Dep.: " + t->startTime().toString(LocalConfig::timeFormatString(true, false)) + "\r\n[addtrip]\r\n" + t->route()->name() + "\r\n" + QString::number(timeProfileIndex) + "\r\n" + QString::number(time) + "\r\n\r\n";
        }
    }

    DataExporter dlg;
    dlg.setFileName("all_tours");
    dlg.setFolderName("tours");
    dlg.setText(result, "", "");
    dlg.exec();
}

void WdgTours::onSelectionChanged() {
    const QModelIndex current = ui->twTours->selectionModel()->currentIndex();
    const int selectionCount = ui->twTours->selectionModel()->selectedRows().count();

    if(selectionCount != 1)
        _currentTour = nullptr;
    else
        _currentTour = _model->itemAt(_proxyModel->mapToSource(current).row());
    refreshUI();
    emit currentTourChanged(_currentTour);
}

void WdgTours::onRowsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);
    ui->twTours->setCurrentIndex(_proxyModel->mapFromSource(_model->index(first, 0)));
    ui->twTours->selectionModel()->select(QItemSelection(_proxyModel->mapFromSource(_model->index(first, 0)), _proxyModel->mapFromSource(_model->index(last, 5))), QItemSelectionModel::ClearAndSelect);
    ui->twTours->setFocus();
}

// TODO: reimplement
void WdgTours::setCurrentTour(Tour *) {

}

void WdgTours::refreshUI() {
    int selectionCount = ui->twTours->selectionModel()->selectedRows(0).count();

    _actionEdit->setEnabled(selectionCount == 1);
    _actionDuplicate->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);
    _actionExport->setEnabled(selectionCount >= 1);
}

Tour * WdgTours::currentTour() const {
    return _currentTour;
}
