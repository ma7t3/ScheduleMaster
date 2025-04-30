#ifndef DOCKABSTRACT_H
#define DOCKABSTRACT_H

#include <QWidget>

class DockAbstract : public QWidget {
    Q_OBJECT
public:
    explicit DockAbstract(QWidget *parent = nullptr);

signals:
};

#endif // DOCKABSTRACT_H
