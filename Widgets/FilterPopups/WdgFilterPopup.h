#ifndef WDGFILTERPOPUP_H
#define WDGFILTERPOPUP_H

#include <QWidget>
#include <QAbstractButton>

#include "Widgets/FilterPopups/WdgFilterPopupContent.h"

namespace Ui {
class WdgFilterPopup;
}

class WdgFilterPopup : public QWidget {
    Q_OBJECT

public:
    explicit WdgFilterPopup(QWidget *parent = nullptr);
    ~WdgFilterPopup();

    void setContentWidget(WdgFilterPopupContent *widget);

private:
    Ui::WdgFilterPopup *ui;
    bool _contentWidgetSet;
};

#endif // WDGFILTERPOPUP_H
