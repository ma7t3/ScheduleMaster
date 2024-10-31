#include "wdgschedule.h"
#include "ui_wdgschedule.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QInputDialog>

#include "ProjectData/projectdata.h"
#include "Commands/CmdSchedule.h"

#include "localconfig.h"

#include "Mainwindow.h"

#include <QStyleHints>
#include <QMenu>

WdgSchedule::WdgSchedule(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgSchedule),
    projectData(((MainWindow *)parent)->projectData()),
    _model(new ScheduleTableModel(this)),
    _proxyModel(new ScheduleTableFilterProxyModel(this)),
    _directionsModel(new LineDirectionListModel(this)),
    _dayTypesModel(new DayTypeListModel(this)),
    _currentLine(nullptr),
    _scheduleStartTimeChanging(false) {
    ui->setupUi(this);

    ui->twSchedule->horizontalHeader()->setDefaultSectionSize(LocalConfig::timeFormat() == LocalConfig::Hours12 ? 60 : 50);
    ui->twSchedule->verticalHeader()->setDefaultSectionSize(10);

    _proxyModel->setSourceModel(_model);
    ui->twSchedule->setModel(_proxyModel);

    ui->cmbDirections->setModel(_directionsModel);

    _dayTypesModel->setProjectData(projectData);
    ui->cmbDayTypes->setModel(_dayTypesModel);

    // generate hour break actions
    for(int i = 0; i < 24; i++) {
        QAction *act = addAction(QString::number(i));
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, [this, i]() {
            actionSetHourBreak(i);
        });
        _hourBreakActions << act;
    }

    ui->twSchedule->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_model,                           &QAbstractItemModel::columnsInserted,   this,                        &WdgSchedule::onColumnsInserted);
    connect(ui->twSchedule,                   &QWidget::customContextMenuRequested,   this,                        &WdgSchedule::showContextMenu);
    connect(ui->twSchedule->selectionModel(), &QItemSelectionModel::currentChanged,   this,                        &WdgSchedule::onCurrentCellChanged);
    connect(ui->twSchedule->selectionModel(), &QItemSelectionModel::selectionChanged, this,                        &WdgSchedule::onSelectionChanged);
    // TODO: Open Tour
    connect(ui->twSchedule,                   &QTableWidget::doubleClicked,           this,                        &WdgSchedule::actionOpenBusstopSchedule);

    connect(projectData->projectSettings(),   &ProjectSettings::changed,              this, [this](){
        if(projectData->projectSettings()->dayTypeCount() > 0)
            on_cmbDayTypes_activated(0);
    });

    QFont boldFont;
    boldFont.setBold(true);

    _actionCurrentBusstopName       = new QAction(this);
    _actionCurrentBusstopName       ->setIcon(QIcon(":/icons/Busstop.ico"));
    _actionCurrentBusstopName       ->setFont(boldFont);

    _actionNew                      = ui->twSchedule->addAction(QIcon(":/icons/Add.ico"),              tr("Add Trip"));
    _actionCopy                     = ui->twSchedule->addAction(QIcon(":/icons/Duplicate.ico"),        tr("Copy Trip"));

    _actionDelete                   = ui->twSchedule->addAction(QIcon(":/icons/Delete.ico"),           tr("Delete Trip(s)"));
    _actionDelete                   ->setShortcut(Qt::Key_Delete);
    _actionDelete                   ->setShortcutContext(Qt::WidgetShortcut);

    _actionOpenBusstopSchedule      = ui->twSchedule->addAction(QIcon(":/icons/BusstopSchedule.ico"),  tr("View Busstop Schedule"));
    _actionOpenBusstopSchedule      ->setShortcuts({QKeySequence(Qt::ALT|Qt::Key_Enter),QKeySequence(Qt::ALT|Qt::Key_Return)});

    _actionShowOnlyImportantBusstops = addAction(QIcon(":/icons/ImportantBusstop.ico"), tr("Show only important Busstops"));
    _actionShowOnlyImportantBusstops->setCheckable(true);


    connect(ui->cbOnlyImportantBusstops, &QCheckBox::checkStateChanged, _actionShowOnlyImportantBusstops, [this](Qt::CheckState state){
        _actionShowOnlyImportantBusstops->setChecked(state == Qt::Checked);
    });

    connect(_actionShowOnlyImportantBusstops, &QAction::toggled,                      ui->cbOnlyImportantBusstops, &QCheckBox::setChecked);
    connect(_actionOpenBusstopSchedule,       &QAction::triggered,                    this,                        &WdgSchedule::actionOpenBusstopSchedule);
    connect(ui->cmbDirections,                &QComboBox::activated,                  this,                        &WdgSchedule::actionChangeDirection);

    connect(_actionShowOnlyImportantBusstops, &QAction::toggled,                      _proxyModel,                 &ScheduleTableFilterProxyModel::setOnlyImportantBusstop);
    connect(ui->leSearchBusstop,              &QLineEdit::textChanged,                _proxyModel,                 &ScheduleTableFilterProxyModel::setBusstopNameFilter);

    refreshUI();
}

