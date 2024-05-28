#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QDockWidget>
#include <QUndoStack>
#include <QToolBar>

#include "ProjectData/FileHandler.h"

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
#include "Widgets/WdgFootnotes.h"

#include "Dialogs/DlgStartupdialog.h"
#include "Dialogs/DlgProgressLogger.h"
#include "Dialogs/DlgPdfExporter.h"

#include "ProjectData/projectdata.h"

#include "Commands/CmdGeneral.h"


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

    ProjectData *projectData() const;
    QUndoStack *undoStack() const;

private slots:

    void startupDialogHandler();

    bool actionFileNew();
    bool actionFileOpen(QString = "");
    bool actionFileSave();
    bool actionFileSaveAs();
    bool actionFileClose();
    bool actionQuit();

    void refreshLastUsedFiles();

    void refreshUndo();
    void refreshRedo();
    void refreshAfterUndoRedo(CmdType t);

    void actionWorkspaceTrackLayout();
    void actionWorkspaceBusstopSchedule();
    void actionWorkspaceScheduling();
    void actionWorkspaceTourPlanning();
    void actionWorkspacePublish();

    void actionOpenBusstopSchedule(Busstop *, QList<Route *>, DayType *);
    void actionOpenTour(Tour *);

    void setSaved(bool b);
    bool openFile(QString);
    bool saveFile(QString);
    void handleFileHandlerFileError(const QString &filePath, const QString errorString);
    void handleFileHandlerFileOldVersionWarning(AppInfo::AppVersion *);
    void handleFileHandlerFileUnkownVersionWarning();
    void handleFileHandlerResult();

    void on_actionCleanup_and_troubleshooting_triggered();
    void on_actionView_As_Tree_triggered();
    void on_actionHelpAbout_triggered();
    void on_actionEditProjectSettings_triggered();
    void on_actionPublishAll_triggered();
    void on_actionEditPreferences_triggered();
    void on_actionPublishManageFootnotes_triggered();
    void on_actionHelpManual_triggered();
    void on_actionViewToolbarGeneral_triggered();
    void on_actionViewToolbarDocks_triggered();
    void on_actionViewToolbarWorkspaces_triggered();
    void on_actionFileImportOmsiSchedule_triggered();
    void on_actionFileExportRoutesWithProfilesCsv_triggered();
    void on_actionHelpAboutQt_triggered();
    void on_actionHelpChangelog_triggered();

signals:
    void projectSettingsChanged();
    void busstopsChanged();
    void linesChanged();
    void lineDirectionsChanged();
    void routesChanged();
    void timeProfilesChanged();
    void tripsChanged();
    void toursChanged();

private:
    Ui::MainWindow *ui;

    ProjectData *_projectData;

    FileHandler *fileHandler;
    StartupDialog *startupDialog;
    DlgProgressLogger *progressLogger;
    DlgPdfExporter *pdfExporter;

    QStringList _lastUsedFileNames;
    QList<QAction *> _lastUsedFileActions;

    WdgUndoView *wdgUndoView = new WdgUndoView(this);
    WdgBusstops *wdgBusstops = new WdgBusstops(this);
    WdgLines *wdgLines = new WdgLines(this);
    WdgRoutes *wdgRoutes = new WdgRoutes(this);
    WdgSchedule *wdgSchedule = new WdgSchedule(this);
    WdgTripEditor *wdgTripEditor = new WdgTripEditor(this, _projectData, undoStack());
    WdgTours *wdgTours = new WdgTours(this, _projectData, undoStack());
    WdgTourEditor *wdgTourEditor = new WdgTourEditor(this, _projectData, undoStack());
    WdgBusstopSchedule *wdgBusstopSchedule = new WdgBusstopSchedule(this, _projectData);
    WdgPublishedLines *wdgPublishedLines = new WdgPublishedLines(this, _projectData, undoStack());
    WdgFootnotes *wdgFootnotes = new WdgFootnotes(this, _projectData, undoStack());

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
    QDockWidget *dwFootnotes = new QDockWidget(tr("Footnotes"), this);

    QToolBar *tbGeneral, *tbDocks, *tbWorkspaces;
    QList<QAction *> hourBreakActions;
    QAction *undoAction, *redoAction;

    bool knownFile;
};
#endif // MAINWINDOW_H
