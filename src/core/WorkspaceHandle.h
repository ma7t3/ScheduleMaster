#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include "IWorkspaceService.h"

class QAction;
class QMainWindow;

namespace ScheduleMaster::Core {

class WorkspaceHandle : public QObject {
    Q_OBJECT
public:
    explicit WorkspaceHandle(const WorkspaceConfig &config, QMainWindow *mainWindow, QObject *parent);

    QString id() const;
    QString name() const;
    int index() const;

    bool isActive() const;
    QAction *action() const;

public slots:
    void setActive(bool active);
    void apply();
    void saveWindowState();
    void clearWindowState();

protected:
    void setupAction();

    void resetWindowLayout();

private:
    QString _id, _name;
    int _index;
    QAction *_action;
    WorkspaceLayout _layout;
    QByteArray _lastWindowState;
    QMainWindow *_mainWindow;
};

}

#endif // WORKSPACE_H
