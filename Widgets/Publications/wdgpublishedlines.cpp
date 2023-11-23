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
    m_currentLine(nullptr),
    m_currentLineDirection(nullptr),
    m_currentBusstop(nullptr)
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
    return m_currentLine;
}

void WdgPublishedLines::actionNew() {
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New line"), tr("Enter line name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLine *l = new PublishedLine(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineNew(projectData, l));
    m_currentLine = l;
    refreshLineList();
    refreshCurrentLine();
}

void WdgPublishedLines::actionEdit() {
    if(!m_currentLine)
        return;

    PublishedLine newL = *m_currentLine;

    newL.setTitle(ui->leTitle->text());
    newL.setFooter(ui->leFooter->text());
    newL.setFilePath(ui->leFilePath->text());

    undoStack->push(new cmdPublishedLineEdit(m_currentLine, newL));
    refreshLineList();
}

void WdgPublishedLines::actionDelete() {
    if(!m_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete published line"), tr("<p><b>Do you really want to delete this published line?</b></p><p>%1</p>").arg(m_currentLine->title()), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdPublishedLineDelete(projectData, m_currentLine));
    refreshLineList();
}

void WdgPublishedLines::actionDirectionNew() {
    if(!m_currentLine)
        return;

    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New direction"), tr("Enter direction name:"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;

    PublishedLineDirection *ld = new PublishedLineDirection(global::getNewID(), name);
    undoStack->push(new cmdPublishedLineDirectionNew(m_currentLine, ld));
    m_currentLineDirection = ld;
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionEdit() {
    if(!m_currentLineDirection)
        return;

    PublishedLineDirection newLd = *m_currentLineDirection;

    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));
    refreshCurrentLine();
}

void WdgPublishedLines::actionDirectionDelete() {

}

void WdgPublishedLines::actionRoutesChange() {
    if(!m_currentLine || !m_currentLineDirection)
        return;

    QList<Route *> routes;

    for(int i = 0; i < ui->twRoutes->topLevelItemCount(); i++) {
        for(int j = 0; j < ui->twRoutes->topLevelItem(i)->childCount(); j++) {
            for(int k = 0; k < ui->twRoutes->topLevelItem(i)->child(j)->childCount(); k++) {
                Qt::CheckState checked = ui->twRoutes->topLevelItem(i)->child(j)->child(k)->checkState(0);
                if(checked == Qt::Checked)
                    routes << m_routesReference[i][j][k];
            }
        }
    }

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.setRoutes(routes);

    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));
}

