#ifndef WDGPREFERENCESPAGEHOME_H
#define WDGPREFERENCESPAGEHOME_H

#include <QWidget>
#include <QToolButton>

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"
#include "ItemModels/LanguagesModel.h"

namespace Ui {
class WdgPreferencesPageHome;
}

class WdgPreferencesPageHome : public WdgPreferencesPage {
    Q_OBJECT

public:
    explicit WdgPreferencesPageHome(QWidget *parent = nullptr);
    ~WdgPreferencesPageHome();

    virtual void reloadPreferences() override;
    virtual void savePreferences() override;
    virtual void discardPreviewPreferences() override;

    virtual QString id() override;
    virtual QString name() override;
    virtual QIcon icon() override;

public slots:
    void setLanguageIndex(const int &index);

signals:
    void languageIndexChanged(const int &index);

signals:
    void openPageRequested(const QString &pageID);
    void openConfigEditorRequested();

private slots:
    void on_clbKeyboardShortcuts_clicked();
    void on_clbConfigEditor_clicked();
    void on_clbUpdateSettings_clicked();
    void on_clbPluginManager_clicked();

private:
    Ui::WdgPreferencesPageHome *ui;

    LanguagesModel *_languagesModel;
};

#endif // WDGPREFERENCESPAGEHOME_H
