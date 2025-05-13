#include "DlgConfigEditor.h"
#include "ui_DlgConfigEditor.h"

#include "Global/ActionShortcutMapper.h"
#include "ItemModels/LocalConfigModel.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QTreeWidgetItem>
#include <QClipboard>

DlgConfigEditor::DlgConfigEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgConfigEditor),
    _model(new LocalConfigModel(this)) {
    ui->setupUi(this);

    ui->treeView->setModel(_model);

    ui->treeView->setColumnWidth(0, 250);
    ui->treeView->setColumnWidth(1, 500);

    _restoreDefaultAction = ui->treeView->addAction(QIcon(":/Icons/undo.ico"),      tr("Restore Default"));
    _deleteAction         = ui->treeView->addAction(QIcon(":/Icons/xmark.ico"),    tr("Delete"));
    _copyIDAction         = ui->treeView->addAction(QIcon(":/Icons/clone.ico"), tr("Copy ID"));
    QAction *test = ui->treeView->addAction("");
    test->setSeparator(true);
    QAction *reloadAction = ui->treeView->addAction(QIcon(":/Icons/rotate.ico"),    tr("Reload"));

    ActionShortcutMapper::map(_restoreDefaultAction, "application.configuration.key.restoreDefault");
    ActionShortcutMapper::map(_deleteAction,         "application.configuration.key.delete");
    ActionShortcutMapper::map(_copyIDAction,         "application.configuration.key.copyID");
    ActionShortcutMapper::map(reloadAction,          "application.configuration.reload");

    _restoreDefaultAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _copyIDAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    reloadAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_restoreDefaultAction,          &QAction::triggered,                     this,                 &DlgConfigEditor::onSettingRestoreDefault);
    connect(_deleteAction,                  &QAction::triggered,                     this,                 &DlgConfigEditor::onSettingDelete);
    connect(_copyIDAction,                  &QAction::triggered,                     this,                 &DlgConfigEditor::onSettingCopyID);

    connect(ui->pbRestoreDefault,           &QPushButton::clicked,                   this,                 &DlgConfigEditor::onSettingRestoreDefault);
    connect(ui->pbDelete,                   &QPushButton::clicked,                   this,                 &DlgConfigEditor::onSettingDelete);
    connect(ui->pbCopyID,                   &QPushButton::clicked,                   this,                 &DlgConfigEditor::onSettingCopyID);

    connect(_restoreDefaultAction,          &QAction::enabledChanged,                ui->pbRestoreDefault, &QPushButton::setEnabled);
    connect(_deleteAction,                  &QAction::enabledChanged,                ui->pbDelete,         &QPushButton::setEnabled);
    connect(_copyIDAction,                  &QAction::enabledChanged,                ui->pbCopyID,         &QPushButton::setEnabled);

    connect(ui->wdgValueEditor,             &WdgVariantEditor::valueChanged,         this,                 &DlgConfigEditor::onPreviewUpdate);

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this,                 &DlgConfigEditor::onCurrentRowChanged);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,  this,                 &DlgConfigEditor::onSelectionChanged);

    connect(_model,                         &QAbstractItemModel::modelReset,         this,                 &DlgConfigEditor::onSelectionChanged);

    connect(ui->pbReload,                   &QPushButton::clicked,                   _model,               &LocalConfigModel::reload);
    connect(reloadAction,                   &QAction::triggered,                     _model,               &LocalConfigModel::reload);

    ui->treeView->setCurrentIndex(QModelIndex());
    onSelectionChanged();
}

DlgConfigEditor::~DlgConfigEditor() {
    delete ui;
}

void DlgConfigEditor::reject() {
    ui->treeView->setCurrentIndex(QModelIndex()); // trigger saving action for last item
    QDialog::reject();
}

