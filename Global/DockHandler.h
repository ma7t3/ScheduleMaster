#ifndef DOCKHANDLER_H
#define DOCKHANDLER_H

#include <QObject>
#include <QDockWidget>

#include "GlobalConfig.h"

#include "Widgets/Docks/DockWelcome.h"

#include "Global/ActionShortcutMapper.h"

#include "Global/DocksManager.h"

class DockHandler : public QObject {
    Q_OBJECT
public:
    explicit DockHandler(QObject *parent = nullptr);
    void loadStandardDocks();

    void addDock(const DockConfig &dock, QWidget *contentWidget);

signals:
    void dockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction);

private:
    /// All available docks
    QMap<QString, QDockWidget *> _docks;

    /// All dock toggle actions
    QMap<QString, QAction *> _dockToggleActions;
};

#endif // DOCKHANDLER_H
