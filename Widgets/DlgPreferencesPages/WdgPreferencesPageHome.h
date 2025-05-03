#ifndef WDGPREFERENCESPAGEHOME_H
#define WDGPREFERENCESPAGEHOME_H

#include <QWidget>
#include <QToolButton>

#include "Widgets/DlgPreferencesPages/WdgPreferencesPage.h"

class LanguagesModel;
class StylesModel;

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
    void setStyleIndex(const int &index);

signals:
    void languageIndexChanged(const int &index);
    void styleIndexChanged(const int &index);

    void openPageRequested(const QString &pageID);
    void openLogfileLocationRequested();
    void openConfigEditorRequested();

private slots:
    void on_clbKeyboardShortcuts_clicked();
    void on_clbConfigEditor_clicked();
    void on_clbUpdateSettings_clicked();
    void on_clbPluginManager_clicked();

    void on_clbOpenLogfileLocation_clicked();

    void on_clbAppearance_clicked();

private:
    Ui::WdgPreferencesPageHome *ui;

    LanguagesModel *_languagesModel;
    StylesModel *_stylesModel;
};

#endif // WDGPREFERENCESPAGEHOME_H
