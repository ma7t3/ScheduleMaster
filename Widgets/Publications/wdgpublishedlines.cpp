#include "wdgpublishedlines.h"
#include "ui_wdgpublishedlines.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QScrollBar>

#include "App/global.h"

#include "Commands/CmdPublications.h"

WdgPublishedLines::WdgPublishedLines(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgPublishedLines),
    projectData(projectData),
    undoStack(undoStack),
    _currentLine(nullptr),
    _currentLineDirection(nullptr),
    _currentBusstop(nullptr)
{
    ui->setupUi(this);

    QObject::connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));

    QObject::connect(ui->leTitle, SIGNAL(editingFinished()), this, SLOT(actionEdit()));
    QObject::connect(ui->leFooter, SIGNAL(editingFinished()), this, SLOT(actionEdit()));

    QObject::connect(ui->pbDirectionNew, SIGNAL(clicked()), this, SLOT(actionDirectionNew()));
    QObject::connect(ui->pbDirectionDelete, SIGNAL(clicked()), this, SLOT(actionDirectionDelete()));

    QObject::connect(ui->leDirectionsName, SIGNAL(editingFinished()), this, SLOT(actionDirectionEdit()));

    QObject::connect(ui->twRoutes, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(refreshRouteCheckBoxRelations(QTreeWidgetItem*)));
    QObject::connect(ui->twRoutes, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(actionRoutesChange()));

    QObject::connect(ui->cmbAllBusstops, SIGNAL(activated(int)), this, SLOT(actionBusstopAdd()));
    QObject::connect(ui->pbBusstopsAddAll, SIGNAL(clicked()), this, SLOT(actionBusstopsAddAll()));

    QObject::connect(ui->pbBusstopsRemove, SIGNAL(clicked()), this, SLOT(actionBusstopRemove()));
    QObject::connect(ui->pbBusstopsRemoveAll, SIGNAL(clicked()), this, SLOT(actionBusstopRemoveAll()));

    QObject::connect(ui->pbBusstopsUp, SIGNAL(clicked()), this, SLOT(actionBusstopUp()));
    QObject::connect(ui->pbBusstopsDown, SIGNAL(clicked()), this, SLOT(actionBusstopDown()));

    QObject::connect(ui->leBusstopsLabel, SIGNAL(textEdited(QString)), this, SLOT(actionEditBusstop()));
    QObject::connect(ui->cbBusstopsShowDivider, SIGNAL(clicked()), this, SLOT(actionEditBusstop()));

    refreshRouteList();
}

WdgPublishedLines::~WdgPublishedLines()
{
    delete ui;
}

PublishedLine *WdgPublishedLines::currentLine() const {
    return _currentLine;
}

void WdgPublishedLines::actionNew() {
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New line"), tr("Enter line name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLine *l = new PublishedLine(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineNew(projectData, l));
    _currentLine = l;
    refreshLineList();
    refreshCurrentLine();
}

void WdgPublishedLines::actionEdit() {
    if(!_currentLine)
        return;

    PublishedLine newL = *_currentLine;

    newL.setTitle(ui->leTitle->text());
    newL.setFooter(ui->leFooter->text());
    newL.setFilePath(ui->leFilePath->text());

    undoStack->push(new cmdPublishedLineEdit(_currentLine, newL));
    refreshLineList();
}

