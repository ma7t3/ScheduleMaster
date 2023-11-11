
// Includes
//********************************************************************************************************************************************************************


#include <QCloseEvent>

#include <QSplashScreen>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QToolBar>
#include <QFileDialog>

#include <QUuid>
#include <QScrollBar>

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>


#include "Commands/cmdgeneral.h"
#include "Mainwindow.h"

#include "App/global.h"
#include "ui_Mainwindow.h"

#include "Widgets/wdgschedule.h"
//#include "filehandler.h"

#include "Dialogs/DlgStartupdialog.h"
#include "Dialogs/DlgProjecttreeviewer.h"
#include "Dialogs/DlgTroubleshooter.h"
#include "Dialogs/dlgprojectsettings.h"


//********************************************************************************************************************************************************************
// Constructor, Destructor, Overwrites (CloseEvent e.g.)
//********************************************************************************************************************************************************************

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    projectData(new ProjectData),
    undoStack(new QUndoStack),
    fileHandler(new DlgFileHandler(this, projectData))
{
    QString splashScreenInfo = "loading ui...";
    QSplashScreen splashScreen;
    QString imagePath = ":/main/splashscreen/splashscreen.png";
    QDate now = QDate::currentDate();

    if(now.dayOfYear() > 300 && now.dayOfYear() < 310)
        imagePath = ":/main/splashscreen/splashscreen_halloween.png";

    QPixmap pixmap(imagePath);
    splashScreen.setPixmap(pixmap);
    splashScreen.showMessage(splashScreenInfo, Qt::AlignBottom);
    splashScreen.finish(this);
    splashScreen.show();

    QObject().thread()->sleep(2);

    ui->setupUi(this);

    this->showMaximized();

    knownFile = false;
    saved = true;
    //projectFilePath = "";

    dwUndoView->setWidget(wdgUndoView);
    dwBusstops->setWidget(wdgBusstops);
    dwLines->setWidget(wdgLines);
    dwRoutes->setWidget(wdgRoutes);
    dwSchedule->setWidget(wdgSchedule);
    dwBusstopSchedule->setWidget(wdgBusstopSchedule);
    dwTours->setWidget(wdgTours);
    dwTourEditor->setWidget(wdgTourEditor);
    dwPublishedLines->setWidget(wdgPublishedLines);

    /*ui->statusbar->addPermanentWidget(statusProgressBar, -5);
    statusProgressBar->setMaximum(1);
    statusProgressBar->resize(QSize(5, 5));*/

    actionWorkspaceTrackLayout();

    ui->centralwidget->setHidden(true);

    QAction *actDockBusstops = dwBusstops->toggleViewAction();
    QAction *actDockLines= dwLines->toggleViewAction();
    QAction *actDockRoutes = dwRoutes->toggleViewAction();
    QAction *actDockSchedule = dwSchedule->toggleViewAction();
    QAction *actDockBusstopSchedule = dwBusstopSchedule->toggleViewAction();
    QAction *actDockTours = dwTours->toggleViewAction();
    QAction *actDockTourEditor = dwTourEditor->toggleViewAction();
    QAction *actDockPublishedLines = dwPublishedLines->toggleViewAction();

    actDockBusstops->setIcon(QIcon(":/main/icons/Busstop.ico"));
    actDockLines->setIcon(QIcon(":/main/icons/Line.ico"));
    actDockRoutes->setIcon(QIcon(":/main/icons/Route.ico"));
    actDockSchedule->setIcon(QIcon(":/main/icons/Schedule.ico"));
    actDockBusstopSchedule->setIcon(QIcon(":/main/icons/BusstopSchedule.ico"));
    actDockTours->setIcon(QIcon(":/main/icons/TourList.ico"));
    actDockTourEditor->setIcon(QIcon(":/main/icons/Tour.ico"));

    actDockBusstops->setShortcut(QKeySequence(Qt::ALT|Qt::Key_B));
    actDockLines->setShortcut(QKeySequence(Qt::ALT|Qt::Key_L));
    actDockRoutes->setShortcut(QKeySequence(Qt::ALT|Qt::Key_R));
    actDockSchedule->setShortcut(QKeySequence(Qt::ALT|Qt::Key_S));
    actDockTours->setShortcut(QKeySequence(Qt::ALT|Qt::Key_T));

    ui->actionFileNew->setShortcut(QKeySequence::New);
    ui->actionFileOpen->setShortcut(QKeySequence::Open);
    ui->actionFileSave->setShortcut(QKeySequence::Save);
    ui->actionFileSaveAs->setShortcuts({QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_S), QKeySequence::SaveAs});
    ui->actionFileClose->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_W), QKeySequence::Close});
    ui->actionFileQuit->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_Q), QKeySequence::Quit});

    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);

    ui->menuDocks->addAction(actDockBusstops);
    ui->menuDocks->addAction(actDockLines);
    ui->menuDocks->addAction(actDockRoutes);
    ui->menuDocks->addAction(actDockSchedule);
    ui->menuDocks->addAction(actDockBusstopSchedule);
    ui->menuDocks->addAction(actDockTours);
    ui->menuDocks->addAction(actDockTourEditor);
    ui->menuDocks->addSeparator();
    ui->menuDocks->addAction(actDockPublishedLines);

    wdgBusstops->setMenubarActions(ui->actionBusstopsNew, ui->actionBusstopsEdit, ui->actionBusstopsDelete);
    wdgLines->setMenubarActions(ui->actionLinesNew, ui->actionLinesEdit, ui->actionLinesDelete);
    wdgRoutes->setMenubarActions(ui->actionRoutesNew, ui->actionRoutesEdit, ui->actionRoutesDuplicate, ui->actionRoutesDelete);

    QObject::connect(ui->actionBusstopsNew, SIGNAL(triggered()), wdgBusstops, SLOT(actionNew()));
    QObject::connect(ui->actionBusstopsEdit, SIGNAL(triggered()), wdgBusstops, SLOT(actionEdit()));
    QObject::connect(ui->actionBusstopsDelete, SIGNAL(triggered()), wdgBusstops, SLOT(actionDelete()));

    QObject::connect(ui->actionLinesNew, SIGNAL(triggered()), wdgLines, SLOT(actionNew()));
    QObject::connect(ui->actionLinesEdit, SIGNAL(triggered()), wdgLines, SLOT(actionEdit()));
    QObject::connect(ui->actionLinesDelete, SIGNAL(triggered()), wdgLines, SLOT(actionDelete()));

    QObject::connect(ui->actionRoutesNew, SIGNAL(triggered()), wdgRoutes, SLOT(actionNew()));
    QObject::connect(ui->actionRoutesEdit, SIGNAL(triggered()), wdgRoutes, SLOT(actionEdit()));
    QObject::connect(ui->actionRoutesDuplicate, SIGNAL(triggered()), wdgRoutes, SLOT(actionDuplicate()));
    QObject::connect(ui->actionRoutesDelete, SIGNAL(triggered()), wdgRoutes, SLOT(actionDelete()));

    /*ui->menuNew->addAction(wdgRoutes->menubarActionNew());
    ui->menuEdit_2->addAction(wdgRoutes->menubarActionEdit());
    ui->menuDelete->addAction(wdgRoutes->menubarActionDelete());
    ui->menuExport->addAction(wdgRoutes->menubarActionExportListCurrent());
    ui->menuExport->addAction(wdgRoutes->menubarActionExportListAll());*/


    QToolBar *toolbar = new QToolBar("toolbar", this);
    toolbar->addAction(ui->actionFileNew);
    toolbar->addAction(ui->actionFileOpen);
    toolbar->addAction(ui->actionFileSave);
    toolbar->addAction(ui->actionUndo);
    toolbar->addAction(ui->actionRedo);
    toolbar->addSeparator();
    toolbar->addAction(actDockBusstops);
    toolbar->addAction(actDockLines);
    toolbar->addAction(actDockRoutes);
    toolbar->addAction(actDockSchedule);
    toolbar->addAction(actDockBusstopSchedule);
    toolbar->addAction(actDockTours);
    toolbar->addAction(actDockTourEditor);
    toolbar->addAction(actDockPublishedLines);
    toolbar->addSeparator();
    toolbar->addAction(ui->actionWorkspaceTrackLayout);
    toolbar->addAction(ui->actionWorkspaceBusstopSchedule);
    toolbar->addAction(ui->actionWorkspaceScheduling);
    toolbar->addAction(ui->actionWorkspaceTourPlanning);
    toolbar->addAction(ui->actionWorkspacePublish);
    this->addToolBar(toolbar);

    dwBusstopSchedule->resize(900, 831);

    setRedoEnabled(false);
    setUndoEnabled(false);
    setSaved(true);

    QObject::connect(ui->actionFileNew, SIGNAL(triggered()), this, SLOT(actionFileNew()));
    QObject::connect(ui->actionFileOpen, SIGNAL(triggered()), this, SLOT(actionFileOpen()));
    QObject::connect(ui->actionFileSave, SIGNAL(triggered()), this, SLOT(actionFileSave()));
    QObject::connect(ui->actionFileSaveAs, SIGNAL(triggered()), this, SLOT(actionFileSaveAs()));
    QObject::connect(ui->actionFileClose, SIGNAL(triggered()), this, SLOT(actionFileClose()));
    QObject::connect(ui->actionFileQuit, SIGNAL(triggered()), this, SLOT(actionQuit()));

    QObject::connect(undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(setSaved(bool)));
    QObject::connect(undoStack, SIGNAL(canUndoChanged(bool)), this, SLOT(setUndoEnabled(bool)));
    QObject::connect(undoStack, SIGNAL(canRedoChanged(bool)), this, SLOT(setRedoEnabled(bool)));
    QObject::connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(actionUndo()));
    QObject::connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(actionRedo()));

    QObject::connect(ui->actionWorkspaceTrackLayout, SIGNAL(triggered()), this, SLOT(actionWorkspaceTrackLayout()));
    QObject::connect(ui->actionWorkspaceBusstopSchedule, SIGNAL(triggered()), this, SLOT(actionWorkspaceBusstopSchedule()));
    QObject::connect(ui->actionWorkspaceScheduling, SIGNAL(triggered()), this, SLOT(actionWorkspaceScheduling()));
    QObject::connect(ui->actionWorkspaceTourPlanning, SIGNAL(triggered()), this, SLOT(actionWorkspaceTourPlanning()));
    QObject::connect(ui->actionWorkspacePublish, SIGNAL(triggered()), this, SLOT(actionWorkspacePublish()));

    QObject::connect(wdgBusstops, SIGNAL(currentBusstopChanged(Busstop*)), wdgBusstopSchedule, SLOT(setAll(Busstop*)));

    QObject::connect(wdgLines, SIGNAL(currentLineChanged(Line*)), wdgRoutes, SLOT(setCurrentLine(Line*)));
    QObject::connect(wdgLines, SIGNAL(currentLineChanged(Line*)), wdgSchedule, SLOT(setCurrentLine(Line*)));

    QObject::connect(wdgSchedule, SIGNAL(busstopScheduleRequested(Busstop *, QList<Busstop *>, QList<Line *>, int)), this, SLOT(actionOpenBusstopSchedule(Busstop *, QList<Busstop *>, QList<Line *>, int)));

    QObject::connect(wdgTours, SIGNAL(currentTourChanged(Tour*)), wdgTourEditor, SLOT(setCurrentTour(Tour*)));


    splashScreenInfo = "loading startup dialog...";
    StartupDialog dlg;
    dlg.exec();
    if(dlg.result() == QDialog::Accepted) {
        int action = dlg.getAction();
        if(action == StartupDialog::openFile)
            actionFileOpen();
        else if(action == StartupDialog::openRecentFile)
            openFile(dlg.getFilePath());
        else if(action == StartupDialog::quit)
            MainWindow::close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Close Event
void MainWindow::closeEvent(QCloseEvent * event)
{
    /*if(undoStack->isClean()) {
        event->accept();
        return;
    }*/

    if(!actionFileClose())
         event->ignore();
    else
         event->accept();
}


//********************************************************************************************************************************************************************
// Actions
//********************************************************************************************************************************************************************

bool MainWindow::actionFileNew() {
    return actionFileClose();
}

bool MainWindow::actionFileOpen() {
    if(!actionFileClose())
        return false;

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Projects";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString path = QFileDialog::getOpenFileName(this, "", dir.path(), "ScheduleMaster2 File (*.smp2)");
    if(path == "")
        return false;

    return openFile(path);
}

bool MainWindow::actionFileSave() {
    if(!knownFile)
        return actionFileSaveAs();

    return saveFile(projectData->filePath());
    //return saveFile(projectFilePath);
}

bool MainWindow::actionFileSaveAs() {
    QDir dir = QDir::homePath() + "/.ScheduleMaster/Projects";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString path = QFileDialog::getSaveFileName(this, "", dir.path(), "ScheduleMaster2 File (*.smp2)");
    if(path == "")
        return false;

    if(saveFile(path)) {
        knownFile = true;
        //projectFilePath = path;
        projectData->setFilePath(path);
        return true;
    }
    return false;
}

bool MainWindow::actionFileClose() {
    if(!undoStack->isClean()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, "Unsaved Changes", "<p><b>There are unsaved changes!</b></p><p>Do want to save them before closing this file?</p>", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if(msg == QMessageBox::Yes) {
            if(!actionFileSave())
                return false;
        } else if(msg == QMessageBox::Cancel) {
            return false;
        }
    }
    projectData->reset();
    knownFile = false;
    //projectFilePath = "";
    projectData->setFilePath("");
    wdgBusstops->refreshBusstopTable();
    wdgLines->refreshLineTable();
    wdgRoutes->refreshRouteTable();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    return true;
}

bool MainWindow::actionQuit() {
    close();
    return true;
}

void MainWindow::actionUndo() {
    undoStack->undo();
    wdgBusstops->refreshBusstopTable();
    wdgLines->refreshLineTable();
    wdgRoutes->refreshRouteTable();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    wdgSchedule->refreshDayTypes();
}

void MainWindow::actionRedo() {
    undoStack->redo();
    wdgBusstops->refreshBusstopTable();
    wdgLines->refreshLineTable();
    wdgRoutes->refreshRouteTable();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    wdgSchedule->refreshDayTypes();
}

/* Notiz an mich:
 * Am besten erst alle nicht-gewollten Docks schließen und dann die gewollten öffnen.
 * Sieht im Code zwar unübersichtlicher aus, aber sonst kann es passiern, dass im MainWindow nicht genug Platz ist.
 * Dann "explodiert" das Fenster, springt aus dem maximierten Modus und ist dann größer als der Bildschirm. ^^
*/

void MainWindow::actionWorkspaceTrackLayout() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->addDockWidget(Qt::RightDockWidgetArea, dwLines);
    this->addDockWidget(Qt::RightDockWidgetArea, dwRoutes);

    dwBusstopSchedule->close();
    dwBusstopSchedule->setFloating(true);

    dwSchedule->close();
    dwSchedule->setFloating(true);

    dwTours->close();
    dwTours->setFloating(false);

    dwTourEditor->close();
    dwTourEditor->setFloating(true);

    dwUndoView->close();
    dwUndoView->setFloating(true);

    dwPublishedLines->close();
    dwPublishedLines->setFloating(true);

    dwBusstops->show();
    dwBusstops->setFloating(false);

    dwLines->show();
    dwLines->setFloating(false);

    dwRoutes->show();
    dwRoutes->setFloating(false);

    this->resizeDocks({dwBusstops, dwLines}, {static_cast<int>(this->width() * 0.5), static_cast<int>(this->width() * 0.5)}, Qt::Horizontal);

    ui->actionWorkspaceTrackLayout->setChecked(true);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceBusstopSchedule() {
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->addDockWidget(Qt::RightDockWidgetArea, dwBusstopSchedule);

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(false);
    dwRoutes->close();

    dwSchedule->setFloating(true);
    dwSchedule->close();

    dwTours->setFloating(false);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwUndoView->setFloating(true);
    dwUndoView->close();

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwBusstops->show();
    dwBusstops->setFloating(false);

    dwBusstopSchedule->show();
    dwBusstopSchedule->setFloating(false);

    this->resizeDocks({dwBusstops, dwBusstopSchedule}, {static_cast<int>(this->width() * 0.5), static_cast<int>(this->width() * 0.5)}, Qt::Horizontal);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(true);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceScheduling() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwLines);
    this->addDockWidget(Qt::RightDockWidgetArea, dwSchedule);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwUndoView);
    this->tabifyDockWidget(dwLines, dwBusstops);
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->tabifyDockWidget(dwSchedule, dwRoutes);

    dwBusstops->setFloating(false);
    dwBusstops->close();

    dwBusstopSchedule->setFloating(true);
    dwBusstopSchedule->close();

    dwRoutes->setFloating(false);
    dwRoutes->close();

    dwTours->setFloating(false);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwLines->show();
    dwLines->setFloating(false);

    dwSchedule->show();
    dwSchedule->setFloating(false);

    dwUndoView->show();
    dwUndoView->setFloating(false);

    this->resizeDocks({dwLines, dwSchedule}, {static_cast<int>(this->width() * 0.01), static_cast<int>(this->width() * 0.99)}, Qt::Horizontal);
    this->resizeDocks({dwLines, dwUndoView}, {static_cast<int>(this->width() * 0.8), static_cast<int>(this->width() * 0.2)}, Qt::Vertical);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(true);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceTourPlanning() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwTours);
    this->addDockWidget(Qt::RightDockWidgetArea, dwTourEditor);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwUndoView);
    this->tabifyDockWidget(dwTours, dwLines);
    this->tabifyDockWidget(dwLines, dwBusstops);
    this->tabifyDockWidget(dwTourEditor, dwSchedule);

    dwBusstops->setFloating(false);
    dwBusstops->close();

    dwBusstopSchedule->setFloating(true);
    dwBusstopSchedule->close();

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(true);
    dwRoutes->close();

    dwSchedule->setFloating(false);
    dwSchedule->close();

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwTours->show();
    dwTours->setFloating(false);

    dwTourEditor->show();
    dwTourEditor->setFloating(false);

    dwUndoView->show();
    dwUndoView->setFloating(false);

    this->resizeDocks({dwTours, dwTourEditor}, {static_cast<int>(this->width() * 0.33), static_cast<int>(this->width() * 0.67)}, Qt::Horizontal);
    this->resizeDocks({dwTours, dwUndoView}, {static_cast<int>(this->width() * 0.8), static_cast<int>(this->width() * 0.2)}, Qt::Vertical);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(true);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspacePublish() {
    this->addDockWidget(Qt::RightDockWidgetArea, dwPublishedLines);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->tabifyDockWidget(dwBusstops, dwLines);

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(true);
    dwRoutes->close();

    dwSchedule->setFloating(true);
    dwSchedule->close();

    dwTours->setFloating(true);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwUndoView->setFloating(true);
    dwUndoView->close();

    dwBusstops->close();
    dwBusstops->setFloating(false);

    dwBusstopSchedule->close();
    dwBusstopSchedule->setFloating(true);

    dwPublishedLines->show();
    dwPublishedLines->setFloating(false);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(true);
}

