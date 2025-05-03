#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>
#include <QDesktopServices>
#include <QProgressDialog>

#include "Global/ProjectFileHandler.h"
#include "Global/WorkspaceHandler.h"
#include "ProjectData/ProjectData.h"
#include "Widgets/Docks/DockWelcome.h"

#include "Global/DockController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void connectToInterface();

    void loadDocks();
    void initToolbars();

    void updateRecentProjectsList();

protected slots:
    void showCrashWarning();

    void newProject();
    void openProject();
    void openProjectFromFile(const QString &filePath);
    void saveProject();
    void saveProjectAs();
    void saveProjectToFile(const QString &filePath);
    void closeProject();
    void quitApplication();

    void removeProjectFromRecentList(const QString &filePath);

    void openPlugins();
    void openPreferences();
    void openConfiguration();
    void openProjectSettings();

    void onDockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction);

    void createFileHandlerProgressDialog(const QString &title);
    void onFileHandlerProgressStepChanged(const QString &text);
    void onFileHandlerProgressMaximum(const int &maximum);
    void onFileHandlerProgressUpdate(const int &current);
    void onFileHandlerFinished();

    void on_actionDebugGeneralTestAction_triggered();
    void on_actionDebugSimulateCrash_triggered();

private:
    Ui::MainWindow *ui;

    QProgressDialog *_fileHandlerProgressDialog;

    DockController *_dockHandler;
    WorkspaceHandler *_workspaceHandler;

    ProjectData *_projectData;
    ProjectFileHandler *_fileHandler;

    QAction *_undoAction, *_redoAction;

    QToolBar *_toolbarGeneral, *_toolbarDocks, *_toolbarWorkspaces;
};
#endif // MAINWINDOW_H
