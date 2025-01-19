#ifndef WDGWELCOME_H
#define WDGWELCOME_H

#include <QWidget>
#include <QListWidgetItem>

#include "WdgAbstract.h"

namespace Ui {
class WdgWelcome;
}

class WdgWelcome : public WdgAbstract
{
    Q_OBJECT

public:
    explicit WdgWelcome(QWidget *parent = nullptr);
    ~WdgWelcome();

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
    void openPreferences();
    void quitApplication();

    void openProjectFromFile(const QString &);
    void removeProjectFromList(const QString &);

private:
    Ui::WdgWelcome *ui;

    QAction *_recentFileOpen, *_recentFileOpenLocation, *_recentFileRemove;
};

#endif // WDGWELCOME_H
