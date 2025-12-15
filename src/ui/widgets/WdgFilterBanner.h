#ifndef WDGFILTERBANNER_H
#define WDGFILTERBANNER_H

#include <QWidget>

namespace Ui {
class WdgFilterBanner;
}

class WdgFilterBanner : public QWidget {
    Q_OBJECT

public:
    explicit WdgFilterBanner(QWidget *parent = nullptr);
    ~WdgFilterBanner();

protected:
    void paintEvent(QPaintEvent *) override;

protected slots:
    void close();
    void dontShowAgain();

signals:
    void clearFilterRequested();
    void closed();

private:
    Ui::WdgFilterBanner *ui;
};

#endif // WDGFILTERBANNER_H
