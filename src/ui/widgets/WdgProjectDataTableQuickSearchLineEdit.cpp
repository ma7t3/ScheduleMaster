#include "WdgProjectDataTableQuickSearchLineEdit.h"

#include "namespace.h"
#include "core/ActionServiceImpl.h"

WdgProjectDataTableQuickSearchLineEdit::WdgProjectDataTableQuickSearchLineEdit(QWidget *parent) :
    QLineEdit(parent), _focusAction(nullptr) {
    setPlaceholderText(tr("Search..."));
    setClearButtonEnabled(true);

    QAction *clearSearchAction = addAction("");
    clearSearchAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    SM::ActionServiceImpl::instance()->addAction(clearSearchAction, "projectDataTable.search.clear", SM::ActionServiceImpl::instance()->ShortcutComponent);
    connect(clearSearchAction, &QAction::triggered, this, &QLineEdit::clear);
}

void WdgProjectDataTableQuickSearchLineEdit::setFocusAction(QAction *focusAction) {
    if(_focusAction)
        _focusAction->disconnect(this);

    _focusAction = focusAction;
    connect(_focusAction, &QAction::triggered, this, [this]() { setFocus(); });
}
