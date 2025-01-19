#ifndef WDGWELCOMERECENTPROJECTENTRY_H
#define WDGWELCOMERECENTPROJECTENTRY_H

#include <QWidget>

namespace Ui {
class WdgWelcomeRecentProjectEntry;
}

class WdgWelcomeRecentProjectEntry : public QWidget {
    Q_OBJECT

public:
    explicit WdgWelcomeRecentProjectEntry(QWidget *parent = nullptr);
    ~WdgWelcomeRecentProjectEntry();

    void setIcon(const QPixmap &);
    void setName(const QString &);
    void setPath(const QString &);
    void setLastUsed(const QDateTime &);

    void setFileMissing();

private:
    Ui::WdgWelcomeRecentProjectEntry *ui;
};

#endif // WDGWELCOMERECENTPROJECTENTRY_H
