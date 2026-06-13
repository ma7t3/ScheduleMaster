#ifndef WDGBUSSTOPSFILTERPOPUP_H
#define WDGBUSSTOPSFILTERPOPUP_H

#include "ui/widgets/FilterPopups/WdgFilterPopupContent.h"
#include "projectdata/Busstop.h"

#include "projectdata/model/LineSelectionListModel.h"
#include "projectdata/model/LineSelectionListProxyModel.h"

namespace Ui {
class WdgBusstopsFilterPopup;
}

class WdgBusstopsFilterPopup : public WdgFilterPopupContent {
    Q_OBJECT

public:
    explicit WdgBusstopsFilterPopup(QWidget *parent = nullptr);
    ~WdgBusstopsFilterPopup();

    bool filterIsEnabled() const override;

    BusstopFlags filterFlags() const;
    PDISet<Line> filterLines() const;

public slots:
    void onSomethingChanged();
    void reset() override;

private:
    Ui::WdgBusstopsFilterPopup *ui;

    LineSelectionListModel *_lineModel;
    LineSelectionListProxyModel *_lineProxyModel;
};

#endif // WDGBUSSTOPSFILTERPOPUP_H