void DlgConfigEditor::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous) {
    // save previous
    if(previous.isValid() && !_model->settingIsGroup(previous) && !_model->settingIsDeleted(previous))
        SettingsManager::setValue(_model->settingID(previous), ui->wdgValueEditor->value());

    // load current
    if(current.isValid()) {
        QString id = _model->settingID(current);
        QVariant value = _model->settingValue(current);
        QString type = value.typeName();

        ui->lID->setText(id);

        if(SettingsManager::itemExists(id)) {
            SettingsItem item = SettingsManager::item(id)            ;
            ui->lDescription->setText(item.description);
            type = item.isGroup ? tr("group") : QMetaType(item.type).name();
        } else {
            ui->lDescription->setText("");
        }

        ui->lType->setText(type);

        if(_model->settingIsGroup(current))
            ui->wdgValueEditor->setValue(QVariant());
        else
            ui->wdgValueEditor->setValue(value);
    } else {
        ui->wdgValueEditor->setValue(QVariant());
        ui->gbCurrentValue->setVisible(false);
    }
}

void DlgConfigEditor::onPreviewUpdate(const QVariant &value) {
    _model->setSettingPreview(ui->treeView->selectionModel()->currentIndex(), value);
}

void DlgConfigEditor::onSelectionChanged() {
    ui->wdgValueEditor->setVisible(true);
    ui->llValue->setVisible(true);

    QModelIndexList selection = ui->treeView->selectionModel()->selectedRows(0);
    int selectionCount = selection.count();

    const QStringList settings = _model->settingsIDList(selection);

    bool canRestoreDefault = false, canDelete = false;

    for(const QString &id : settings) {
        bool exists = SettingsManager::itemExists(id);
        canRestoreDefault |= exists && !SettingsManager::item(id).isGroup;
        canDelete |= !exists;
    }

    _restoreDefaultAction->setEnabled(canRestoreDefault);
    _deleteAction->setEnabled(canDelete);

    ui->gbCurrentValue->setVisible(selectionCount == 1);

    ui->pbRestoreDefault->setVisible(canRestoreDefault);
    ui->pbDelete->setVisible(canDelete || (!canDelete && !canRestoreDefault));

    _copyIDAction->setEnabled(selectionCount == 1);
}

void DlgConfigEditor::onSettingRestoreDefault() {
    const QModelIndexList selection = ui->treeView->selectionModel()->selectedRows();

    QStringList IDs;
    for(const QModelIndex &index : selection)
        IDs << _model->settingID(index);

    QString listString = "<ul><li>" + IDs.join("</li><li>") + "</li></ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Restore default settings"), tr("<p><b>Do you really want to restore the default values for these settings?</b></p>") + listString + (IDs.count() > 1 ? tr("<p><b>Hint:</b> Some unregistered settings cannot restore a default value. They will not change.</p>") : ""), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    for(const QString &id : std::as_const(IDs)) {
        if(!SettingsManager::itemExists(id))
            continue;

        QVariant defaultValue = SettingsManager::restoreDefaultValue(id);
        ui->wdgValueEditor->setValue(defaultValue);
    }
}

void DlgConfigEditor::onSettingDelete() {
    const QModelIndexList selection = ui->treeView->selectionModel()->selectedRows();

    QStringList IDs;
    for(const QModelIndex &index : selection)
        IDs << _model->settingID(index);

    QString listString = "<ul><li>" + IDs.join("</li><li>") + "</li></ul>";

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete settings"), tr("<p><b>Do you really want to delete these setting?</b></p>")+ listString + (IDs.count() > 1 ? tr("<p><b>Hint:</b> Some registered settings cannot be deleted. They will be reset to their default value instead.</p>") : ""), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    for(const QString &id : std::as_const(IDs)) {
        if(SettingsManager::itemExists(id)) {
            QVariant defaultValue = SettingsManager::restoreDefaultValue(id);
            ui->wdgValueEditor->setValue(defaultValue);
        } else {
            SettingsManager::removeKey(id);
        }
    }
}

void DlgConfigEditor::onSettingCopyID() {
    QModelIndex current = ui->treeView->currentIndex();
    QApplication::clipboard()->setText(_model->settingID(current));
}
