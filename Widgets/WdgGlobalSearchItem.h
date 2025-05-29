#ifndef WDGGLOBALSEARCHITEM_H
#define WDGGLOBALSEARCHITEM_H

#include "Global/ActionManager.h"

#include <QWidget>

namespace Ui {
class WdgGlobalSearchItem;
}

class WdgGlobalSearchItem : public QWidget {
    Q_OBJECT

public:
    explicit WdgGlobalSearchItem(const ActionConfig &actionConfig, QWidget *parent = nullptr);
    ~WdgGlobalSearchItem();

    void setSelected(const bool &selected);

    void setAction(const ActionConfig &actionConfig);

private:
    Ui::WdgGlobalSearchItem *ui;
};

#endif // WDGGLOBALSEARCHITEM_H
