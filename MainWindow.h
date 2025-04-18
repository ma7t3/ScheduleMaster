#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>
#include <QDesktopServices>

#include "Global/WorkspaceHandler.h"
#include "ProjectData/ProjectData.h"
#include "Widgets/WdgWelcome.h"

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
    void loadWorkspaces();
    void initToolbars();
    void initDockWidgets();

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


private slots:
    void on_actionDebugGeneralTestAction_triggered();

    void setUiFont(const QString &fontFamily);

    void on_actionDebugSimulateCrash_triggered();

private:
    Ui::MainWindow *ui;

    WorkspaceHandler *_workspaceHandler;

    ProjectData *_projectData;

    QAction *_undoAction, *_redoAction;

    QToolBar *_toolbarGeneral, *_toolbarDocks, *_toolbarWorkspaces;

    QDockWidget *_dwWelcome;

    WdgWelcome *_wdgWelcome;
};
#endif // MAINWINDOW_H
