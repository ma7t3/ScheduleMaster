#include "Dialogs/DlgRouteeditor.h"
#include "ui_DlgRouteeditor.h"

#include "Dialogs/DlgTimeprofileeditor.h"

#include <QAbstractItemModel>

#include <QScrollBar>
#include <QMessageBox>

DlgRouteEditor::DlgRouteEditor(QWidget *parent, Route *r, const bool &createMode) :
    QDialog(parent),
    ui(new Ui::DlgRouteEditor),
    _allBusstopsModel(new SimpleBusstopListModel(this)),
    _routeBusstopsModel(new SimpleCustomBusstopListModel(this)),
    _route(*r),
    _routePtr(r) {
    ui->setupUi(this);

    _allBusstopsModel->setProjectData(dynamic_cast<ProjectData *>(r->parent()->parent()));

    ui->lwAllBusstops->setModel(_allBusstopsModel);
    ui->lwRouteBusstops->setModel(_routeBusstopsModel);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setToolTip(tr("This feature is not implemented now."));

    setCreateMode(createMode);
    setRoute(*r);

    connect(ui->pbBusstopAdd,      &QPushButton::clicked,           this,              &DlgRouteEditor::actionBusstopAdd);
    connect(ui->lwAllBusstops,     &QListWidget::doubleClicked,     this,              &DlgRouteEditor::actionBusstopAdd);
    connect(ui->pbBusstopRemove,   &QPushButton::clicked,           this,              &DlgRouteEditor::actionBusstopRemove);
    connect(ui->pbBusstopUp,       &QPushButton::clicked,           this,              &DlgRouteEditor::actionBusstopUp);
    connect(ui->pbBusstopDown,     &QPushButton::clicked,           this,              &DlgRouteEditor::actionBusstopDown);
    connect(ui->pbBusstopsReverse, &QPushButton::clicked,           this,              &DlgRouteEditor::actionBusstopsReverse);
    connect(ui->pbProfileNew,      &QPushButton::clicked,           this,              &DlgRouteEditor::actionProfileNew);
    connect(ui->pbProfileEdit,     &QPushButton::clicked,           this,              &DlgRouteEditor::actionProfileEdit);
    connect(ui->twProfiles,        &QTreeWidget::itemDoubleClicked, this,              &DlgRouteEditor::actionProfileEdit);
    connect(ui->pbProfileDelete,   &QPushButton::clicked,           this,              &DlgRouteEditor::actionProfileDelete);
    connect(ui->leBusstopsSearch,  &QLineEdit::textChanged,         _allBusstopsModel, &SimpleBusstopListModel::setSearchFilter);

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

    for(int i = 0; i < _route.busstopCount(); i++)
        _routeBusstopsModel->addBusstop(_route.busstopAt(i));

    _matchingRoutes = dynamic_cast<ProjectData *>(_routePtr->parent()->parent())->matchingRoutes(_routePtr);

    refreshProfiles();
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
    if(!ui->lwAllBusstops->currentIndex().isValid())
        return;

    const int index = ui->lwAllBusstops->currentIndex().row();
    Busstop *b = _allBusstopsModel->itemAt(index);

    QModelIndex targetModelIndex = ui->lwRouteBusstops->currentIndex();

    int targetIndex;
    if(!targetModelIndex.isValid())
        targetIndex = 0;
    else
        targetIndex = ui->lwRouteBusstops->currentIndex().row() + 1;

    _route.insertBusstop(targetIndex, b);
    _routeBusstopsModel->addBusstop(b, targetIndex);
    ui->lwRouteBusstops->setCurrentIndex(_routeBusstopsModel->index(targetIndex, 0, QModelIndex()));
}

void DlgRouteEditor::actionBusstopRemove() {
    if(!ui->lwRouteBusstops->currentIndex().isValid())
        return;

    const int removeIndex = ui->lwRouteBusstops->currentIndex().row();
    _route.removeBusstop(removeIndex);
    _routeBusstopsModel->removeBusstop(removeIndex);
    ui->lwRouteBusstops->setCurrentIndex(QModelIndex());
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

    _routeBusstopsModel->setBusstops(list);
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
    if(!ui->lwRouteBusstops->currentIndex().isValid())
        return;

    Busstop *b = _routeBusstopsModel->itemAt(ui->lwRouteBusstops->currentIndex());

    const int oldIndex = _route.indexOfBusstop(b);
    const int moveVal  = direction ? 1 : (-1);

    if(direction && oldIndex >= _route.busstopCount() - 1)
        return;

    if(!direction && oldIndex < 1)
        return;

    _route.removeBusstop(oldIndex);
    _route.insertBusstop(oldIndex + moveVal, b);

    _routeBusstopsModel->removeBusstop(oldIndex);
    _routeBusstopsModel->addBusstop(b, oldIndex + moveVal);
    ui->lwRouteBusstops->setCurrentIndex(_routeBusstopsModel->index(oldIndex + moveVal, 0, QModelIndex()));
}
