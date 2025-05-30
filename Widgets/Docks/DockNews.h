#ifndef DOCKNEWS_H
#define DOCKNEWS_H

#include <QWidget>

#include "DockAbstract.h"

namespace Ui {
class DockNews;
}

class DockNews : public DockAbstract {
    Q_OBJECT

public:
    explicit DockNews(QWidget *parent = nullptr);
    ~DockNews();

protected slots:
    void updateIcon();

private:
    Ui::DockNews *ui;
};

#endif // DOCKNEWS_H