WdgSchedule::~WdgSchedule() {
    delete ui;
}

QList<QAction *> WdgSchedule::actions() {
    return ui->twSchedule->actions() << _actionShowOnlyImportantBusstops;
}

QList<QAction *> WdgSchedule::hourBreakActions() {
    return _hourBreakActions;
}

void WdgSchedule::refreshUI() {
    if(!_currentLine) {
        _actionNew                ->setEnabled(false);
        _actionCopy               ->setEnabled(false);
        _actionOpenBusstopSchedule->setEnabled(false);
        _actionDelete             ->setEnabled(false);
        return;
    }

    const int tripCount = _currentTrips.count();
    QModelIndex currentIndex = ui->twSchedule->currentIndex();

    _actionNew ->setEnabled(tripCount <= 1);
    _actionCopy->setEnabled(tripCount == 1);
    if(currentIndex.isValid())
        _actionOpenBusstopSchedule->setEnabled(currentIndex.row() >= ScheduleTableModel::HeaderRowCount);
    else
        _actionOpenBusstopSchedule->setEnabled(false);
    _actionDelete->setEnabled(tripCount > 0);
}

void WdgSchedule::refreshHourBreak() {
    for(int i = 0; i < 24; i++)
        _hourBreakActions[i]->setChecked(false);

    if(!_currentLine)
        return;

    _hourBreakActions[_currentLine->hourBreak()]->setChecked(true);
}

void WdgSchedule::setCurrentLine(Line *l) {
    _currentTrips.clear();
    _currentLine = l;

    _model->setLine(l);
    _directionsModel->setLine(l);

    LineDirection *direction = _directionsModel->itemAt(ui->cmbDirections->currentIndex());
    _model->setDirection(direction);

    emit currentLineChanged(_currentLine, direction);
    emit currentTripsChanged(_currentTrips);

    refreshHourBreak();
    refreshUI();
}

void WdgSchedule::setCurrenTrips(const QList<Trip *> trips) {
    _currentTrips = trips;
    refreshUI();
}

void WdgSchedule::actionChangeDirection()
{
    LineDirection *direction = _directionsModel->itemAt(ui->cmbDirections->currentIndex());

    _currentTrips.clear();
    emit currentLineChanged(_currentLine, direction);
    emit currentTripsChanged(_currentTrips);
    _model->setDirection(direction);
}

void WdgSchedule::actionSetHourBreak(const int &newLineHourBreak) {
    if(!_currentLine)
        return;

    for(int i = 0; i < 24; i++)
        _hourBreakActions[i]->setChecked(false);
    _hourBreakActions[newLineHourBreak]->setChecked(true);

    projectData->undoStack()->push(new CmdScheduleChangeHourBreak(_currentLine, newLineHourBreak));
}

