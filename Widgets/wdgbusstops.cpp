#include "wdgbusstops.h"
#include "ui_wdgbusstops.h"

#include <QMessageBox>
#include <QUndoStack>
#include <QToolBar>

#include "Mainwindow.h"
#include "Commands/CmdBusstops.h"
#include "Dialogs/DlgBusstopeditor.h"

#include "App/global.h"

WdgBusstops::WdgBusstops(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgBusstops),
    projectData(((MainWindow *)parent)->projectData()) {
    ui->setupUi(this);

    _actionNew          = ui->twBusstops->addAction(QIcon(":/icons/Add.ico"),             tr("New"));
    _actionEdit         = ui->twBusstops->addAction(QIcon(":/icons/Edit.ico"),            tr("Edit"));
    _actionDelete       = ui->twBusstops->addAction(QIcon(":/icons/Delete.ico"),          tr("Delete"));
    _actionViewSchedule = ui->twBusstops->addAction(QIcon(":/icons/BusstopSchedule.ico"), tr("View Schedule"));

    _actionEdit->setDisabled(true);
    _actionDelete->setDisabled(true);

    _actionEdit->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Space), QKeySequence(Qt::Key_Return)});
    _actionEdit->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionDelete->setShortcut(QKeySequence::Delete);
    _actionDelete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _actionViewSchedule->setShortcuts({QKeySequence(tr("Alt+Enter")), QKeySequence(tr("Alt+Return"))});
    _actionViewSchedule->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->twBusstops->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_actionNew,          &QAction::triggered,              this,                &WdgBusstops::actionNew);
    connect(_actionEdit,         &QAction::triggered,              this,                &WdgBusstops::actionEdit);
    connect(_actionDelete,       &QAction::triggered,              this,                &WdgBusstops::actionDelete);

    connect(_actionNew,          &QAction::enabledChanged,         ui->pbBusstopNew,    &QPushButton::setEnabled);
    connect(_actionEdit,         &QAction::enabledChanged,         ui->pbBusstopEdit,   &QPushButton::setEnabled);
    connect(_actionDelete,       &QAction::enabledChanged,         ui->pbBusstopDelete, &QPushButton::setEnabled);

    connect(ui->pbBusstopNew,    &QPushButton::clicked,            this,                &WdgBusstops::actionNew);
    connect(ui->pbBusstopEdit,   &QPushButton::clicked,            this,                &WdgBusstops::actionEdit);
    connect(ui->twBusstops,      &QTableWidget::cellDoubleClicked, this,                &WdgBusstops::actionEdit);
    connect(ui->pbBusstopDelete, &QPushButton::clicked,            this,                &WdgBusstops::actionDelete);
    connect(ui->leBusstopSearch, &QLineEdit::textChanged,          this,                &WdgBusstops::refreshBusstopList);

    connect(_actionViewSchedule, &QAction::triggered,              this,                [this](){emit if(_currentBusstop && projectData->projectSettings()->dayTypeCount() > 0) busstopScheduleRequested(_currentBusstop, {}, projectData->projectSettings()->dayTypeAt(0));});

    ui->twBusstops->verticalHeader()->setVisible(false);
    ui->twBusstops->setEditTriggers(QTableWidget::NoEditTriggers);
}

WdgBusstops::~WdgBusstops() {
    delete ui;
}

QList<QAction *> WdgBusstops::actions() {
    return ui->twBusstops->actions();
}

void WdgBusstops::refreshUI() {
    int selectionCount = ui->twBusstops->selectionModel()->selectedRows(0).count();

    _actionEdit->setEnabled(selectionCount == 1);
    _actionDelete->setEnabled(selectionCount >= 1);

    if(selectionCount == 0)
        ui->twBusstops->setCurrentItem(nullptr);
}

