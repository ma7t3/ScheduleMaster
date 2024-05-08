#include "WdgChangelogBrowser.h"

#include "localconfig.h"

WdgChangelogBrowser::WdgChangelogBrowser(QWidget *parent) :
    QTextBrowser(parent) {
}

void WdgChangelogBrowser::setVersion(AppInfo::AppVersion *v) {
    clear();
    QLocale locale(LocalConfig::language() == LocalConfig::German ? QLocale::German : QLocale::English);

    if(!v)
        return;
    QString text = tr("<h1>Changelog (%1)</h1><hr><table><tr><td><b>Released on:</b></td><td>%2</td></tr><tr><td><b>Build ID:</b></td><td>%3</td></tr></table><hr><h2>Overview</h2>")
                       .arg(v->name())
                       .arg(v->releaseDate().toString(locale.dateFormat(QLocale::LongFormat)))
                       .arg(v->buildID());
    text += v->changelogCompactHtml();
    text += "<hr>";
    text += tr("<h2>Details</h2>");
    text += v->changelogDetailedHtml(3);
    setText(text);
}