void WdgSchedule::actionOpenBusstopSchedule() {
    QModelIndex currentIndex = ui->twSchedule->currentIndex();
    if(!currentIndex.isValid())
        return;

    if(currentIndex.row() == 1) {
        int index = currentIndex.column();
        Trip *t = _model->itemAt(index);
        QList<Tour *> tours = projectData->toursOfTrip(t);

        if(tours.count() == 0)
            return;

        if(tours.count() == 1) {
            emit tourRequested(tours[0]);
        } else {
            QStringList tourNames;
            foreach (Tour *o, tours) {
                tourNames << o->name();
            }
            bool ok;
            QString result = QInputDialog::getItem(this, tr("Select tour"), tr("Select the tour you want to open:"), tourNames, 0, false, &ok);
            if(!ok)
                return;

            for (int i = 0; i < tourNames.count(); i++) {
                if(tourNames[i] == result)
                    emit tourRequested(tours[i]);
            }
        }
    } else {
        int index = currentIndex.row();
        if(index < ScheduleTableModel::HeaderRowCount)
            return;

        Busstop *b = _model->busstopAt(index);
        QList<Route *> routes;
        for (int i = 0; i < _currentLine->routeCount(); ++i) {
            Route *r = _currentLine->routeAt(i);
            if(r->direction() == _directionsModel->itemAt(ui->cmbDirections->currentIndex()))
                routes << r;
        }
        emit busstopScheduleRequested(b, routes, _dayTypesModel->itemAt(ui->cmbDayTypes->currentIndex()));
    }
}

bool WdgSchedule::checkMatchingWeekdays(WeekDays d) {
    DayType *dt = _dayTypesModel->itemAt(ui->cmbDayTypes->currentIndex());

    if(!dt)
        return false;

    return WeekDays::overlap(d, *dt);
}

void WdgSchedule::on_cmbDayTypes_activated(int index) {
    DayType *dt = _dayTypesModel->itemAt(index);
    _currentTrips.clear();
    emit currentDayTypeChanged(dt);
    emit currentTripsChanged(_currentTrips);
    _model->setWeekDays(*dt);
}

void WdgSchedule::onSelectionChanged() {
    QModelIndexList selection = ui->twSchedule->selectionModel()->selectedColumns();

    _currentTrips.clear();
    for(QModelIndex index : selection) {
        _currentTrips << _model->itemAt(index.column());
    }
    emit currentTripsChanged(_currentTrips);
    refreshUI();
}

void WdgSchedule::onCurrentCellChanged() {
    refreshUI();
}

void WdgSchedule::onColumnsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);
    ui->twSchedule->setCurrentIndex(_model->index(first, 0));
    ui->twSchedule->selectionModel()->select(QItemSelection(_model->index(0, first), _model->index(_model->rowCount() - 1, last)), QItemSelectionModel::ClearAndSelect);
    ui->twSchedule->setFocus();
}

void WdgSchedule::showContextMenu() {
    QMenu menu;
    Busstop *currentBusstop = _model->busstopAt(ui->twSchedule->currentIndex().row());

    if(currentBusstop) {
        QString busstopName = currentBusstop->name();
        _actionCurrentBusstopName->setText(busstopName);
        menu.addAction(_actionCurrentBusstopName);
        menu.addAction(_actionOpenBusstopSchedule);
        menu.addSeparator();
    }
    menu.addAction(_actionNew);
    menu.addAction(_actionCopy);
    menu.addAction(_actionDelete);
    menu.addAction(_actionDelete);

    menu.addSeparator();

    QMenu hourBreaksMenu(tr("Hour Break"));;
    hourBreaksMenu.setIcon(QIcon(":/icons/HourBreak.ico"));
    hourBreaksMenu.addActions(_hourBreakActions);
    menu.addMenu(&hourBreaksMenu);
    menu.exec(QCursor::pos());
}
