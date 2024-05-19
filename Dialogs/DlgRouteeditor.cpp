#include "Dialogs/DlgRouteeditor.h"
#include "ui_DlgRouteeditor.h"

#include "Dialogs/DlgTimeprofileeditor.h"

#include <QScrollBar>
#include <QMessageBox>

DlgRouteEditor::DlgRouteEditor(QWidget *parent, Route *r, const bool &createMode) :
    QDialog(parent),
    ui(new Ui::DlgRouteEditor),
    _route(*r),
    _routePtr(r),
    _currentRouteBusstop(nullptr) {
    ui->setupUi(this);

    _allBusstops = dynamic_cast<ProjectData *>(r->parent()->parent())->busstops();

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setToolTip(tr("This feature is not implemented now."));

    refreshAllBusstops();

    setCreateMode(createMode);
    setRoute(*r);

    connect(ui->pbBusstopAdd,      &QPushButton::clicked,           this, &DlgRouteEditor::actionBusstopAdd);
    connect(ui->lwAllBusstops,     &QListWidget::itemDoubleClicked, this, &DlgRouteEditor::actionBusstopAdd);
    connect(ui->pbBusstopRemove,   &QPushButton::clicked,           this, &DlgRouteEditor::actionBusstopRemove);
    connect(ui->pbBusstopUp,       &QPushButton::clicked,           this, &DlgRouteEditor::actionBusstopUp);
    connect(ui->pbBusstopDown,     &QPushButton::clicked,           this, &DlgRouteEditor::actionBusstopDown);
    connect(ui->pbBusstopsReverse, &QPushButton::clicked,           this, &DlgRouteEditor::actionBusstopsReverse);
    connect(ui->pbProfileNew,      &QPushButton::clicked,           this, &DlgRouteEditor::actionProfileNew);
    connect(ui->pbProfileEdit,     &QPushButton::clicked,           this, &DlgRouteEditor::actionProfileEdit);
    connect(ui->twProfiles,        &QTreeWidget::itemDoubleClicked, this, &DlgRouteEditor::actionProfileEdit);
    connect(ui->pbProfileDelete,   &QPushButton::clicked,           this, &DlgRouteEditor::actionProfileDelete);

    connect(ui->leBusstopsSearch,  &QLineEdit::textChanged,         this, &DlgRouteEditor::refreshAllBusstops);

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
    // set data here
    Route r = _route;
    r.setName(ui->leName->text());
    r.setCode(ui->sbCode->value());
    if(ui->cbDirections->currentIndex() == -1 || _directionsReference.isEmpty())
        r.setDirection(nullptr);
    else
        r.setDirection(_directionsReference[ui->cbDirections->currentIndex()]);

    return r;
}

void DlgRouteEditor::setRoute(const Route &route) {
    _route = route;
    Line *l = dynamic_cast<Line *>(route.parent());

    _route.setTimeProfiles(_routePtr->cloneTimeProfiles());

    for(int i = 0; i < l->directionCount(); i++) {
        LineDirection *ld = l->directionAt(i);
        _directionsReference << ld;
        ui->cbDirections->addItem(ld->description());
        if(ld == _route.direction())
            ui->cbDirections->setCurrentIndex(i);
    }

    ui->leName->setText(_route.name());
    ui->sbCode->setValue(_route.code());

    for(int i = 0; i < _route.busstopCount(); i++) {
        Busstop *b = _route.busstopAt(i);        
        ui->lwRouteBusstops->addItem(b->name());
    }

    _matchingRoutes = dynamic_cast<ProjectData *>(_routePtr->parent()->parent())->matchingRoutes(_routePtr);

    refreshProfiles();
}

void DlgRouteEditor::refreshRouteBusstops() {
    int scrollValue = ui->lwRouteBusstops->verticalScrollBar()->value();

    Busstop *busstopToSelect = _currentRouteBusstop;

    ui->lwRouteBusstops->clear();
    for(int i = 0; i < _route.busstopCount(); i++) {
        Busstop *b = _route.busstopAt(i);
        QListWidgetItem *itm = new QListWidgetItem(b->name());
        ui->lwRouteBusstops->addItem(itm);
        if(b == busstopToSelect)
            ui->lwRouteBusstops->setCurrentItem(itm);
    }

    ui->lwRouteBusstops->verticalScrollBar()->setValue(scrollValue);
}

