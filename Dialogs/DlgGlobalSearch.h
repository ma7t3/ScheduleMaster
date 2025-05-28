#ifndef DLGGLOBALSEARCH_H
#define DLGGLOBALSEARCH_H

#include <QWidget>

class QListWidgetItem;

namespace Ui {
    class DlgGlobalSearch;
}

class DlgGlobalSearch : public QWidget {
    Q_OBJECT

public:
    explicit DlgGlobalSearch(QWidget *parent = nullptr);
    ~DlgGlobalSearch();

public slots:
    void open();

protected slots:
    void updateResults();

    bool eventFilter(QObject* obj, QEvent* event) override;

    void navigateList(int direction);
    void activateSelectedItem();
    void onItemActivated(QListWidgetItem *item);

private:
    Ui::DlgGlobalSearch *ui;
};

#endif // DLGGLOBALSEARCH_H
