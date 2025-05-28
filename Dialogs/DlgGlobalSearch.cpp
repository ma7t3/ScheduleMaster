#include "DlgGlobalSearch.h"
#include "ui_DlgGlobalSearch.h"

#include "Global/ActionManager.h"
#include "Global/ActionController.h"
#include "Global/IconController.h"

#include <QKeyEvent>

DlgGlobalSearch::DlgGlobalSearch(QWidget *parent) : QWidget(parent),
    ui(new Ui::DlgGlobalSearch) {
    ui->setupUi(this);
    setWindowFlag(Qt::Popup);

    ui->leSearch->installEventFilter(this);

    connect(ui->leSearch, &QLineEdit::textChanged, this, &DlgGlobalSearch::updateResults);
    connect(ui->listWidget, &QListWidget::itemActivated, this, &DlgGlobalSearch::onItemActivated);
}

DlgGlobalSearch::~DlgGlobalSearch() {
    delete ui;
}

void DlgGlobalSearch::open() {
    QWidget *parentWindow = static_cast<QWidget *>(parent());
    QPoint center = parentWindow->geometry().center();
    move(center.x() - width() / 2, center.y() - height() / 2);
    ui->leSearch->clear();
    show();
    ui->leSearch->setFocus();

    updateResults();
}

void DlgGlobalSearch::updateResults() {
    if(!isVisible())
        return;

    ui->listWidget->clear();
    QStringList actionIDs = ActionController::globalActionIDs();
    actionIDs.sort();

    for(const QString &actionID : std::as_const(actionIDs)) {
        ActionConfig action = ActionManager::item(actionID);
        if(action.description.contains(ui->leSearch->text(), Qt::CaseInsensitive) ||
            action.id().contains(ui->leSearch->text(), Qt::CaseInsensitive)) {
            QListWidgetItem *item = new QListWidgetItem(action.text + " (" + action.id() + ")");
            item->setIcon(IconController::icon(action.icon));
            item->setData(Qt::UserRole, action.id());
            ui->listWidget->addItem(item);
        }
    }

    if(ui->listWidget->count() > 0)
        ui->listWidget->setCurrentRow(0);
}

bool DlgGlobalSearch::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->leSearch && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        switch (keyEvent->key()) {
        case Qt::Key_Down:
            navigateList(1);
            return true;
        case Qt::Key_Up:
            navigateList(-1);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            activateSelectedItem();
            return true;
        case Qt::Key_Backspace:
            if(ui->leSearch->text().length() == 0)
                close();

            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void DlgGlobalSearch::navigateList(int direction) {
    int current = ui->listWidget->currentRow();
    int count = ui->listWidget->count();
    if (count == 0)
        return;

    int next = qBound(0, current + direction, count - 1);
    ui->listWidget->setCurrentRow(next);
}

void DlgGlobalSearch::activateSelectedItem() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item) {
        onItemActivated(item);
    }
}

void DlgGlobalSearch::onItemActivated(QListWidgetItem *item) {
    QString id = item->data(Qt::UserRole).toString();
    ActionController::executeGlobalAction(id);
    close();
}
