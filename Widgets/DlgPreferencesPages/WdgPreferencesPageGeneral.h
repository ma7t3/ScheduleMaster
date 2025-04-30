#ifndef WDGPREFERENCESPAGEGENERAL_H
#define WDGPREFERENCESPAGEGENERAL_H

#include <QWidget>
#include <QDesktopServices>

#include "Global/LocalConfig.h"
#include "ItemModels/LanguagesModel.h"

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

public slots:
    void setLanguageIndex(const int &index);
    void openLogfileLocation();
    void openLogfile();

signals:
    void languageIndexChanged(const int &index);

private slots:
    void on_cbLogfileMode_currentIndexChanged(int index);


private:
    Ui::WdgPreferencesPageGeneral *ui;

    LanguagesModel *_languagesModel;
};

#endif // WDGPREFERENCESPAGEGENERAL_H
