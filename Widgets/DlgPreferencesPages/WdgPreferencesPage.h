#ifndef WDGPREFERENCESPAGE_H
#define WDGPREFERENCESPAGE_H

#include <QWidget>

class WdgPreferencesPage : public QWidget {
    Q_OBJECT
public:
    explicit WdgPreferencesPage(QWidget *parent);

    virtual void reloadPreferences();
    virtual void savePreferences();
    virtual void discardPreviewPreferences();
    bool unsavedChanges();

    virtual QString id() = 0;
    virtual QString name() = 0;
    virtual QIcon icon() = 0;

protected slots:
    void setUnsaved();

signals:

private:
    bool _unsavedChanges;
};

#endif // WDGPREFERENCESPAGE_H
