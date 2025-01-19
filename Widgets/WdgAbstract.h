#ifndef WDGABSTRACT_H
#define WDGABSTRACT_H

#include <QWidget>

class WdgAbstract : public QWidget {
    Q_OBJECT
public:
    explicit WdgAbstract(QWidget *parent = nullptr);

signals:
};

#endif // WDGABSTRACT_H
