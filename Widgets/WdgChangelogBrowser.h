#ifndef WDGCHANGELOGBROWSER_H
#define WDGCHANGELOGBROWSER_H

#include <QWidget>
#include <QTextBrowser>

#include "AppInfo.h"

class WdgChangelogBrowser : public QTextBrowser {
    Q_OBJECT

public:
    explicit WdgChangelogBrowser(QWidget *parent = nullptr);

    void setVersion(AppInfo::AppVersion *);
};

#endif // WDGCHANGELOGBROWSER_H