void WdgPublishedLines::actionDelete() {
    if(!_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete published line"), tr("<p><b>Do you really want to delete this published line?</b></p><p>%1</p>").arg(_currentLine->title()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdPublishedLineDelete(projectData, _currentLine));
    refreshLineList();
}

void WdgPublishedLines::actionDirectionNew() {
    if(!_currentLine)
        return;

    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New direction"), tr("Enter direction name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLineDirection *ld = new PublishedLineDirection(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineDirectionNew(_currentLine, ld));
    _currentLineDirection = ld;
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionEdit() {
    if(!_currentLineDirection)
        return;

    PublishedLineDirection newLd = *_currentLineDirection;

    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionDelete() {

}

void WdgPublishedLines::actionRoutesChange() {
    if(!_currentLine || !_currentLineDirection)
        return;

    QList<Route *> routes;

    for(int i = 0; i < ui->twRoutes->topLevelItemCount(); i++) {
        for(int j = 0; j < ui->twRoutes->topLevelItem(i)->childCount(); j++) {
            for(int k = 0; k < ui->twRoutes->topLevelItem(i)->child(j)->childCount(); k++) {
                Qt::CheckState checked = ui->twRoutes->topLevelItem(i)->child(j)->child(k)->checkState(0);
                if(checked == Qt::Checked)
                    routes << _routesReference[i][j][k];
            }
        }
    }

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setRoutes(routes);

    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));
}

void WdgPublishedLines::actionBusstopAdd() {
    if(!_currentLineDirection)
        return;

    int busstopIndex = ui->cmbAllBusstops->currentIndex();

    Busstop *b = _allBusstopsReference[busstopIndex];
    if(!b)
        return;

    bool busstopFound = false;
    for(int i = 0; i < _currentLineDirection->busstopCount(); i++) {
        if(_currentLineDirection->busstopAt(i)->linkedBusstop() == b)
            busstopFound = true;
    }
    if(busstopFound)
        return;

    PublishedBusstop *pb = new PublishedBusstop(global::getNewID(), b, "");

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.addBusstop(pb);

    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopsAddAll() {
    if(!_currentLineDirection)
        return;

    for(int i = 0; i < _allBusstopsReference.count(); i++) {
        ui->cmbAllBusstops->setCurrentIndex(i);
        actionBusstopAdd();
        qApp->processEvents();
    }
}

void WdgPublishedLines::actionBusstopRemove() {
    if(!_currentLineDirection)
        return;

    if(!ui->lwBusstops->currentItem())
        return;

    PublishedBusstop *b = _currentLineDirection->busstopAt(ui->lwBusstops->currentRow());
    if(!b)
        return;

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.removeBusstop(b);

    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopRemoveAll() {
    if(!_currentLineDirection)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "Remove all busstops", "Do you really want to remove all busstops?", QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setBusstops({});
    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopUp() {
    if(!_currentLineDirection)
        return;

    QList<PublishedBusstop *> busstops = _currentLineDirection->busstops();
    int index = ui->lwBusstops->currentRow();

    if(index == 0)
        return;

    PublishedBusstop *b = busstops[index];
    busstops.remove(index);
    busstops.insert(index - 1, b);

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setBusstops(busstops);
    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopDown() {
    if(!_currentLineDirection)
        return;

    QList<PublishedBusstop *> busstops = _currentLineDirection->busstops();
    int index = ui->lwBusstops->currentRow();

    if(index == ui->lwBusstops->count() - 1)
        return;

    PublishedBusstop *b = busstops[index];
    busstops.remove(index);
    busstops.insert(index + 1, b);

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setBusstops(busstops);
    undoStack->push(new cmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionEditBusstop() {
    if(!_currentLineDirection || !_currentBusstop)
        return;

    PublishedBusstop newB = *_currentBusstop;
    newB.setLabel(ui->leBusstopsLabel->text());
    newB.setShowDivider(ui->cbBusstopsShowDivider->isChecked());

    undoStack->push(new cmdPublishedBusstopEdit(_currentBusstop, newB));

    refreshBusstopList();
}

void WdgPublishedLines::setCurrentLine(PublishedLine *newCurrentLine) {
    _currentLine = newCurrentLine;
}

void WdgPublishedLines::refreshLineList() {
    refreshing = true;
    ui->lwLines->clear();
    _listReference.clear();

    QList<PublishedLine *> publishedLines = projectData->publications()->lines();

    for(int i = 0; i < publishedLines.count(); i++) {
        PublishedLine *l = publishedLines[i];
        _listReference << l;
        ui->lwLines->addItem(l->title());

        if(_currentLine == l)
            ui->lwLines->setCurrentRow(i);
    }

    refreshing = false;
}

void WdgPublishedLines::refreshCurrentLine() {
    ui->lwDirections->clear();
    _directionsListReference.clear();

    if(!_currentLine)
        return;

    refreshingCurrentLine = true;

    ui->leTitle->setText(_currentLine->title());
    ui->leFooter->setText(_currentLine->footer());
    ui->leFilePath->setText(_currentLine->filePath());

    QList<PublishedLineDirection *> directions = _currentLine->directions();
    for(int i = 0; i < directions.count(); i++) {
        PublishedLineDirection *ld = directions[i];
        ui->lwDirections->addItem(ld->name());
        _directionsListReference << ld;
        if(_currentLineDirection == ld)
            ui->lwDirections->setCurrentRow(i);
    }

    for(int i = 0; i < _dayTypesReference.count(); i++) {
        if(_currentLine->hasDayType(_dayTypesReference[i]))
            ui->lwDayTypes->item(i)->setCheckState(Qt::Checked);
        else
            ui->lwDayTypes->item(i)->setCheckState(Qt::Unchecked);
    }

    refreshCurrentLineDirection();
    refreshingCurrentLine = false;
}

void WdgPublishedLines::refreshCurrentLineDirection() {

    // clear lists here before

    ui->leDirectionsName->clear();

    if(!_currentLine || !_currentLineDirection)
        return;

    ui->leDirectionsName->setText(_currentLineDirection->name());

    refreshRouteCheckBoxes();

    refreshAllBusstops();

    refreshBusstopList();
}

void WdgPublishedLines::refreshDayTypes() {
    ui->lwDayTypes->clear();

    ProjectSettings *s = projectData->projectSettings();

    for(int i = 0; i < s->dayTypeCount(); i++) {
        DayType *dt = s->dayTypeAt(i);
        _dayTypesReference << dt;

        QListWidgetItem *itm = new QListWidgetItem(dt->name());
        itm->setCheckState(Qt::Unchecked);
        ui->lwDayTypes->addItem(itm);
    }
}


void WdgPublishedLines::refreshRouteList() {
    ui->twRoutes->clear();

    _routesReference.clear();
    _routesDirectionsReference.clear();
    _routesLinesReference.clear();

    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        _routesLinesReference << l;
        QTreeWidgetItem *lineItm = new QTreeWidgetItem(ui->twRoutes, {l->name()});
        lineItm->setCheckState(0, Qt::Unchecked);

        QList<LineDirection *> directions = l->directions();
        QList<QList<Route *>> routesParents;

        for(int j = 0; j < directions.count(); j++) {
            LineDirection *ld = directions[j];
            QTreeWidgetItem *directionItm = new QTreeWidgetItem(lineItm, {tr("to ") + ld->description()});
            directionItm->setCheckState(0, Qt::Unchecked);

            QList<Route *> routes = l->routesToDirection(ld);
            routesParents << routes;

            for(int k = 0; k < routes.count(); k++) {
                Route *r = routes[k];
                QTreeWidgetItem *routeItm = new QTreeWidgetItem(directionItm, {r->name()});
                routeItm->setCheckState(0, Qt::Unchecked);
            }
        }

        _routesLinesReference << l;
        _routesDirectionsReference << directions;
        _routesReference << routesParents;
    }
}

void WdgPublishedLines::refreshRouteCheckBoxes() {
    if(!_currentLine || !_currentLineDirection)
        return;

    for(int i = 0; i < ui->twRoutes->topLevelItemCount(); i++) {
        for(int j = 0; j < ui->twRoutes->topLevelItem(i)->childCount(); j++) {
            for(int k = 0; k < ui->twRoutes->topLevelItem(i)->child(j)->childCount(); k++) {
                if(_currentLineDirection->hasRoute(_routesReference[i][j][k]))
                    ui->twRoutes->topLevelItem(i)->child(j)->child(k)->setCheckState(0, Qt::Checked);
                else
                    ui->twRoutes->topLevelItem(i)->child(j)->child(k)->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void WdgPublishedLines::refreshRouteCheckBoxRelations(QTreeWidgetItem *changedItm) {

    if(refreshingRouteCheckBoxes)
        return;

    refreshingRouteCheckBoxes = true;

    // for each line
    for(int i = 0; i < ui->twRoutes->topLevelItemCount(); i++) {
        QTreeWidgetItem *lineItm = ui->twRoutes->topLevelItem(i);
        bool lineChanged = lineItm == changedItm;

        bool checkedDirectionFound = false;
        bool partiallyCheckedDirectionFound = false;
        bool uncheckedDirectionFound = false;

        // for each direction
        for(int j = 0; j < lineItm->childCount(); j++) {
            QTreeWidgetItem *directionItm = lineItm->child(j);
            bool directionChanged = directionItm == changedItm;

            if(lineChanged)
                directionItm->setCheckState(0, lineItm->checkState(0));

            bool checkedRouteFound = false;
            bool uncheckedRouteFound = false;

            // for each route
            for(int k = 0; k < directionItm->childCount(); k++) {
                QTreeWidgetItem *routeItm = directionItm->child(k);
                if(lineChanged)
                    routeItm->setCheckState(0, lineItm->checkState(0));
                else if(directionChanged)
                    routeItm->setCheckState(0, directionItm->checkState(0));
                else {
                    if(routeItm->checkState(0) == Qt::Checked)
                        checkedRouteFound = true;
                    if(routeItm->checkState(0) == Qt::Unchecked)
                        uncheckedRouteFound = true;
                }
            }

            if(!directionChanged && !lineChanged) {
                if(checkedRouteFound && uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::PartiallyChecked);
                else if(checkedRouteFound && !uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::Checked);
                else if(!checkedRouteFound && uncheckedRouteFound)
                    directionItm->setCheckState(0, Qt::Unchecked);
            }

            if(!lineChanged) {
                if(directionItm->checkState(0) == Qt::Checked)
                    checkedDirectionFound = true;
                if(directionItm->checkState(0) == Qt::PartiallyChecked)
                    partiallyCheckedDirectionFound = true;
                if(directionItm->checkState(0) == Qt::Unchecked)
                    uncheckedDirectionFound = true;
            }
        }

        if(!lineChanged) {
            if((checkedDirectionFound && uncheckedDirectionFound) || partiallyCheckedDirectionFound)
                lineItm->setCheckState(0, Qt::PartiallyChecked);
            else if(checkedDirectionFound && !uncheckedDirectionFound)
                lineItm->setCheckState(0, Qt::Checked);
            else if(!checkedDirectionFound && uncheckedDirectionFound)
                lineItm->setCheckState(0, Qt::Unchecked);
        }
    }

    refreshingRouteCheckBoxes = false;
}

void WdgPublishedLines::refreshAllBusstops() {
    ui->cmbAllBusstops->clear();

    QList<Busstop *> allBusstops = projectData->combinedRoutes(_currentLineDirection->routes());
    for(int i = 0; i < allBusstops.count(); i++) {
        ui->cmbAllBusstops->addItem(allBusstops[i]->name());
    }

    _allBusstopsReference = allBusstops;
}

void WdgPublishedLines::refreshBusstopList() {
    if(!_currentLineDirection)
        return;

    int scrollValue = ui->lwBusstops->verticalScrollBar()->value();

    ui->lwBusstops->clear();

    for(int i = 0; i < _currentLineDirection->busstopCount(); i++) {
        PublishedBusstop *b = _currentLineDirection->busstopAt(i);

        QString text = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        if(b->showDivider())
            text += "\r\n--------------------------------------------------------------------------------";
        ui->lwBusstops->addItem(text);

        if(b == _currentBusstop)
            ui->lwBusstops->setCurrentRow(i);
    }

    ui->lwBusstops->verticalScrollBar()->setValue(scrollValue);
}

void WdgPublishedLines::on_lwLines_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshing)
        return;

    if(!current) {
        _currentLine = nullptr;
        _currentLineDirection = nullptr;
        return;
    }

    _currentLine = _listReference[ui->lwLines->currentRow()];

    if(_currentLine->directionCount() == 0) {
        _currentLineDirection = nullptr;
    } else {
        _currentLineDirection = _currentLine->directionAt(0);
    }

    emit currentLineChanged(_currentLine);
    refreshCurrentLine();
}

void WdgPublishedLines::on_lwDirections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshingCurrentLine)
        return;

    if(!current)
        _currentLineDirection = nullptr;
    else
        _currentLineDirection = _directionsListReference[ui->lwDirections->currentRow()];

    //emit currentLineChanged(_currentLineDirection);
    refreshCurrentLineDirection();
}


void WdgPublishedLines::on_pbFilePathBrowse_clicked() {
    if(!_currentLine)
        return;

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Publications/";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString fileName = QFileDialog::getSaveFileName(this, "", dir.path(), tr("Portable document format (*.pdf)"));
    if(fileName == "")
        return;

    ui->leFilePath->setText(fileName);
    actionEdit();
}




void WdgPublishedLines::on_lwBusstops_itemClicked(QListWidgetItem *item) {
    Q_UNUSED(item);

    if(!_currentLineDirection)
        return;

    _currentBusstop = _currentLineDirection->busstopAt(ui->lwBusstops->currentRow());
    ui->leBusstopsLabel->setPlaceholderText(_currentBusstop->linkedBusstop()->name());
    QString label = _currentBusstop->label();
    ui->leBusstopsLabel->setText(label.isEmpty() ? _currentBusstop->linkedBusstop()->name() : label);
    ui->cbBusstopsShowDivider->setChecked(_currentBusstop->showDivider());
}


void WdgPublishedLines::on_pbBusstopSearchAndReplace_clicked() {
    if(!_currentLineDirection)
        return;

    bool ok1, ok2;
    QString search = QInputDialog::getText(this, "Search and replace", "Enter search string:", QLineEdit::Normal, "", &ok1);
    QString replace = QInputDialog::getText(this, "Search and replace", "Enter replace string:", QLineEdit::Normal, "", &ok2);

    if(!ok1 || !ok2)
        return;

    for(int i = 0; i < _currentLineDirection->busstopCount(); i++) {
        PublishedBusstop *b = _currentLineDirection->busstopAt(i);
        QString input = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        QString result = input;
        result.replace(search, replace, Qt::CaseInsensitive);
        if(result != input)
            b->setLabel(result);
    }

    refreshBusstopList();
}


void WdgPublishedLines::on_lwDayTypes_itemChanged(QListWidgetItem *item) {
    if(!item || !_currentLine)
        return;

    if(refreshingCurrentLine)
        return;

    int index = ui->lwDayTypes->row(item);

    Qt::CheckState checkState = item->checkState();
    if(checkState == Qt::Checked)
        _currentLine->addDayType(_dayTypesReference[index]);
    else
        _currentLine->addDayType(_dayTypesReference[index]);
}

