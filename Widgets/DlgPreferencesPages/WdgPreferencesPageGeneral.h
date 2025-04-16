#ifndef WDGPREFERENCESPAGEGENERAL_H
#define WDGPREFERENCESPAGEGENERAL_H

#include <QWidget>
#include <QDesktopServices>

#include "Global/LocalConfig.h"

#include "WdgPreferencesPage.h"

namespace Ui {
class WdgPreferencesPageGeneral;
}

class WdgPreferencesPageGeneral : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageGeneral(QWidget *parent);
    ~WdgPreferencesPageGeneral();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

private slots:
    void on_cbLogfileMode_currentIndexChanged(int index);

    void on_pbLogfileLocation_clicked();

private:
    Ui::WdgPreferencesPageGeneral *ui;
};

#endif // WDGPREFERENCESPAGEGENERAL_H
