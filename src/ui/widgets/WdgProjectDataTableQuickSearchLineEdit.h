#ifndef WDGPROJECTDATATABLEQUICKSEARCHLINEEDIT_H
#define WDGPROJECTDATATABLEQUICKSEARCHLINEEDIT_H

#include <QLineEdit>

class WdgProjectDataTableQuickSearchLineEdit : public QLineEdit {
    Q_OBJECT
public:
    WdgProjectDataTableQuickSearchLineEdit(QWidget *parent = nullptr);

    void setFocusAction(QAction *focusAction);

private:
    QAction *_focusAction;
};

#endif // WDGPROJECTDATATABLEQUICKSEARCHLINEEDIT_H