void MainWindow::actionOpenBusstopSchedule(Busstop *b, QList<Busstop *> directions, QList<Line *> lines, int days) {
    wdgBusstopSchedule->setAll(b, directions, lines, days);
    dwBusstopSchedule->show();
}

void MainWindow::setUndoEnabled(bool b) {
    ui->actionUndo->setEnabled(b);
}

void MainWindow::setRedoEnabled(bool b) {
    ui->actionRedo->setEnabled(b);
}

void MainWindow::setSaved(bool b) {
    if(!b)
        QMainWindow::setWindowTitle("* ScheduleMaster");
    else
        QMainWindow::setWindowTitle("ScheduleMaster");

    ui->actionFileSave->setDisabled(b);
}

bool MainWindow::openFile(QString path) {
    ui->statusbar->showMessage("Opening project file...");

    QFile f(path);
    QFileInfo fi(path);

    if(!f.exists()) {
        QMessageBox::warning(this, "File not found", "<p>The given file was not found!</p>");
        actionFileOpen();
        return false;
    }

    QTextStream s(&f);

    if(!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Failed reading file", "<p>Could not read file " + fi.fileName() + "</p>");
        actionFileOpen();
        return false;
    }

    f.close();

    statusProgressBar->setMaximum(0);
    fileHandler->readFromFile(path);
    statusProgressBar->setMaximum(1);

    projectData->cleanup();

    wdgBusstops->refreshBusstopTable();
    wdgLines->refreshLineTable();
    wdgTours->refreshTourList();
    wdgSchedule->refreshDayTypes();

    undoStack->clear();
    knownFile = true;
    projectData->setFilePath(path);

    ui->statusbar->showMessage(path);

    return true;
}

