#ifndef WDGGLOBALSEARCHITEM_H
#define WDGGLOBALSEARCHITEM_H

#include "namespace.h"
#include "core/ActionServiceImpl.h"

#include <QWidget>

namespace Ui {
class WdgGlobalSearchItem;
}

class WdgGlobalSearchItem : public QWidget {
    Q_OBJECT

public:
    explicit WdgGlobalSearchItem(const SMA::ActionConfig &actionConfig, QWidget *parent = nullptr);
    ~WdgGlobalSearchItem();

    void setSelected(const bool &selected);

    void setAction(const SMA::ActionConfig &actionConfig);

private:
    Ui::WdgGlobalSearchItem *ui;
};

#endif // WDGGLOBALSEARCHITEM_H
