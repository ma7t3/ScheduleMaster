#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DlgGlobalSearch;
class QProgressDialog;
class DockController;
class WorkspaceHandler;
class ProjectData;
class ProjectFileHandler;

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

    void showEvent(QShowEvent *event) override;

protected slots:
    void showCrashWarning();

    void newProject();
    void openProject();
    void openProjectFromFile(const QString &filePath);
    void showRecentFilesMenu();
    void saveProject();
    void saveProjectAs();
    void saveProjectToFile(const QString &filePath);
    void closeProject();
    void closeProjectBackToHome();
    void quitApplication();

    void removeProjectFromRecentList(const QString &filePath);

    void showDockMenu();
    void showWorkspaceMenu();

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

    void showGlobalSearch();

private:
    Ui::MainWindow *ui;

    QProgressDialog *_fileHandlerProgressDialog;

    DockController *_dockController;
    WorkspaceHandler *_workspaceHandler;

    ProjectData *_projectData;
    ProjectFileHandler *_fileHandler;

    QAction *_undoAction, *_redoAction, *_openProjectFromFileInRecentFilesMenuAction;

    QToolBar *_toolbarGeneral, *_toolbarDocks, *_toolbarWorkspaces;

    DlgGlobalSearch *_globalSearch;
};
#endif // MAINWINDOW_H
