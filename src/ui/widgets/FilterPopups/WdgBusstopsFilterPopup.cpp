#include "WdgBusstopsFilterPopup.h"
#include "ui_WdgBusstopsFilterPopup.h"

WdgBusstopsFilterPopup::WdgBusstopsFilterPopup(QWidget *parent) :
    WdgFilterPopupContent(parent), ui(new Ui::WdgBusstopsFilterPopup),
    _lineModel(new LineSelectionListModel(this)),
    _lineProxyModel(new LineSelectionListProxyModel(this)) {
    ui->setupUi(this);

    ui->flagSelector->setEditorMode(false);

    connect(ui->flagSelector, &WdgBusstopFlagEditor::flagsChanged,          this, &WdgBusstopsFilterPopup::onSomethingChanged);
    connect(_lineModel,       &LineSelectionListModel::checkedLinesChanged, this, &WdgBusstopsFilterPopup::onSomethingChanged);

    _lineModel->setCheckboxesEnabled(true);
    _lineProxyModel->setSourceModel(_lineModel);

    ui->lwLines->setSortingEnabled(true);
    ui->lwLines->setModel(_lineProxyModel);
    ui->lwLines->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->lwLines->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->lwLines->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->lwLines->sortByColumn(1, Qt::AscendingOrder);
}

WdgBusstopsFilterPopup::~WdgBusstopsFilterPopup() {
    delete ui;
}

bool WdgBusstopsFilterPopup::filterIsEnabled() const {
    return ui->flagSelector->flags() != BusstopFlag::StandardBusstop || !_lineModel->checkedLines().isEmpty();
}

BusstopFlags WdgBusstopsFilterPopup::filterFlags() const {
    return ui->flagSelector->flags();
}

PDISet<Line> WdgBusstopsFilterPopup::filterLines() const {
    return _lineModel->checkedLines();
}

void WdgBusstopsFilterPopup::onSomethingChanged() {
    emit filterChanged();
}

void WdgBusstopsFilterPopup::reset() {
    _lineModel->clearCheckedLines();
    ui->flagSelector->setFlags(BusstopFlag::StandardBusstop);
}
