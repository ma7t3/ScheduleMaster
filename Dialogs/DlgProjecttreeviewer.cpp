#include "Dialogs\DlgProjecttreeviewer.h"
#include "ui_DlgProjecttreeviewer.h"

ProjectTreeViewer::ProjectTreeViewer(QWidget *parent, ProjectData *p) :
    QDialog(parent),
    ui(new Ui::ProjectTreeViewer),
    projectData(p)
{
    ui->setupUi(this);

    ui->twTree->setColumnHidden(1, true);

    loadBusstops();
    loadLines();
}

ProjectTreeViewer::~ProjectTreeViewer()
{
    delete ui;
}

void ProjectTreeViewer::loadBusstops()
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    QTreeWidgetItem *mainItm = new QTreeWidgetItem(ui->twTree);
    mainItm->setFont(0, bold);
    ui->twTree->addTopLevelItem(mainItm);

    QList<QTreeWidgetItem *> items;
    
    for(int i = 0; i < projectData->busstopCount(); i++) {
        Busstop *b = projectData->busstopAt(i);
        QTreeWidgetItem *itm = new QTreeWidgetItem(mainItm);
        if(!b) {
            itm->setText(1, "/");
            itm->setText(0, tr("invalid"));
            itm->setFont(0, italic);
            itm->setBackground(1, Qt::red);
            itm->setForeground(1, Qt::white);
        } else {
            itm->setText(1, b->id());
            itm->setText(0, b->name());
        }

        items << itm;
    }
    mainItm->addChildren(items);
    mainItm->setText(0, tr("Busstops (%n)", "", mainItm->childCount()));
}

void ProjectTreeViewer::loadLines()
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    QTreeWidgetItem *mainItm = new QTreeWidgetItem(ui->twTree);
    mainItm->setFont(0, bold);
    ui->twTree->addTopLevelItem(mainItm);

    QList<QTreeWidgetItem *> items;
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QTreeWidgetItem *itm = new QTreeWidgetItem(mainItm);
        if(!l) {
            itm->setText(1, "/");
            itm->setText(0, "invalid");
            itm->setFont(0, italic);
            itm->setBackground(1, Qt::red);
            itm->setForeground(1, Qt::white);
        } else {
            itm->setText(1, l->id());
            itm->setText(0, "[" + l->name() + "] " + l->getDescription());

            QTreeWidgetItem *routesItm = new QTreeWidgetItem(itm);
            itm->addChild(routesItm);
            itm->addChildren(loadRoutes(l, routesItm));
            routesItm->setText(0, tr("Routes (%n)", "", routesItm->childCount()));

            QTreeWidgetItem *tripsItm = new QTreeWidgetItem(itm);
            itm->addChild(tripsItm);
            itm->addChildren(loadTrips(l->trips(), tripsItm));
            tripsItm->setText(0, tr("Trips (%1)", "", tripsItm->childCount()));
        }
        items << itm;
    }
    mainItm->addChildren(items);
    mainItm->setText(0, tr("Lines (%1)", "", mainItm->childCount()));
}

QList<QTreeWidgetItem*> ProjectTreeViewer::loadRoutes(Line *l, QTreeWidgetItem *parent)
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    QList<QTreeWidgetItem *> items;

    for(int i = 0; i < l->routeCount(); i++) {
        Route *r = l->routeAt(i);
        QTreeWidgetItem *itm = new QTreeWidgetItem(parent);
        if(!r) {
            itm->setText(1, "/");
            itm->setText(0, tr("invalid"));
            itm->setFont(0, italic);
            itm->setBackground(1, Qt::red);
            itm->setForeground(1, Qt::white);
        } else {
            itm->setText(1, r->id());
            itm->setText(0, "(" + QString::number(r->code()) + ") " + r->name());
            for(int j = 0; j < r->busstopCount(); j++) {
                Busstop *b = r->busstopAt(j);
                QTreeWidgetItem *subItm = new QTreeWidgetItem(itm);
                if(!b) {
                    itm->setText(1, "/");
                    itm->setText(0, tr("invalid"));
                    itm->setFont(0, italic);
                    itm->setBackground(1, Qt::red);
                    itm->setForeground(1, Qt::white);
                } else {
                    subItm->setText(1, b->id());
                    subItm->setText(0, b->name());
                }
                itm->addChild(subItm);
            }
        }
        items << itm;
    }
    return items;
}

QList<QTreeWidgetItem*> ProjectTreeViewer::loadTrips(QList<Trip *> trips, QTreeWidgetItem *parent)
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    QList<QTreeWidgetItem *> items;

    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];
        QTreeWidgetItem *itm = new QTreeWidgetItem(parent);
        if(!t) {
            itm->setText(1, "/");
            itm->setText(0, tr("invalid"));
            itm->setFont(0, italic);
            itm->setBackground(1, Qt::red);
            itm->setForeground(1, Qt::white);
        } else {
            itm->setText(1, t->id());
            itm->setText(0, "[" + t->startTime().toString("hh:mm") + "] " + t->route()->name() + " (" + t->weekDays()->toString() + ") (" + QString::number(t->childCount()) + " childs) (" + t->id() + ")");
            itm->addChildren(loadTripChilds(t, itm));
        }
        items << itm;
    }
    return items;
}

QList<QTreeWidgetItem*> ProjectTreeViewer::loadTripChilds(Trip *t, QTreeWidgetItem *parent)
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    QList<QTreeWidgetItem *> items;
    
    for(int i = 0; i < t->repeatTimes(); i++) {
        Trip *c = t->repetitionAt(i);
        qDebug() << c;
        QTreeWidgetItem *itm = new QTreeWidgetItem(parent);
        if(!c) {
            itm->setText(1, "/");
            itm->setText(0, tr("invalid"));
            itm->setFont(0, italic);
            itm->setBackground(1, Qt::red);
            itm->setForeground(1, Qt::white);
        } else {
            itm->setText(1, c->id());
            itm->setText(0, "[" + c->startTime().toString("hh:mm") + "] " + c->route()->name());
        }
        items << itm;
    }
    return items;
}