void WdgBusstops::refreshBusstopList() {
    qDebug() << "refreshing busstop list...";
    _refreshing = true;

    QString filter = ui->leBusstopSearch->text();
    ui->twBusstops->setRowCount(0);
    _tableReference.clear();

    QList<Busstop *> busstops = projectData->busstops();
    busstops = ProjectData::sortItems(busstops);

    for(int i = 0; i < busstops.count(); i++) {
        Busstop * b = busstops[i];

        QString name = b->name();
        bool important = b->isImportant();

        bool foundInName = name.contains(filter, Qt::CaseInsensitive);

        bool foundInLines = false;
        QList<Line *> usedLines = projectData->linesAtBusstop(b);
        for(int j = 0; j < usedLines.count(); j++) {
            if(usedLines[j]->name().contains(filter, Qt::CaseInsensitive)) {
                foundInLines = true;
                break;
            }
        }
        if(!foundInLines && !foundInName)
            continue;

        int currentRow = ui->twBusstops->rowCount();
        ui->twBusstops->insertRow(currentRow);


        int xValue = 5;
        for(int j = 0; j < usedLines.count(); j++) {
            int width = ui->twBusstops->fontMetrics().boundingRect(usedLines[j]->name()).width();
            xValue += (width + 20);
        }

        QPixmap linesPixmap(xValue, 15);
        linesPixmap.fill(Qt::transparent);
        QPainter painter(&linesPixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        xValue = 5;
        for(int j = 0; j < usedLines.count(); j++) {
            int width = ui->twBusstops->fontMetrics().boundingRect(usedLines[j]->name()).width();
            QColor color = usedLines[j]->color();
            QRect rect(xValue, 0, width + 15, 15);
            xValue += (width + 20);
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect, 7.5, 7.5);
            painter.setPen(global::getContrastColor(color));
            painter.drawText(rect, Qt::AlignCenter, usedLines[j]->name());
        }

        QTableWidgetItem * nameItm = new QTableWidgetItem(name);
        QTableWidgetItem * usedLinesItm = new QTableWidgetItem;

        usedLinesItm->setData(Qt::DecorationRole, linesPixmap);

        QFont bold;
        bold.setBold(true);
        if(important)
            nameItm->setFont(bold);

        ui->twBusstops->setItem(currentRow, 0, nameItm);
        ui->twBusstops->setItem(currentRow, 1, usedLinesItm);

        _tableReference << b;

        if(b == _currentBusstop)
            ui->twBusstops->setCurrentItem(nameItm);
    }

    if(ui->leBusstopSearch->text().isEmpty()) {
        ui->twBusstops->resizeColumnToContents(0);
        ui->twBusstops->resizeColumnToContents(1);
    }

    for(int i = 0; i < ui->twBusstops->rowCount(); i++)
        ui->twBusstops->setRowHeight(i, 15);

    _refreshing = false;
}

Busstop *WdgBusstops::currentBusstop() {
    return _currentBusstop;
}

void WdgBusstops::actionNew() {
    Busstop *b = projectData->newBusstop();

    busstopEditor dlg(this, *b, true);
    if(dlg.exec() != QDialog::Accepted) {
        delete b;
        return;
    }

    *b = dlg.busstop();

    projectData->undoStack()->push(new CmdBusstopNew(projectData, b));
    emit refreshRequested();
}

void WdgBusstops::actionEdit() {
    if(!_currentBusstop)
        return;

    busstopEditor dlg(this, *_currentBusstop, false);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Busstop newB = dlg.busstop();

    projectData->undoStack()->push(new CmdBusstopEdit(_currentBusstop, newB));
    emit refreshRequested();
}

void WdgBusstops::actionDelete() {
    QModelIndexList selection = ui->twBusstops->selectionModel()->selectedRows(0);

    QString showList ="<ul>";
    QList<Busstop *> busstops;
    for(int i = 0; i < selection.count(); i++) {
        Busstop *b = _tableReference[selection[i].row()];
        busstops << b;
        showList += QString("<li>%1</li>").arg(b->name());
    }
    showList += "</ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete %n busstop(s)", "", busstops.count()), tr("<p><b>Do you really want to delete these %n busstop(s)?</b></p>%1", "", busstops.count()).arg(showList), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdBusstopsDelete(projectData, busstops));
    emit refreshRequested();
}

/*
void WdgBusstops::actionExportList() {
    QString plainText, csvText, htmlText;

    QFileInfo fi(projectData->filePath());

    csvText = "Name;Important;used by lines\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All busstops of project " + fi.baseName() + "</title></head>\r\n\t<body>\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Used by lines</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->busstopCount(); i++) {
        Busstop *b = projectData->busstopAt(i);
        QString name = b->name();
        bool important = b->isImportant();
        QString importantStr = important ? "Yes" : "No";
        QStringList usedLinesStrList;
        QList<Line *> usedLines = projectData->linesAtBusstop(b);
        for(int i = 0; i < usedLines.count(); i++)
            usedLinesStrList << usedLines[i]->name();
        QString usedLinesStr = usedLinesStrList.join(", ");
        plainText += name + "\r\n";

        csvText += "\"" + name + "\";" + importantStr + ";" + usedLinesStr + "\r\n";
        if(important)
            htmlText += "\t\t\t\t\t<tr><td><b>" + name + "</b></td><td>" + usedLinesStr + "</td></tr>\r\n";
        else
            htmlText += "\t\t\t\t\t<tr><td>" + name + "</td><td>" + usedLinesStr + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFileName("all_busstops");
    dlg.setFolderName(fi.baseName());
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}
*/

void WdgBusstops::on_twBusstops_itemSelectionChanged() {
    if(_refreshing)
        return;

    QTableWidgetItem *current = ui->twBusstops->currentItem();
    int selectionCount = ui->twBusstops->selectionModel()->selectedRows().count();

    if(!current || selectionCount == 0 || selectionCount > 1)
        _currentBusstop = nullptr;
    else
        _currentBusstop = _tableReference[current->row()];

    refreshUI();

    emit currentBusstopChanged(_currentBusstop);
}
