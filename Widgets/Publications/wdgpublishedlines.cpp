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

    //QObject::connect(ui->twRoutes, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(refreshRouteCheckBoxRelations(QTreeWidgetItem*)));
    QObject::connect(ui->routeSelector, SIGNAL(routesChanged(QList<Route*>)), this, SLOT(actionRoutesChange()));

    QObject::connect(ui->cmbAllBusstops, SIGNAL(activated(int)), this, SLOT(actionBusstopAdd()));
    QObject::connect(ui->pbBusstopsAddAll, SIGNAL(clicked()), this, SLOT(actionBusstopsAddAll()));

    QObject::connect(ui->pbBusstopsRemove, SIGNAL(clicked()), this, SLOT(actionBusstopRemove()));
    QObject::connect(ui->pbBusstopsRemoveAll, SIGNAL(clicked()), this, SLOT(actionBusstopRemoveAll()));

    QObject::connect(ui->pbBusstopsUp, SIGNAL(clicked()), this, SLOT(actionBusstopUp()));
    QObject::connect(ui->pbBusstopsDown, SIGNAL(clicked()), this, SLOT(actionBusstopDown()));

    QObject::connect(ui->leBusstopsLabel, SIGNAL(textEdited(QString)), this, SLOT(actionEditBusstop()));
    QObject::connect(ui->cbBusstopsShowDivider, SIGNAL(clicked()), this, SLOT(actionEditBusstop()));

    ui->routeSelector->setProjectData(projectData);
    ui->routeSelector->refresh();
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

    PublishedLine *l = projectData->publications()->newLine();
    l->setTitle(name);
    undoStack->push(new CmdPublishedLineNew(projectData, l));
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

    undoStack->push(new CmdPublishedLineEdit(_currentLine, newL));
    refreshLineList();
}

void WdgPublishedLines::actionDelete() {
    if(!_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete published line"), tr("<p><b>Do you really want to delete this published line?</b></p><p>%1</p>").arg(_currentLine->title()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new CmdPublishedLineDelete(projectData, _currentLine));
    refreshLineList();
}

void WdgPublishedLines::actionDirectionNew() {
    if(!_currentLine)
        return;

    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New direction"), tr("Enter direction name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLineDirection *ld = _currentLine->newDirection();
    ld->setName(name);
    undoStack->push(new CmdPublishedLineDirectionNew(_currentLine, ld));
    _currentLineDirection = ld;
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionEdit() {
    if(!_currentLineDirection)
        return;

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setName(ui->leDirectionsName->text());

    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionDelete() {

}

void WdgPublishedLines::actionRoutesChange() {
    if(!_currentLine || !_currentLineDirection)
        return;

    PublishedLineDirection newLd = *_currentLineDirection;
    newLd.setRoutes(ui->routeSelector->routes());

    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));
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

    PublishedBusstop *pb = new PublishedBusstop(nullptr, global::getNewID(), b, "");

    PublishedLineDirection newLd = *_currentLineDirection;

    int insertIndex;
    if(ui->lwBusstops->currentRow() != -1)
        insertIndex = ui->lwBusstops->currentRow() + 1;
    else
        insertIndex = ui->lwBusstops->count();

    newLd.addBusstopAt(insertIndex, pb);

    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

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

    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

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
    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

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
    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

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
    undoStack->push(new CmdPublishedLineDirectionEdit(_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionEditBusstop() {
    if(!_currentLineDirection || !_currentBusstop)
        return;

    PublishedBusstop newB = *_currentBusstop;
    newB.setLabel(ui->leBusstopsLabel->text());
    newB.setShowDivider(ui->cbBusstopsShowDivider->isChecked());

    undoStack->push(new CmdPublishedBusstopEdit(_currentBusstop, newB));

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
    publishedLines = ProjectData::sortItems(publishedLines);

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
        if(i == 0) {
            ui->lwDirections->setCurrentRow(0);
            _currentLineDirection = _currentLine->directionAt(0);
        }
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

    if(_currentLine && _currentLineDirection) {
        ui->leDirectionsName->setText(_currentLineDirection->name());
        ui->routeSelector->setSelectedRoutes(_currentLineDirection->routes());
    }

    refreshAllBusstops();
    refreshBusstopList();
}

void WdgPublishedLines::refreshDayTypes() {
    qDebug() << "refreshing day types in published lines...";

    ui->lwDayTypes->clear();
    _dayTypesReference.clear();

    ProjectSettings *s = projectData->projectSettings();

    for(int i = 0; i < s->dayTypeCount(); i++) {
        DayType *dt = s->dayTypeAt(i);
        _dayTypesReference << dt;

        QListWidgetItem *itm = new QListWidgetItem(dt->name());
        itm->setCheckState(Qt::Unchecked);
        ui->lwDayTypes->addItem(itm);
    }
}


void WdgPublishedLines::refreshRoutes() {
    qDebug() << "refreshing route list in published lines...";
    ui->routeSelector->refresh();
}

void WdgPublishedLines::refreshAllBusstops() {
    ui->cmbAllBusstops->clear();

    if(!_currentLine || !_currentLineDirection)
        return;

    QList<Busstop *> allBusstops = projectData->combinedRoutes(_currentLineDirection->routes());
    for(int i = 0; i < allBusstops.count(); i++) {
        ui->cmbAllBusstops->addItem(allBusstops[i]->name());
    }

    _allBusstopsReference = allBusstops;
}

void WdgPublishedLines::refreshBusstopList() {
    ui->lwBusstops->clear();

    if(!_currentLine || !_currentLineDirection)
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

    QList<QPair<PublishedBusstop *, PublishedBusstop>> affectedItems;

    for(int i = 0; i < _currentLineDirection->busstopCount(); i++) {
        PublishedBusstop *b = _currentLineDirection->busstopAt(i);
        QString input = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        QString result = input;
        result.replace(search, replace, Qt::CaseInsensitive);
        if(result != input) {
            PublishedBusstop newB = *b;
            newB.setLabel(result);
            affectedItems << QPair<PublishedBusstop *, PublishedBusstop>(b, newB);
        }
    }

    undoStack->push(new CmdPublishedBusstopsEdit(affectedItems));

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

