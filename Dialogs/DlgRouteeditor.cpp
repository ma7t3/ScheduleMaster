#include "Dialogs/DlgRouteeditor.h"
#include "ui_DlgRouteeditor.h"

#include "Dialogs/DlgTimeprofileeditor.h"

#include <QAbstractItemModel>

#include <QScrollBar>
#include <QMessageBox>

DlgRouteEditor::DlgRouteEditor(QWidget *parent, Route *r, const bool &createMode) :
    QDialog(parent),
    ui(new Ui::DlgRouteEditor),
    _directionsModel(new LineDirectionListModel(this)),
    _allBusstopsModel(new SimpleBusstopListModel(this)),
    _routeBusstopsModel(new SimpleRouteBusstopListModel(this)),
    _timeProfilesModel(new TimeProfileTableModel(this)),
    _allBusstopsProxyModel(new QSortFilterProxyModel(this)),
    _route(*r),
    _routePtr(r) {
    ui->setupUi(this);

    _allBusstopsModel->setProjectData(dynamic_cast<ProjectData *>(r->parent()->parent()));
    _routeBusstopsModel->setRoute(&_route);
    _timeProfilesModel->setRoute(&_route);

    _allBusstopsProxyModel->setSourceModel(_allBusstopsModel);
    _allBusstopsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _allBusstopsProxyModel->setSortLocaleAware(true);
    _allBusstopsProxyModel->sort(0, Qt::AscendingOrder);

    ui->lwAllBusstops->setModel(_allBusstopsProxyModel);
    ui->lwRouteBusstops->setModel(_routeBusstopsModel);
    ui->twProfiles->setModel(_timeProfilesModel);

    connect(_routeBusstopsModel, &QAbstractItemModel::rowsInserted, this, &DlgRouteEditor::onRouteBusstopsRowsInserted);
    connect(_timeProfilesModel, &QAbstractItemModel::rowsInserted, this, &DlgRouteEditor::onTimeProfilesRowsInserted);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setToolTip(tr("This feature is not implemented now."));

    ui->cbDirections->setModel(_directionsModel);

    setCreateMode(createMode);
    setRoute(*r);


    connect(ui->pbBusstopAdd,      &QPushButton::clicked,       this,                   &DlgRouteEditor::actionBusstopAdd);
    connect(ui->lwAllBusstops,     &QListView::doubleClicked,   this,                   &DlgRouteEditor::actionBusstopAdd);
    connect(ui->pbBusstopRemove,   &QPushButton::clicked,       this,                   &DlgRouteEditor::actionBusstopRemove);
    connect(ui->pbBusstopUp,       &QPushButton::clicked,       this,                   &DlgRouteEditor::actionBusstopUp);
    connect(ui->pbBusstopDown,     &QPushButton::clicked,       this,                   &DlgRouteEditor::actionBusstopDown);
    connect(ui->pbBusstopsReverse, &QPushButton::clicked,       this,                   &DlgRouteEditor::actionBusstopsReverse);
    connect(ui->pbProfileNew,      &QPushButton::clicked,       this,                   &DlgRouteEditor::actionProfileNew);
    connect(ui->pbProfileEdit,     &QPushButton::clicked,       this,                   &DlgRouteEditor::actionProfileEdit);
    connect(ui->twProfiles,        &QTreeView::doubleClicked,   this,                   &DlgRouteEditor::actionProfileEdit);
    connect(ui->pbProfileDelete,   &QPushButton::clicked,       this,                   &DlgRouteEditor::actionProfileDelete);
    connect(ui->leBusstopsSearch,  &QLineEdit::textChanged,     _allBusstopsProxyModel, &QSortFilterProxyModel::setFilterFixedString);

    ui->leName->setFocus();
}

DlgRouteEditor::~DlgRouteEditor()
{
    delete ui;
}

void DlgRouteEditor::setCreateMode(const bool &newCreateMode) {
    if(newCreateMode)
        setWindowTitle(tr("Create route"));
}

Route DlgRouteEditor::route() const {
    Route r = _route;
    r.setName(ui->leName->text());
    r.setCode(ui->sbCode->value());
    if(_directionsModel->itemCount() > 0)
        r.setDirection(_directionsModel->itemAt(ui->cbDirections->currentIndex()));
    else
        r.setDirection(nullptr);

    return r;
}

