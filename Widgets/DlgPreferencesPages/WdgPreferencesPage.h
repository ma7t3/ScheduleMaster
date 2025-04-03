#ifndef WDGPREFERENCESPAGE_H
#define WDGPREFERENCESPAGE_H

#include <QWidget>

class WdgPreferencesPage : public QWidget
{
    Q_OBJECT
public:
    explicit WdgPreferencesPage(QWidget *parent);

    virtual void reloadPreferences() = 0;
    virtual void savePreferences() = 0;

signals:
};

#endif // WDGPREFERENCESPAGE_H
