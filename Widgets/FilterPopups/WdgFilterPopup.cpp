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

    connect(ui->tbPopOut, &QAbstractButton::clicked, this, [this]() {
        QPoint buttonPos = mapToGlobal(ui->tbPopOut->pos());
        showTool(buttonPos - QPoint{width(), height()});
    });
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

void WdgFilterPopup::showTool(QPoint pos) {
    setWindowFlags(Qt::Tool);
    ui->tbPopOut->setVisible(false);
    move(pos);
    show();
}

void WdgFilterPopup::showPopup(QPoint pos) {
    setWindowFlags(Qt::Popup);
    ui->tbPopOut->setVisible(true);

    QScreen *screen = QGuiApplication::screenAt(pos);
    if(!screen)
        screen = QGuiApplication::primaryScreen();
    const QRect screenGeometry = screen->availableGeometry();

    const QRect popupRect(pos, size());

    if(popupRect.right() > screenGeometry.right())
        pos.setX(screenGeometry.right() - width());

    if(popupRect.bottom() > screenGeometry.bottom())
        pos.setY(pos.y() - height() - height());
    move(pos);
    show();
}
