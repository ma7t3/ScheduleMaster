#include "WdgTimeProfileEditor.h"
#include "ui_WdgTimeProfileEditor.h"

#include "src/projectdata/TimeProfile.h"

#include "Global/ActionController.h"

WdgTimeProfileEditor::WdgTimeProfileEditor(QWidget *parent) :
    QWidget(parent), ui(new Ui::WdgTimeProfileEditor), _timeProfile(nullptr) {
    ui->setupUi(this);

    connect(ui->leProfileName, &QLineEdit::textChanged, this, [this](const QString &name){
        _timeProfile->setName(name);
        emit somethingChanged();
    });

    connect(ui->cbHighlightNegativeTime, &QCheckBox::toggled, ui->twProfile, &WdgTimeProfileEditorTable::setHighlightNegativeTime);

    connect(ui->rbInputMethodRaw,     &QRadioButton::clicked, this, &WdgTimeProfileEditor::onInputModeChanged);
    connect(ui->rbInputMethodRefTime, &QRadioButton::clicked, this, &WdgTimeProfileEditor::onInputModeChanged);

    connect(ui->teRefTime, &QTimeEdit::timeChanged, ui->twProfile, &WdgTimeProfileEditorTable::setReferenceTime);

    ui->twProfile->setHighlightNegativeTime(ui->cbHighlightNegativeTime->isChecked());

    connect(ui->rbInputBehaviorRaw,             &QRadioButton::clicked, this, &WdgTimeProfileEditor::onInputBehaviorChanged);
    connect(ui->rbInputBehaviorNoNegativeTimes, &QRadioButton::clicked, this, &WdgTimeProfileEditor::onInputBehaviorChanged);
    connect(ui->rbInputBehaviorProportional,    &QRadioButton::clicked, this, &WdgTimeProfileEditor::onInputBehaviorChanged);

    connect(ui->cbKeepLayovers,    &QCheckBox::toggled, ui->twProfile, &WdgTimeProfileEditorTable::setKeepLayovers);
    connect(ui->cbNoNegativeTimes, &QCheckBox::toggled, ui->twProfile, &WdgTimeProfileEditorTable::setPreventNegativeTravelTimesWhileProportionalMode);
    connect(ui->cbKeepLayovers2,   &QCheckBox::toggled, ui->twProfile, &WdgTimeProfileEditorTable::setKeepLayoversWhileProportionalMode);

    connect(ui->pbOptions, &QPushButton::toggled, this, [this](const bool &checked) {
        ui->gbOptions->setVisible(checked);
    });

    ActionController::add(ui->pbOptions, "timeProfileEditor.options", ActionController::IconComponent|ActionController::TooltipComponent);

    ui->gbOptions->setVisible(false);

    connect(ui->cbNoNegativeTimes, &QCheckBox::toggled, ui->cbKeepLayovers2, [this]() {
        ui->cbKeepLayovers2->setEnabled(ui->cbNoNegativeTimes->isChecked()
                                        && ui->rbInputBehaviorProportional->isChecked());
    });

    connect(ui->rbInputBehaviorProportional, &QRadioButton::toggled, ui->cbKeepLayovers2, [this]() {
        ui->cbKeepLayovers2->setEnabled(ui->cbNoNegativeTimes->isChecked()
                                        && ui->rbInputBehaviorProportional->isChecked());
    });

    ui->twProfile->setKeepLayovers(ui->cbKeepLayovers->isChecked());
    onInputBehaviorChanged();
}

WdgTimeProfileEditor::~WdgTimeProfileEditor() {
    delete ui;
}

void WdgTimeProfileEditor::setTimeProfile(TimeProfile *p) {
    _timeProfile = p;
    ui->twProfile->setTimeProfile(_timeProfile);

    ui->leProfileName->blockSignals(true);
    ui->leProfileName->setText(_timeProfile ? _timeProfile->name() : "");
    ui->leProfileName->blockSignals(false);
}

void WdgTimeProfileEditor::reload() {
    ui->twProfile->reload();
}

void WdgTimeProfileEditor::onInputModeChanged() {
    const bool raw = ui->rbInputMethodRaw->isChecked();
    ui->teRefTime->setEnabled(!raw);
    ui->twProfile->setInputMode(raw ? WdgTimeProfileEditorTable::RawInputMode : WdgTimeProfileEditorTable::ReferenceTimeInputMode);
}

void WdgTimeProfileEditor::onInputBehaviorChanged() {
    ui->twProfile->setInputBehavior(
        ui->rbInputBehaviorNoNegativeTimes->isChecked()
            ? WdgTimeProfileEditorTable::PreventNegativeTravelTimesInputBehavior
        : ui->rbInputBehaviorProportional->isChecked()
            ? WdgTimeProfileEditorTable::ProportionalInputBehavior
            : WdgTimeProfileEditorTable::RawInputBehavior);

    ui->cbKeepLayovers->setEnabled(ui->rbInputBehaviorNoNegativeTimes->isChecked());
    ui->cbNoNegativeTimes->setEnabled(ui->rbInputBehaviorProportional->isChecked());
}
