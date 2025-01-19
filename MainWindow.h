#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>

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
    void initToolbars();
    void initDockWidgets();

protected slots:
    void newProject();
    void openProject();
    void openProjectFromFile(const QString &filePath);
    void saveProject();
    void saveProjectAs();
    void saveProjectToFile(const QString &filePath);
    void closeProject();
    void quitApplication();

    void openPreferences();
    void openProjectSettings();


private:
    Ui::MainWindow *ui;

    QToolBar *_toolbarGeneral, *_toolbarDocks, *_toolbarWorkspaces;

    QDockWidget *_dwWelcome;

    WdgWelcome *_wdgWelcome;
};
#endif // MAINWINDOW_H
