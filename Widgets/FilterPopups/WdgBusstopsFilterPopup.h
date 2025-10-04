#ifndef WDGBUSSTOPSFILTERPOPUP_H
#define WDGBUSSTOPSFILTERPOPUP_H

#include "Widgets/FilterPopups/WdgFilterPopupContent.h"
#include "ProjectData/Busstop.h"

#include "ProjectDataModels/LineSelectionListModel.h"
#include "ProjectDataModels/LineSelectionListProxyModel.h"

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
