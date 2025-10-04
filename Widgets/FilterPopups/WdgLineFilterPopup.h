#ifndef WDGLINEFILTERPOPUP_H
#define WDGLINEFILTERPOPUP_H

#include "Widgets/FilterPopups/WdgFilterPopupContent.h"

namespace Ui {
class WdgLineFilterPopup;
}

class WdgLineFilterPopup : public WdgFilterPopupContent {
    Q_OBJECT

public:
    explicit WdgLineFilterPopup(QWidget *parent = nullptr);
    ~WdgLineFilterPopup();

    bool filterIsEnabled() const override;;

public slots:
    void reset() override;

signals:
    void filterChanged();

private:
    Ui::WdgLineFilterPopup *ui;
};

#endif // WDGLINEFILTERPOPUP_H