void DlgRouteEditor::setRoute(const Route &route) {
    _route = route;
    Line *l = dynamic_cast<Line *>(route.parent());

    _route.setTimeProfiles(_routePtr->cloneTimeProfiles());

    _directionsModel->setLine(l);

    ui->cbDirections->setCurrentIndex(_directionsModel->indexOf(route.direction()));

    ui->leName->setText(_route.name());
    ui->sbCode->setValue(_route.code());

    _timeProfilesModel->setRoute(&_route);
}

void DlgRouteEditor::actionBusstopAdd() {
    if(!ui->lwAllBusstops->currentIndex().isValid())
        return;

    const QModelIndex index = ui->lwAllBusstops->currentIndex();
    Busstop *b = _allBusstopsModel->itemAt(_allBusstopsProxyModel->mapToSource(index).row());

    QModelIndex targetModelIndex = ui->lwRouteBusstops->currentIndex();

    int targetIndex;
    if(!targetModelIndex.isValid())
        targetIndex = 0;
    else
        targetIndex = ui->lwRouteBusstops->currentIndex().row() + 1;

    _route.insertBusstop(targetIndex, b);
}

void DlgRouteEditor::actionBusstopRemove() {
    if(!ui->lwRouteBusstops->currentIndex().isValid())
        return;

    const int removeIndex = ui->lwRouteBusstops->currentIndex().row();
    _route.removeBusstop(removeIndex);
}

void DlgRouteEditor::actionBusstopUp() {
    moveCurrentRouteBusstop(false);
}

void DlgRouteEditor::actionBusstopDown() {
    moveCurrentRouteBusstop(true);
}

void DlgRouteEditor::actionBusstopsReverse() {
    QList<Busstop *> list;

    const int busstopCount = _route.busstopCount();
    for(int i = busstopCount - 1; i >= 0; i--)
        list << _route.busstopAt(i);

    _route.setBusstops(list);
}

void DlgRouteEditor::actionProfileNew() {
    TimeProfile *p = _routePtr->newTimeProfile();

    DlgTimeProfileEditor dlg(this, p, &_route, true);
    if(dlg.exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    *p = dlg.timeProfile();
    _route.addTimeProfile(p);
}

void DlgRouteEditor::actionProfileEdit() {
    if(!ui->twProfiles->currentIndex().isValid())
        return;

    TimeProfile *p = _timeProfilesModel->itemAt(ui->twProfiles->currentIndex().row());

    DlgTimeProfileEditor dlg(this, p, &_route);
    if(dlg.exec() != QDialog::Accepted)
        return;

    *p = dlg.timeProfile();
}

void DlgRouteEditor::actionProfileDelete() {
    if(!ui->twProfiles->currentIndex().isValid())
        return;

    TimeProfile *p = _timeProfilesModel->itemAt(ui->twProfiles->currentIndex().row());

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete profile"), tr("<p>Do you really want to delete this profile?</p><table><tr><td><b>Name:</b></td><td>%1</td></tr><tr><td><b>Duration:</b></td><td>%2 min.</td></tr></table><p><b>You cannot undo this action!</b></p>").arg(p->name(), QString::number(p->duration())), QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    _route.removeTimeProfile(p);

}

void DlgRouteEditor::moveCurrentRouteBusstop(bool direction) {
    if(!ui->lwRouteBusstops->currentIndex().isValid())
        return;

    Busstop *b = _routeBusstopsModel->itemAt(ui->lwRouteBusstops->currentIndex().row());

    const int oldIndex = _route.indexOfBusstop(b);
    const int moveVal  = direction ? 1 : (-1);

    if(direction && oldIndex >= _route.busstopCount() - 1)
        return;

    if(!direction && oldIndex < 1)
        return;

    _route.removeBusstop(oldIndex);
    qApp->processEvents();
    _route.insertBusstop(oldIndex + moveVal, b);
}

void DlgRouteEditor::onRouteBusstopsRowsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);Q_UNUSED(last);
    ui->lwRouteBusstops->setCurrentIndex(_routeBusstopsModel->index(first, 0));
    ui->lwRouteBusstops->selectionModel()->select(_routeBusstopsModel->index(first, 0), QItemSelectionModel::ClearAndSelect);
    ui->lwRouteBusstops->setFocus();
}

void DlgRouteEditor::onTimeProfilesRowsInserted(QModelIndex parent, int first, int last) {
    Q_UNUSED(parent);Q_UNUSED(last);
    ui->twProfiles->setCurrentIndex(_timeProfilesModel->index(first, 0));
    ui->twProfiles->selectionModel()->select(QItemSelection(_timeProfilesModel->index(first, 0), _timeProfilesModel->index(first, 1)), QItemSelectionModel::ClearAndSelect);
    ui->twProfiles->setFocus();
}
