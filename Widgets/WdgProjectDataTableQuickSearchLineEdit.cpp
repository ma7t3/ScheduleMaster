#include "WdgProjectDataTableQuickSearchLineEdit.h"

#include "Global/ActionController.h"

WdgProjectDataTableQuickSearchLineEdit::WdgProjectDataTableQuickSearchLineEdit(QWidget *parent) :
    QLineEdit(parent), _focusAction(nullptr) {
    setPlaceholderText(tr("Search..."));
    setClearButtonEnabled(true);

    QAction *clearSearchAction = addAction("");
    clearSearchAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ActionController::add(clearSearchAction, "projectDataTable.search.clear", ActionController::ShortcutComponent);
    connect(clearSearchAction, &QAction::triggered, this, &QLineEdit::clear);
}

void WdgProjectDataTableQuickSearchLineEdit::setFocusAction(QAction *focusAction) {
    if(_focusAction)
        _focusAction->disconnect(this);

    _focusAction = focusAction;
    connect(_focusAction, &QAction::triggered, this, [this]() { setFocus(); });
}
