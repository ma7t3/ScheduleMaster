#ifndef WDGWELCOME_H
#define WDGWELCOME_H

#include <QWidget>

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

    void updateRecentFilesList();

private slots:
    void on_pbToggleNews_clicked();

signals:
    void newProject();
    void openProject();
    void openPreferences();
    void quitApplication();

    void openProjectFromFile(const QString &);
private:
    Ui::WdgWelcome *ui;

    QStringList _lastUsedFiles;
    QAction *_recentFileOpen, *_recentFileRemove;
};

#endif // WDGWELCOME_H