bool MainWindow::saveFile(QString path) {
    QFile f(path);
    QFileInfo fi(path);
    QTextStream s(&f);


    if(!f.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, "Failed reading file", "<p>Could not write to file " + fi.fileName() + "</p>");
        actionFileSaveAs();
        return false;
    }

    f.close();

    projectData->cleanup();

    statusProgressBar->setMaximum(0);
    fileHandler->saveToFile(path);
    statusProgressBar->setMaximum(1);

    undoStack->setClean();
    knownFile = true;
    ui->statusbar->showMessage("File saved!", 5000);
    return true;
}

/*void MainWindow::reviceDataFromFileHandler() {

}*/

/*void MainWindow::on_actionBusstopExportList_triggered()
{
    QString plainText, csvText, htmlText;

    QFileInfo fi(this->projectFilePath);

    csvText = "Name;Important;used by lines\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All busstops of project " + fi.baseName() + "</title></head>\r\n\t<body style=\"font-family:sans-serif\">\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Used by lines</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->getBusstopCount(); i++) {
        Busstop *b = projectData->getBusstopAt(i);
        QString name = b->name();
        bool important = b->isImportant();
        QString importantStr = important ? "Yes" : "No";
        QStringList usedLinesStrList;
        QList<Line *> usedLines = projectData->getLinesAtBusstop(b);
        for(int i = 0; i < usedLines.count(); i++)
            usedLinesStrList << "[" + usedLines[i]->name() + "]";
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

void MainWindow::on_actionLineExportList_triggered()
{
    QString plainText, csvText, htmlText;

    QFileInfo fi(this->projectFilePath);

    csvText = "Name;Description\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All lines of project " + fi.baseName() + "</title></head>\r\n\t<body style=\"font-family:sans-serif\">\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Description</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->getLineCount(); i++) {
        Line *l = projectData->getLineAt(i);
        QString name = l->name();
        QColor color = l->getColor();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->getDescription();

        plainText += "[" + name + "] " + description + "\r\n";
        csvText += "\"" + name + "\";\"" + description + "\"\r\n";
        htmlText += "\t\t\t\t\t<tr><td style=\"color: " + contrastColor.name(QColor::HexRgb) + "; background-color: " + color.name(QColor::HexRgb) + "\"><b>" + name + "</b></td><td>" + description + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFolderName(fi.baseName());
    dlg.setFileName("all_lines");
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}*/


