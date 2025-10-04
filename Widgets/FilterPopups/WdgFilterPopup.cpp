#include "WdgFilterPopup.h"
#include "ui_WdgFilterPopup.h"

#include "Global/ActionController.h"

#include <QScreen>

WdgFilterPopup::WdgFilterPopup(QWidget *parent) :
    QWidget(parent), ui(new Ui::WdgFilterPopup), _contentWidgetSet(false) {
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

    connect(ui->pbClose, &QPushButton::clicked, this, &QWidget::hide);

    ActionController::add(ui->pbClearFilter,
                          "projectDataTable.filter.clear",
                          ActionController::AllExceptIconComponent
                              & ActionController::AllExceptShortcutComponent);
}

WdgFilterPopup::~WdgFilterPopup() {
    delete ui;
}

void WdgFilterPopup::setContentWidget(WdgFilterPopupContent *widget) {
    if(!_contentWidgetSet) {
        widget->setParent(this);
        ui->verticalLayout->insertWidget(0, widget, 1);
        connect(ui->pbClearFilter, &QAbstractButton::clicked, widget, &WdgFilterPopupContent::reset);

        _contentWidgetSet = true;
    }
}
