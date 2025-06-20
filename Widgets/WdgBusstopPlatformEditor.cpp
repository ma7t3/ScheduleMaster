#include "WdgBusstopPlatformEditor.h"
#include "ui_WdgBusstopPlatformEditor.h"

WdgBusstopPlatformEditor::WdgBusstopPlatformEditor(QWidget *parent) :
    QWidget(parent), ui(new Ui::WdgBusstopPlatformEditor) {
    ui->setupUi(this);

    _flagCheckboxes = {ui->cbFlagStandard, ui->cbFlagArrival, ui->cbFlagBreak, ui->cbFlagDeparture, ui->cbFlagWaiting, ui->cbFlagInternal};

    connect(ui->leName, &QLineEdit::textChanged, this, &WdgBusstopPlatformEditor::onNameChanged);
    connect(ui->pteComment, &QPlainTextEdit::textChanged, this, &WdgBusstopPlatformEditor::onCommentChanged);

    for(QCheckBox *cb : std::as_const(_flagCheckboxes))
        connect(cb, &QCheckBox::toggled, this, [this, cb]() { onFlagChanged(cb, cb->isChecked()); });
}

WdgBusstopPlatformEditor::~WdgBusstopPlatformEditor() {
    delete ui;
}

BusstopPlatform *WdgBusstopPlatformEditor::platform() const {
    return _platform;
}

void WdgBusstopPlatformEditor::setPlatform(BusstopPlatform *platform) {
    _platform = platform;

    ui->leName->blockSignals(true);
    ui->pteComment->blockSignals(true);

    ui->leName->setText(_platform->name());
    ui->pteComment->setPlainText(_platform->comment());

    ui->leName->blockSignals(false);
    ui->pteComment->blockSignals(false);

    setFlags(_platform->flags());
}

void WdgBusstopPlatformEditor::focusName() {
    ui->leName->setFocus();
}

void WdgBusstopPlatformEditor::setFlags(const BusstopPlatformFlags &flags) {
    blockCheckboxSignals(false);

    for(QCheckBox *cb : std::as_const(_flagCheckboxes))
        cb->setChecked(false);

    if(flags.testFlag(BusstopPlatformFlag::StandardBusstopPlatform)) {
        ui->cbFlagStandard->setChecked(true);
    } else {
        if(flags.testFlag(BusstopPlatformFlag::ArrivalBusstopPlatform))
            ui->cbFlagArrival->setChecked(true);

        if(flags.testFlag(BusstopPlatformFlag::BreakBusstopPlatform))
            ui->cbFlagBreak->setChecked(true);

        if(flags.testFlag(BusstopPlatformFlag::DepartureBusstopPlatform))
            ui->cbFlagDeparture->setChecked(true);

        if(flags.testFlag(BusstopPlatformFlag::WaitBusstopPlatform))
            ui->cbFlagWaiting->setChecked(true);

        if(flags.testFlag(BusstopPlatformFlag::InternalBusstopPlatform))
            ui->cbFlagInternal->setChecked(true);
    }

    blockCheckboxSignals(false);
}

BusstopPlatformFlags WdgBusstopPlatformEditor::readCheckboxes() const {
    BusstopPlatformFlags flags;
    if(ui->cbFlagStandard->isChecked())
        return flags;

    if(ui->cbFlagArrival->isChecked())
        flags.setFlag(BusstopPlatformFlag::ArrivalBusstopPlatform);

    if(ui->cbFlagBreak->isChecked())
        flags.setFlag(BusstopPlatformFlag::BreakBusstopPlatform);

    if(ui->cbFlagDeparture->isChecked())
        flags.setFlag(BusstopPlatformFlag::DepartureBusstopPlatform);

    if(ui->cbFlagWaiting->isChecked())
        flags.setFlag(BusstopPlatformFlag::WaitBusstopPlatform);

    if(ui->cbFlagInternal->isChecked())
        flags.setFlag(BusstopPlatformFlag::InternalBusstopPlatform);

    return flags;
}

void WdgBusstopPlatformEditor::blockCheckboxSignals(const bool &block) {
    for(QCheckBox *cb : std::as_const(_flagCheckboxes))
        cb->blockSignals(block);
}

void WdgBusstopPlatformEditor::onNameChanged(const QString &name) {
    if(_platform)
        _platform->setName(name);
}

void WdgBusstopPlatformEditor::onCommentChanged() {
    if(_platform)
        _platform->setComment(ui->pteComment->toPlainText());
}

void WdgBusstopPlatformEditor::onFlagChanged(QCheckBox *checkBox, const bool &checked) {
    blockCheckboxSignals(true);

    if(checkBox == ui->cbFlagStandard && checked) {
        for(QCheckBox *cb : std::as_const(_flagCheckboxes))
            if(cb != ui->cbFlagStandard)
                cb->setChecked(false);

    } else if(checkBox == ui->cbFlagStandard && !checked) {
        ui->cbFlagStandard->setChecked(true);
    } else {
        bool anyChecked = false;
        for(QCheckBox *cb : std::as_const(_flagCheckboxes))
            anyChecked |= cb->isChecked();

        ui->cbFlagStandard->setChecked(!anyChecked);
    }

    _platform->setFlags(readCheckboxes());

    blockCheckboxSignals(false);
}
