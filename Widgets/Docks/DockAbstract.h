#ifndef DOCKABSTRACT_H
#define DOCKABSTRACT_H

#include <QWidget>
#include <QMenu>

class DockAbstract : public QWidget {
    Q_OBJECT
public:
    explicit DockAbstract(QWidget *parent = nullptr) : QWidget(parent), _globalMenu(new QMenu(this)) {}

    QMenu *globalMenu() const { return _globalMenu; }

signals:

private:
    QMenu *_globalMenu;
};

#endif // DOCKABSTRACT_H
