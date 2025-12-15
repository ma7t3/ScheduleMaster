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

    void paintEvent(QPaintEvent *e) override;

    void setName(const QString &);
    void setPath(const QString &);
    void setLastUsed(const QDateTime &);

    void setFileMissing();

    QString path() const;

signals:
    void open(const QString &);
    void removeFromList(const QString &);

protected:
    void setupLastUsedLabel();
    void updateIcon();

private:
    Ui::WdgWelcomeRecentProjectEntry *ui;

    bool _fileExists;
    QIcon _icon;
};

#endif // WDGWELCOMERECENTPROJECTENTRY_H
