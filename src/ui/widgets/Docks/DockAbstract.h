#ifndef DOCKABSTRACT_H
#define DOCKABSTRACT_H

#include <QWidget>
#include <QMenu>

class DockAbstract : public QWidget {
    Q_OBJECT
public:
    explicit DockAbstract(QWidget *parent = nullptr) : QWidget(parent), _globalMenu(new QMenu(this)) {}

    QMenu *globalMenu() const { return _globalMenu; }

protected:
    QAction *setupAction(const Qt::ShortcutContext shortcutContext = Qt::WidgetWithChildrenShortcut,
                         QWidget *parent = nullptr) {
        if(!parent)
            parent = this;

        QAction *action = parent->addAction("");
        action->setShortcutContext(shortcutContext);
        return action;
    }

signals:

private:
    QMenu *_globalMenu;
};

#endif // DOCKABSTRACT_H