void MainWindow::on_actionCleanup_and_troubleshooting_triggered()
{
    Troubleshooter dlg;
    dlg.exec();
}


void MainWindow::on_actionView_As_Tree_triggered() {
    ProjectTreeViewer dlg(this, projectData);
    dlg.exec();
}

void MainWindow::on_actionHelpAbout_triggered() {
    QString text;
    text = "<h1>About ScheduleMaster 2</h1><table><tr><td><b>Version:</b></td><td>" + QString::number(global::primaryVersion) + "." + QString::number(global::secondaryVersion) + "." + QString::number(global::tertiaryVersion) + "</td></tr><tr><td><b>Release type:</b></td><td>Beta</td></tr></table>";
    QMessageBox msg;
    msg.setWindowTitle("ScheduleMaster 2 - About");
    msg.setText(text);
    //msg.setDetailedText("this is some detailed text");
    msg.setIconPixmap(QPixmap(":/main/appIcon/Icon_128px.ico"));
    msg.setStandardButtons(QMessageBox::Close);
    msg.exec();
}


void MainWindow::on_actionEditProjectSettings_triggered() {
    QList<DayType> dayTypes;
    for(int i = 0; i < projectData->projectSettings()->dayTypeCount(); i++)
        dayTypes << *projectData->projectSettings()->dayTypeAt(i);

    qDebug() << projectData->projectSettings()->icon().fileName();

    DlgProjectSettings *dlg = new DlgProjectSettings(this);
    dlg->setNames(projectData->projectSettings()->displayName(), projectData->projectSettings()->shortName());
    dlg->setIcon(projectData->projectSettings()->icon());
    dlg->setDayTypes(dayTypes);

    dlg->exec();

    if(dlg->result() != QDialog::Accepted)
        return;

    ProjectSettings newS;
    newS.setNames(dlg->displayName(), dlg->shortName());
    newS.setIcon(dlg->icon());
    newS.setDayTypes(dlg->dayTypes());

    undoStack->push(new cmdEditProjectSettings(projectData->projectSettings(), newS));

    wdgSchedule->refreshDayTypes();
}


