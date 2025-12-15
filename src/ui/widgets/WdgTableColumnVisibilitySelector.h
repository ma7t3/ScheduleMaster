#ifndef WDGTABLECOLUMNVISIBILITYSELECTOR_H
#define WDGTABLECOLUMNVISIBILITYSELECTOR_H

#include <QDialog>


class QAbstractItemModel;
class QTableView;
class QAbstractButton;
class QListWidgetItem;
class QToolButton;

namespace Ui {
class WdgTableColumnVisibilitySelector;
}

class WdgTableColumnVisibilitySelector : public QDialog {
    Q_OBJECT

public:
    explicit WdgTableColumnVisibilitySelector(QTableView *view, QToolButton *triggerButton, QWidget *parent = nullptr);
    ~WdgTableColumnVisibilitySelector();

    QMenu *menu();

protected:
    void reset();

    void setColumnVisible(const int &index, const bool &show);

    bool columnIsMandatory(const int &index);

private slots:
    void onButtonBoxClicked(QAbstractButton *button);
    void onItemChanged(QListWidgetItem *item);

    void restoreDefaults();

private:
    Ui::WdgTableColumnVisibilitySelector *ui;

    QTableView *_view;
    QAbstractItemModel *_model;
    QMenu *_menu;
    QAction *_restoreDefaultsAction, *_showInWindowAction;
    QList<QAction *> _actions;
};

#endif // WDGTABLECOLUMNVISIBILITYSELECTOR_H
