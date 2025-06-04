#ifndef DOCKCONTROLLER_H
#define DOCKCONTROLLER_H

#include "Global/DockManager.h"

#include <QObject>

class QDockWidget;
class QAction;

class DockController : public QObject {
    Q_OBJECT
public:
    explicit DockController(QObject *parent = nullptr);
    void loadStandardDocks();

    void addDock(const DockConfig &dock, QWidget *contentWidget);

    static QMap<QString, QDockWidget *> docks();
    QDockWidget *dock(const QString &id) const;

signals:
    void dockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction);

private:
    /// All available docks
    static inline QMap<QString, QDockWidget *> _docks;

    /// All dock toggle actions
    QMap<QString, QAction *> _dockToggleActions;
};

#endif // DOCKCONTROLLER_H
