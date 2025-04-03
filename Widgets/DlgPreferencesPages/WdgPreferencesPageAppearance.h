#ifndef WDGPREFERENCESPAGEAPPEARANCE_H
#define WDGPREFERENCESPAGEAPPEARANCE_H

#include <QWidget>

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPageAppearance;
}

class WdgPreferencesPageAppearance : public WdgPreferencesPage
{
    Q_OBJECT

public:
    explicit WdgPreferencesPageAppearance(QWidget *parent);
    ~WdgPreferencesPageAppearance();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;


private:
    Ui::WdgPreferencesPageAppearance *ui;
};

#endif // WDGPREFERENCESPAGEAPPEARANCE_H