void WdgPublishedLines::actionBusstopAdd() {
    if(!m_currentLineDirection)
        return;

    int busstopIndex = ui->cmbAllBusstops->currentIndex();

    Busstop *b = m_allBusstopsReference[busstopIndex];
    if(!b)
        return;

    bool busstopFound = false;
    for(int i = 0; i < m_currentLineDirection->busstopCount(); i++) {
        if(m_currentLineDirection->busstopAt(i)->linkedBusstop() == b)
            busstopFound = true;
    }
    if(busstopFound)
        return;

    PublishedBusstop *pb = new PublishedBusstop(global::getNewID(), b, "");

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.addBusstop(pb);

    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopsAddAll() {
    if(!m_currentLineDirection)
        return;

    for(int i = 0; i < m_allBusstopsReference.count(); i++) {
        ui->cmbAllBusstops->setCurrentIndex(i);
        actionBusstopAdd();
        qApp->processEvents();
    }
}

void WdgPublishedLines::actionBusstopRemove() {
    if(!m_currentLineDirection)
        return;

    if(!ui->lwBusstops->currentItem())
        return;

    PublishedBusstop *b = m_currentLineDirection->busstopAt(ui->lwBusstops->currentRow());
    if(!b)
        return;

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.removeBusstop(b);

    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopRemoveAll() {
    if(!m_currentLineDirection)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "Remove all busstops", "Do you really want to remove all busstops?", QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.setBusstops({});
    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopUp() {
    if(!m_currentLineDirection)
        return;

    QList<PublishedBusstop *> busstops = m_currentLineDirection->busstops();
    int index = ui->lwBusstops->currentRow();

    if(index == 0)
        return;

    PublishedBusstop *b = busstops[index];
    busstops.remove(index);
    busstops.insert(index - 1, b);

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.setBusstops(busstops);
    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionBusstopDown() {
    if(!m_currentLineDirection)
        return;

    QList<PublishedBusstop *> busstops = m_currentLineDirection->busstops();
    int index = ui->lwBusstops->currentRow();

    if(index == ui->lwBusstops->count() - 1)
        return;

    PublishedBusstop *b = busstops[index];
    busstops.remove(index);
    busstops.insert(index + 1, b);

    PublishedLineDirection newLd = *m_currentLineDirection;
    newLd.setBusstops(busstops);
    undoStack->push(new cmdPublishedLineDirectionEdit(m_currentLineDirection, newLd));

    refreshBusstopList();
}

void WdgPublishedLines::actionEditBusstop() {
    if(!m_currentLineDirection || !m_currentBusstop)
        return;

    PublishedBusstop newB = *m_currentBusstop;
    newB.setLabel(ui->leBusstopsLabel->text());
    newB.setShowDivider(ui->cbBusstopsShowDivider->isChecked());

    undoStack->push(new cmdPublishedBusstopEdit(m_currentBusstop, newB));

    refreshBusstopList();
}

void WdgPublishedLines::setCurrentLine(PublishedLine *newCurrentLine) {
    m_currentLine = newCurrentLine;
}

void WdgPublishedLines::refreshLineList() {
    refreshing = true;
    ui->lwLines->clear();
    m_listReference.clear();

    QList<PublishedLine *> publishedLines = projectData->publications()->lines();

    for(int i = 0; i < publishedLines.count(); i++) {
        PublishedLine *l = publishedLines[i];
        m_listReference << l;
        ui->lwLines->addItem(l->title());

        if(m_currentLine == l)
            ui->lwLines->setCurrentRow(i);
    }

    refreshing = false;
}

void WdgPublishedLines::refreshCurrentLine() {
    ui->lwDirections->clear();
    m_directionsListReference.clear();

    if(!m_currentLine)
        return;

    refreshingCurrentLine = true;

    ui->leTitle->setText(m_currentLine->title());
    ui->leFooter->setText(m_currentLine->footer());
    ui->leFilePath->setText(m_currentLine->filePath());

    QList<PublishedLineDirection *> directions = m_currentLine->directions();
    for(int i = 0; i < directions.count(); i++) {
        PublishedLineDirection *ld = directions[i];
        ui->lwDirections->addItem(ld->name());
        m_directionsListReference << ld;
        if(m_currentLineDirection == ld)
            ui->lwDirections->setCurrentRow(i);
    }

    for(int i = 0; i < m_dayTypesReference.count(); i++) {
        if(m_currentLine->hasDayType(m_dayTypesReference[i]))
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

    if(!m_currentLine || !m_currentLineDirection)
        return;

    ui->leDirectionsName->setText(m_currentLineDirection->name());

    refreshRouteCheckBoxes();

    refreshAllBusstops();

    refreshBusstopList();
}

void WdgPublishedLines::refreshDayTypes() {
    ui->lwDayTypes->clear();

    ProjectSettings *s = projectData->projectSettings();

    for(int i = 0; i < s->dayTypeCount(); i++) {
        DayType *dt = s->dayTypeAt(i);
        m_dayTypesReference << dt;

        QListWidgetItem *itm = new QListWidgetItem(dt->name());
        itm->setCheckState(Qt::Unchecked);
        ui->lwDayTypes->addItem(itm);
    }
}


void WdgPublishedLines::refreshRouteList() {
    ui->twRoutes->clear();

    m_routesReference.clear();
    m_routesDirectionsReference.clear();
    m_routesLinesReference.clear();

    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        m_routesLinesReference << l;
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

        m_routesLinesReference << l;
        m_routesDirectionsReference << directions;
        m_routesReference << routesParents;
    }
}

void WdgPublishedLines::refreshRouteCheckBoxes() {
    if(!m_currentLine || !m_currentLineDirection)
        return;

    for(int i = 0; i < ui->twRoutes->topLevelItemCount(); i++) {
        for(int j = 0; j < ui->twRoutes->topLevelItem(i)->childCount(); j++) {
            for(int k = 0; k < ui->twRoutes->topLevelItem(i)->child(j)->childCount(); k++) {
                if(m_currentLineDirection->hasRoute(m_routesReference[i][j][k]))
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

    QList<Busstop *> allBusstops = projectData->combinedRoutes(m_currentLineDirection->routes());
    for(int i = 0; i < allBusstops.count(); i++) {
        ui->cmbAllBusstops->addItem(allBusstops[i]->name());
    }

    m_allBusstopsReference = allBusstops;
}

void WdgPublishedLines::refreshBusstopList() {
    if(!m_currentLineDirection)
        return;

    int scrollValue = ui->lwBusstops->verticalScrollBar()->value();

    ui->lwBusstops->clear();

    for(int i = 0; i < m_currentLineDirection->busstopCount(); i++) {
        PublishedBusstop *b = m_currentLineDirection->busstopAt(i);

        QString text = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        if(b->showDivider())
            text += "\r\n--------------------------------------------------------------------------------";
        ui->lwBusstops->addItem(text);

        if(b == m_currentBusstop)
            ui->lwBusstops->setCurrentRow(i);
    }

    ui->lwBusstops->verticalScrollBar()->setValue(scrollValue);
}

void WdgPublishedLines::on_lwLines_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshing)
        return;

    if(!current) {
        m_currentLine = nullptr;
        m_currentLineDirection = nullptr;
        return;
    }

    m_currentLine = m_listReference[ui->lwLines->currentRow()];

    if(m_currentLine->directionCount() == 0) {
        m_currentLineDirection = nullptr;
    } else {
        m_currentLineDirection = m_currentLine->directionAt(0);
    }

    emit currentLineChanged(m_currentLine);
    refreshCurrentLine();
}

void WdgPublishedLines::on_lwDirections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshingCurrentLine)
        return;

    if(!current)
        m_currentLineDirection = nullptr;
    else
        m_currentLineDirection = m_directionsListReference[ui->lwDirections->currentRow()];

    //emit currentLineChanged(m_currentLineDirection);
    refreshCurrentLineDirection();
}


void WdgPublishedLines::on_pbFilePathBrowse_clicked() {
    if(!m_currentLine)
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

    if(!m_currentLineDirection)
        return;

    m_currentBusstop = m_currentLineDirection->busstopAt(ui->lwBusstops->currentRow());
    ui->leBusstopsLabel->setPlaceholderText(m_currentBusstop->linkedBusstop()->name());
    QString label = m_currentBusstop->label();
    ui->leBusstopsLabel->setText(label.isEmpty() ? m_currentBusstop->linkedBusstop()->name() : label);
    ui->cbBusstopsShowDivider->setChecked(m_currentBusstop->showDivider());
}


void WdgPublishedLines::on_pbBusstopSearchAndReplace_clicked() {
    if(!m_currentLineDirection)
        return;

    bool ok1, ok2;
    QString search = QInputDialog::getText(this, "Search and replace", "Enter search string:", QLineEdit::Normal, "", &ok1);
    QString replace = QInputDialog::getText(this, "Search and replace", "Enter replace string:", QLineEdit::Normal, "", &ok2);

    if(!ok1 || !ok2)
        return;

    for(int i = 0; i < m_currentLineDirection->busstopCount(); i++) {
        PublishedBusstop *b = m_currentLineDirection->busstopAt(i);
        QString input = b->label().isEmpty() ? b->linkedBusstop()->name() : b->label();
        QString result = input;
        result.replace(search, replace, Qt::CaseInsensitive);
        if(result != input)
            b->setLabel(result);
    }

    refreshBusstopList();
}


void WdgPublishedLines::on_lwDayTypes_itemChanged(QListWidgetItem *item) {
    if(!item || !m_currentLine)
        return;

    if(refreshingCurrentLine)
        return;

    int index = ui->lwDayTypes->row(item);

    Qt::CheckState checkState = item->checkState();
    if(checkState == Qt::Checked)
        m_currentLine->addDayType(m_dayTypesReference[index]);
    else
        m_currentLine->addDayType(m_dayTypesReference[index]);
}