void MainWindow::on_actionPublishTest_triggered() {
    /*QString fileName = QFileDialog::getSaveFileName(this, "", "D:/cpp_Projekte_Qt/ScheduleMaster2/build-ScheduleMaster2-Desktop_Qt_6_5_2_MinGW_64_bit-Release/Test.pdf", "Portable document format (*.pdf)");
    if(fileName.isEmpty())
        return;*/

    QString fileName = "D:/cpp_Projekte_Qt/ScheduleMaster2/build-ScheduleMaster2-Desktop_Qt_6_5_2_MinGW_64_bit-Release/Test.pdf";
    QFile f(fileName);
    if(!f.open(QFile::WriteOnly))
        return;

    QPdfWriter pdf(fileName);

    pdf.setResolution(254);
    pdf.setCreator("ScheduleMaster 2");
    pdf.setTitle("Test document");
    pdf.setPageLayout(QPageLayout(QPageSize::A4, QPageLayout::Landscape, QMargins(10, 10, 10, 10), QPageLayout::Millimeter));

    QPen defaultPen(Qt::black, 5);

    QBrush grayRow(Qt::lightGray);
    QBrush whiteRow(Qt::white);

    QFont headline("Helvetica", 16);
    headline.setBold(true);

    QFont normal("Helvetica", 11);

    defaultPen.setJoinStyle(Qt::MiterJoin);

    QPainter painter(&pdf);
    painter.setFont(headline);
    painter.setPen(defaultPen);

    QRect rect(0, 0, 2770, 75);

    // headline
    painter.drawRect(rect);
    painter.drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignLeft|Qt::AlignVCenter, "21/22 Kaisersee - Grundorf - Einsteindorf - Hertz - Nordspitze - Kirchdorf");
    painter.setFont(normal);
    painter.drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignRight|Qt::AlignBottom, "Gütlig ab: 07.11.2023");


    // table
    painter.setBackground(grayRow);

    for(int i = 0; i < 26; i++) {

        QRect rect(0, 200 + (i * 45), 2770, 45);
        if(i % 2 == 0) {
            painter.setBrush(grayRow);
        } else {
            painter.setBrush(whiteRow);
        }

        painter.setPen(Qt::NoPen);
        painter.drawRect(rect);

        painter.setPen(defaultPen);
        painter.drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignLeft|Qt::AlignVCenter, "Haltestellenname");
    }

    QRect tableBorder(0, 200, 2770, 45*26);
    painter.setPen(defaultPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(tableBorder.adjusted(- defaultPen.width() / 2, - defaultPen.width() / 2, defaultPen.width() / 2, defaultPen.width() / 2));


    QRect footer(0, 1900 - 45 + 25, 2770, 45);
    painter.drawText(footer, Qt::AlignRight|Qt::AlignBottom, "Seite 1 von 1");

    QDesktopServices::openUrl(QUrl(fileName));
}






