void DlgRouteEditor::refreshAllBusstops() {
    _allBusstopsReference.clear();
    ui->lwAllBusstops->clear();
    QString filterStr = ui->leBusstopsSearch->text();

    for(int i = 0; i < _allBusstops.count(); i++) {
        Busstop *b = _allBusstops[i];

        if(!b->name().contains(filterStr, Qt::CaseInsensitive))
            continue;

        ui->lwAllBusstops->addItem(b->name());
        _allBusstopsReference << b;
    }

    ui->lwAllBusstops->sortItems(Qt::AscendingOrder);
    _allBusstopsReference = ProjectData::sortItems(_allBusstopsReference);
}

void DlgRouteEditor::refreshProfiles()
{
    _timeProfilesReference.clear();
    ui->twProfiles->clear();
    for(int i = 0; i < _route.timeProfileCount(); i++) {
        TimeProfile *p = _route.timeProfileAt(i);

        QString name = p->name();
        float duration = p->duration();

        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twProfiles);

        itm->setText(0, name);
        itm->setText(1, QString::number(duration) + " min.");

        ui->twProfiles->addTopLevelItem(itm);
        _timeProfilesReference << p;
    }
}

void DlgRouteEditor::actionBusstopAdd() {
    if(!ui->lwAllBusstops->currentItem())
        return;

    const int index = ui->lwAllBusstops->currentIndex().row();
    Busstop *b = _allBusstopsReference[index];

    QListWidgetItem *currentTargetItem = ui->lwRouteBusstops->currentItem();

    int targetIndex;
    if(!currentTargetItem)
        targetIndex = 0;
    else
        targetIndex = ui->lwRouteBusstops->currentIndex().row() + 1;

    _route.insertBusstop(targetIndex, b);

    _currentRouteBusstop = b;
    refreshRouteBusstops();
}

void DlgRouteEditor::actionBusstopRemove() {
    if(!ui->lwRouteBusstops->currentItem())
        return;

    const int removeIndex = ui->lwRouteBusstops->currentIndex().row();
    _route.removeBusstop(removeIndex);

    refreshRouteBusstops();
}

void DlgRouteEditor::actionBusstopUp() {
    moveCurrentRouteBusstop(false);
    refreshRouteBusstops();
}

void DlgRouteEditor::actionBusstopDown() {
    moveCurrentRouteBusstop(true);
    refreshRouteBusstops();
}

void DlgRouteEditor::actionBusstopsReverse() {
    QList<Busstop *> list;

    const int busstopCount = _route.busstopCount();
    for(int i = busstopCount - 1; i >= 0; i--)
        list << _route.busstopAt(i);

    _route.setBusstops(list);

    refreshRouteBusstops();
}

void DlgRouteEditor::actionProfileNew() {
    TimeProfile *p = _route.newTimeProfile();

    DlgTimeProfileEditor dlg(this, p, &_route, true);
    if(dlg.exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    *p = dlg.timeProfile();
    _route.addTimeProfile(p);
    refreshProfiles();
}

void DlgRouteEditor::actionProfileEdit() {
    if(!ui->twProfiles->currentItem())
        return;

    TimeProfile *p = _timeProfilesReference[ui->twProfiles->currentIndex().row()];

    DlgTimeProfileEditor dlg(this, p, &_route);
    if(dlg.exec() != QDialog::Accepted)
        return;

    *p = dlg.timeProfile();

    refreshProfiles();
}

void DlgRouteEditor::actionProfileDelete() {
    if(!ui->twProfiles->currentItem())
        return;

    QString id = ui->twProfiles->currentItem()->text(0);
    TimeProfile *p = _route.timeProfile(id);
    if(!p)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete profile"), tr("<p>Do you really want to delete this profile?</p><table><tr><td><b>Name:</b></td><td>%1</td></tr><tr><td><b>Duration:</b></td><td>%2 min.</td></tr></table><p><b>You cannot undo this action!</b></p>").arg(p->name(), QString::number(p->duration())), QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    _route.removeTimeProfile(p);

    refreshProfiles();
}

void DlgRouteEditor::moveCurrentRouteBusstop(bool direction) {
    if(!_currentRouteBusstop)
        return;

    const int oldIndex = _route.indexOfBusstop(_currentRouteBusstop);
    const int moveVal  = direction ? 1 : (-1);

    if(direction && oldIndex >= _route.busstopCount() - 1)
        return;

    if(!direction && oldIndex < 1)
        return;

    _route.removeBusstop(oldIndex);
    _route.insertBusstop(oldIndex + moveVal, _currentRouteBusstop);

    refreshRouteBusstops();
}

void DlgRouteEditor::on_lwRouteBusstops_currentItemChanged(QListWidgetItem *current,
                                                        QListWidgetItem *previous) {
    Q_UNUSED(current); Q_UNUSED(previous);
    _currentRouteBusstop = _route.busstopAt(ui->lwRouteBusstops->currentIndex().row());
}

