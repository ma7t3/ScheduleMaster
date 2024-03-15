#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QDockWidget>
#include <QUndoStack>
#include <QToolBar>

#include "Dialogs/DlgFilehandler.h"

#include "Widgets/wdgbusstops.h"
#include "Widgets/wdglines.h"
#include "Widgets/wdgroutes.h"
#include "Widgets/wdgschedule.h"
#include "Widgets/wdgtripeditor.h"
#include "Widgets/wdgtours.h"
#include "Widgets/wdgtoureditor.h"
#include "Widgets/wdgbusstopschedule.h"
#include "Widgets/wdgundoview.h"
#include "Widgets/Publications/WdgPublishedLines.h"

#include "Dialogs/DlgStartupdialog.h"
#include "Dialogs/DlgFilehandler.h"
#include "Dialogs/DlgPdfExporter.h"

#include "ProjectData/projectdata.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event) override;

private slots:

    bool actionFileNew();
    bool actionFileOpen();
    bool actionFileSave();
    bool actionFileSaveAs();
    bool actionFileClose();
    bool actionQuit();

    void actionEditUndo();
    void actionEditRedo();

    void actionWorkspaceTrackLayout();
    void actionWorkspaceBusstopSchedule();
    void actionWorkspaceScheduling();
    void actionWorkspaceTourPlanning();
    void actionWorkspacePublish();

    void actionOpenBusstopSchedule(Busstop *, QList<Route *>, DayType *);
    void actionOpenTour(Tour *);

    void setUndoEnabled(bool);
    void setRedoEnabled(bool);

    void setSaved(bool b);
    bool openFile(QString);
    bool saveFile(QString);

    //void reviceDataFromFileHandler();

    /*
     * ////////////////////////////////////////////////////////////////////////////////////////////
    */


    void on_actionCleanup_and_troubleshooting_triggered();

    void on_actionView_As_Tree_triggered();


    void on_actionHelpAbout_triggered();


    void on_actionEditProjectSettings_triggered();

    //void on_actionPublishTest_triggered();

    void on_actionPublishAll_triggered();

    void on_actionEditPreferences_triggered();

    void on_actionPublishManageFootnotes_triggered();

    void on_actionHelpManual_triggered();

    void on_actionViewToolbarGeneral_triggered();
    void on_actionViewToolbarDocks_triggered();
    void on_actionViewToolbarWorkspaces_triggered();

private:
    Ui::MainWindow *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    StartupDialog *startupDialog;
    DlgFileHandler *fileHandler;
    DlgPdfExporter *pdfExporter;

    WdgUndoView *wdgUndoView = new WdgUndoView(this, undoStack);
    WdgBusstops *wdgBusstops = new WdgBusstops(this, projectData, undoStack);
    WdgLines *wdgLines = new WdgLines(this, projectData, undoStack);
    WdgRoutes *wdgRoutes = new WdgRoutes(this, projectData, undoStack);
    WdgSchedule *wdgSchedule = new WdgSchedule(this, projectData, undoStack);
    WdgTripEditor *wdgTripEditor = new WdgTripEditor(this, projectData, undoStack);
    WdgTours *wdgTours = new WdgTours(this, projectData, undoStack);
    WdgTourEditor *wdgTourEditor = new WdgTourEditor(this, projectData, undoStack);
    WdgBusstopSchedule *wdgBusstopSchedule = new WdgBusstopSchedule(this, projectData);
    WdgPublishedLines *wdgPublishedLines = new WdgPublishedLines(this, projectData, undoStack);

    QDockWidget *dwUndoView = new QDockWidget(tr("Undo View"), this);
    QDockWidget *dwBusstops = new QDockWidget(tr("Busstops"), this);
    QDockWidget *dwLines = new QDockWidget(tr("Lines"), this);
    QDockWidget *dwRoutes = new QDockWidget(tr("Routes"), this);
    QDockWidget *dwSchedule = new QDockWidget(tr("Schedule"), this);
    QDockWidget *dwTripEditor = new QDockWidget(tr("Current trips"), this);
    QDockWidget *dwTours = new QDockWidget(tr("Tours"), this);
    QDockWidget *dwTourEditor = new QDockWidget(tr("Current tour"), this);
    QDockWidget *dwBusstopSchedule = new QDockWidget(tr("Busstop schedule"), this);
    QDockWidget *dwPublishedLines = new QDockWidget(tr("Published lines"), this);

    QToolBar *tbGeneral, *tbDocks, *tbWorkspaces;

    bool saved;
    bool knownFile;
    //QString projectFilePath;
};
#endif // MAINWINDOW_H
