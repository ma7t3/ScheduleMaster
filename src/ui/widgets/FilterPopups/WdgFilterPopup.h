#ifndef WDGFILTERPOPUP_H
#define WDGFILTERPOPUP_H

#include <QWidget>
#include <QAbstractButton>

#include "src/ui/widgets/FilterPopups/WdgFilterPopupContent.h"

class QHBoxLayout;

namespace Ui {
class WdgFilterPopup;
}

class WdgFilterPopup : public QWidget {
    Q_OBJECT

public:
    explicit WdgFilterPopup(QWidget *parent = nullptr);
    ~WdgFilterPopup();

    void setContentWidget(WdgFilterPopupContent *widget);

    void showTool(QPoint pos);
    void showPopup(QPoint pos);

private:
    Ui::WdgFilterPopup *ui;
    bool _contentWidgetSet;
};

#endif // WDGFILTERPOPUP_H
