#include "WdgBusstopFlagEditor.h"
#include "ui_WdgBusstopFlagEditor.h"

#include <QMessageBox>
#include "Global/IconController.h"

WdgBusstopFlagEditor::WdgBusstopFlagEditor(QWidget *parent) :
    QWidget(parent), ui(new Ui::WdgBusstopFlagEditor), _editorMode(true), _anotherChecked(false) {
    ui->setupUi(this);

    ui->cbStandard->setIcon(IconController::icon("busstop"));
    ui->cbFinal->setIcon(IconController::icon("busstop-terminus"));
    ui->cbTransfer->setIcon(IconController::icon("busstop-transfer"));
    ui->cbCentralTransfer->setIcon(IconController::icon("busstop-transfer-central"));
    ui->cbInternal->setIcon(IconController::icon("gear"));

    _checkboxes = {ui->cbFinal, ui->cbTransfer, ui->cbCentralTransfer, ui->cbInternal};

    for(QCheckBox *cb : std::as_const(_checkboxes))
        connect(cb, &QCheckBox::toggled, this, [this, cb]() {
            onAnyCheckboxToggled(cb);
        });

    connect(ui->cbStandard, &QCheckBox::toggled, this, &WdgBusstopFlagEditor::onStandardCheckboxToggled);
}

WdgBusstopFlagEditor::~WdgBusstopFlagEditor() {
    delete ui;
}

BusstopFlags WdgBusstopFlagEditor::flags() const {
    BusstopFlags flags = BusstopFlag::StandardBusstop;;
    if(ui->cbStandard->isChecked())
        return flags;

    flags.setFlag(BusstopFlag::FinalBusstop, ui->cbFinal->isChecked());
    flags.setFlag(BusstopFlag::TransferBusstop, ui->cbTransfer->isChecked());
    flags.setFlag(BusstopFlag::CentralTransferBusstop, ui->cbCentralTransfer->isChecked());
    flags.setFlag(BusstopFlag::InternalBusstop, ui->cbInternal->isChecked());

    return flags;
}

void WdgBusstopFlagEditor::setEditorMode(const bool &b) {
    _editorMode = b;
}

void WdgBusstopFlagEditor::setFlags(const BusstopFlags &flags) {
    if(flags.testFlag(BusstopFlag::StandardBusstop)) {
        ui->cbStandard->setChecked(true);
    } else {
        ui->cbFinal->setChecked(flags.testFlag(BusstopFlag::FinalBusstop));
        ui->cbTransfer->setChecked(flags.testFlag(BusstopFlag::TransferBusstop));
        ui->cbCentralTransfer->setChecked(flags.testFlag(BusstopFlag::CentralTransferBusstop));
        ui->cbInternal->setChecked(flags.testFlag(BusstopFlag::InternalBusstop));
    }

    emit flagsChanged(this->flags());
}

void WdgBusstopFlagEditor::onStandardCheckboxToggled(const bool &checked) {
    if(checked) {
        for(QCheckBox *cb : std::as_const(_checkboxes))
            cb->setChecked(false);

        emit flagsChanged(this->flags());
    } else {
        if(_editorMode)
            QMessageBox::warning(this, tr("Unable to uncheck \"Standard Busstop\""), tr("You cannot uncheck this option because it's the default value. Check another one instead."));
        ui->cbStandard->setChecked(true);
    }
}

void WdgBusstopFlagEditor::onAnyCheckboxToggled(QCheckBox *checkbox) {
    bool anyChecked = std::any_of(_checkboxes.begin(), _checkboxes.end(), [](QCheckBox *cb) {
        return cb->isChecked();
    });

    if(_editorMode) {
        if(checkbox->isChecked()) {
            if(checkbox == ui->cbCentralTransfer) {
                blockAllCheckboxSignals(true);
                ui->cbTransfer->setChecked(false);
                blockAllCheckboxSignals(false);
            } else if(checkbox == ui->cbTransfer) {
                blockAllCheckboxSignals(true);
                ui->cbCentralTransfer->setChecked(false);
                blockAllCheckboxSignals(false);
            }
        }
    }

    blockAllCheckboxSignals(true);
    ui->cbStandard->setChecked(!anyChecked);
    blockAllCheckboxSignals(false);

    emit flagsChanged(this->flags());
}

void WdgBusstopFlagEditor::blockAllCheckboxSignals(const bool &block) {
    ui->cbStandard->blockSignals(block);
    for(QCheckBox *cb : std::as_const(_checkboxes))
        cb->blockSignals(block);
}
