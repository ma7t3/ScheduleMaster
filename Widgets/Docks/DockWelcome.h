#ifndef DOCKWELCOME_H
#define DOCKWELCOME_H

#include <QWidget>
#include <QListWidgetItem>

#include "DockAbstract.h"

namespace Ui {
class DockWelcome;
}

class DockWelcome : public DockAbstract {
    Q_OBJECT

public:
    explicit DockWelcome(QWidget *parent = nullptr);
    ~DockWelcome();

    void updateRecentProjectsList();

private slots:
    void on_pbToggleNews_clicked();

    void onRecentFileOpen();
    void onRecentFileOpenLocation();
    void onRecentFileRemove();

    void on_lwRecentProjects_itemDoubleClicked(QListWidgetItem *item);

private:
    QString currentRecentFilePath() const;

signals:
    void newProject();
    void openProject();
    void openPlugins();
    void openPreferences();
    void quitApplication();

    void openProjectFromFile(const QString &);
    void removeProjectFromList(const QString &);

private:
    Ui::DockWelcome *ui;

    QAction *_recentFileOpen, *_recentFileOpenLocation, *_recentFileRemove;
};

#endif // DOCKWELCOME_H
