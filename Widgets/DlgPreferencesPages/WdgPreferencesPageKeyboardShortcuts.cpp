#include "WdgPreferencesPageKeyboardShortcuts.h"
#include "ui_WdgPreferencesPageKeyboardShortcuts.h"

#include "Global/KeyboardShortcutManager.h"
#include "ItemModels/KeyboardShortcutsModel.h"
#include "Global/ActionShortcutMapper.h"

#include <QStandardPaths>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>

WdgPreferencesPageKeyboardShortcuts::WdgPreferencesPageKeyboardShortcuts(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageKeyboardShortcuts),
    _sortFilterProxyModel(new KeyboardShortcutsSortFilterProxyModel(this)),
    _model(new KeyboardShortcutsModel(this)) {
    ui->setupUi(this);

    reloadPreferences();

    _restoreDefaultShortcutAction = ui->twShortcuts->addAction(QIcon(":/Icons/Undo.ico"),      tr("Restore Default"));
    _removeShortcutAction         = ui->twShortcuts->addAction(QIcon(":/Icons/Delete.ico"),    tr("Remove Shortcut"));
    QAction *separatorAction       = ui->twShortcuts->addAction(""); separatorAction->setSeparator(true);
    _copyIDAction                 = ui->twShortcuts->addAction(QIcon(":/Icons/Duplicate.ico"), tr("Copy ID"));

    _showOnlyModifiedAction       = addAction(QIcon(":/Icons/Edit.ico"), tr("Show only modified")); _showOnlyModifiedAction->setCheckable(true);
    _importAction                 = addAction(QIcon(":/Icons/FileImport.ico"), tr("Import"));
    _exportAction                 = addAction(QIcon(":/Icons/FileExport.ico"), tr("Export"));
    _resetAllAction               = addAction(QIcon(":/Icons/Undo.ico"), tr("Reset All"));
    _focusSearchAction            = addAction("");

    ActionShortcutMapper::map(_restoreDefaultShortcutAction, "application.preferences.keyboardShortcuts.restoreDefaultShortcut");
    ActionShortcutMapper::map(_removeShortcutAction,         "application.preferences.keyboardShortcuts.removeShortcut");
    ActionShortcutMapper::map(_copyIDAction,                 "application.preferences.keyboardShortcuts.copyID");
    ActionShortcutMapper::map(_showOnlyModifiedAction,       "application.preferences.keyboardShortcuts.showOnlyModified");
    ActionShortcutMapper::map(_importAction,                 "application.preferences.keyboardShortcuts.import");
    ActionShortcutMapper::map(_exportAction,                 "application.preferences.keyboardShortcuts.export");
    ActionShortcutMapper::map(_resetAllAction,               "application.preferences.keyboardShortcuts.resetAll");
    ActionShortcutMapper::map(_focusSearchAction,            "application.preferences.keyboardShortcuts.focusSearch");

    QMenu *actionsMenu = new QMenu(ui->tbActions);
    actionsMenu->addAction(_showOnlyModifiedAction);
    actionsMenu->addSeparator();
    actionsMenu->addAction(_importAction);
    actionsMenu->addAction(_exportAction);
    actionsMenu->addAction(_resetAllAction);

    ui->tbActions->setMenu(actionsMenu);
    ui->tbActions->setPopupMode(QToolButton::InstantPopup);

    ui->twShortcuts->setContextMenuPolicy(Qt::ActionsContextMenu);
    _sortFilterProxyModel->setSourceModel(_model);
    _sortFilterProxyModel->setSortRole(Qt::DisplayRole);


    ui->twShortcuts->setModel(_sortFilterProxyModel);

    ui->twShortcuts->setColumnWidth(0, 500);

    ui->twShortcuts->setUniformRowHeights(true);
    ui->twShortcuts->setStyleSheet("QTreeView::item { height: 30px; }");

    ui->twShortcuts->setSortingEnabled(true);
    ui->twShortcuts->header()->setSortIndicatorShown(true);
    ui->twShortcuts->header()->setSortIndicator(0, Qt::AscendingOrder);

    ui->gbCurrentAction->setVisible(false);

    connect(ui->leSearch,                      &QLineEdit::textChanged,               _sortFilterProxyModel,   &KeyboardShortcutsSortFilterProxyModel::setFilterText);
    connect(_showOnlyModifiedAction,           &QAction::toggled,                     _sortFilterProxyModel,   &KeyboardShortcutsSortFilterProxyModel::setFilterModifiedOnly);

    connect(ui->tbID,                          &QToolButton::clicked,                 this,                    &WdgPreferencesPageKeyboardShortcuts::onCopyID);
    connect(ui->tbRestoreDefault,              &QToolButton::clicked,                 this,                    &WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut);

    connect(ui->twShortcuts->selectionModel(), &QItemSelectionModel::currentChanged,  this,                    &WdgPreferencesPageKeyboardShortcuts::onCurrentIndexChanged);

    connect(ui->kseShortcut,                   &QKeySequenceEdit::keySequenceChanged, this,                    &WdgPreferencesPageKeyboardShortcuts::onShortcutChanged);


    connect(_restoreDefaultShortcutAction,     &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut);
    connect(_removeShortcutAction,             &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onRemoveShortcut);
    connect(_copyIDAction,                     &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onCopyID);

    connect(_importAction,                     &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onImport);
    connect(_exportAction,                     &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onExport);
    connect(_resetAllAction,                   &QAction::triggered,                   this,                    &WdgPreferencesPageKeyboardShortcuts::onResetAll);

    connect(_focusSearchAction,                &QAction::triggered,                   ui->leSearch,            [this](){ui->leSearch->setFocus(Qt::ShortcutFocusReason);});
}

WdgPreferencesPageKeyboardShortcuts::~WdgPreferencesPageKeyboardShortcuts() {
    delete ui;
}

void WdgPreferencesPageKeyboardShortcuts::reloadPreferences() {
    _model->reload();
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageKeyboardShortcuts::savePreferences() {
    _model->saveShortcuts();
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageKeyboardShortcuts::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageKeyboardShortcuts::id() {
    return "base.keyboardShortcuts";
}

QString WdgPreferencesPageKeyboardShortcuts::name() {
    return tr("Keyboard Shortcuts");
}

QIcon WdgPreferencesPageKeyboardShortcuts::icon() {
    return QIcon(":/Icons/Keyboard.ico");
}

void WdgPreferencesPageKeyboardShortcuts::onCurrentIndexChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    if(!current.isValid()) {
        ui->gbCurrentAction->setVisible(false);
        return;
    }

    QModelIndex mappedIndex = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());

    const KeyboardShortcutConfig metadata = _model->metaData(mappedIndex);
    const QKeySequence     shortcut = _model->shortcut(mappedIndex);

    ui->lIcon->setPixmap(QPixmap(metadata.icon));
    ui->lName->setText(metadata.description);
    ui->lID->setText(metadata.id());

    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(shortcut);
    ui->kseShortcut->blockSignals(false);

    ui->gbCurrentAction->setVisible(true);
}

void WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id();
    QKeySequence shortcut = KeyboardShortcutManager::item(id).defaultKeySequence;
    _model->setModifiedShortcut(id, shortcut);
    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(shortcut);
    ui->kseShortcut->blockSignals(false);
    setUnsaved();
}

void WdgPreferencesPageKeyboardShortcuts::onRemoveShortcut() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id();
    _model->setModifiedShortcut(id, QKeySequence());
    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(QKeySequence());
    ui->kseShortcut->blockSignals(false);
    setUnsaved();
}

void WdgPreferencesPageKeyboardShortcuts::onCopyID() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const KeyboardShortcutConfig shortcut = _model->metaData(current);
    QApplication::clipboard()->setText(shortcut.id());
}

void WdgPreferencesPageKeyboardShortcuts::onShortcutChanged(const QKeySequence &shortcut) {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id();
    _model->setModifiedShortcut(id, shortcut);
    setUnsaved();
}

void WdgPreferencesPageKeyboardShortcuts::onImport() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Keyboard Shortcuts"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("JSON File (*.json)"));
    if(fileName.isEmpty())
        return;

    QFile f(fileName);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Failed reading file"), tr("<p><b>Couldn't read file:</b></p><p>%1</p><p><b>%2</b></p>").arg(fileName, f.errorString()));
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);
    f.close();
    if(error.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, tr("Failed parsing file"), tr("<p><b>Couldn't parse file:</b></p><p>%1</p><p><b>%2</b></p>").arg(fileName, error.errorString()));
        return;
    }

    QJsonArray array = doc.array();
    KeyboardShortcutManager::importJson(array);
    _model->reload();
}

void WdgPreferencesPageKeyboardShortcuts::onExport() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Keyboard Shortcuts"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/ScheduleMaster_KeyboardShortcuts.json", tr("JSON File (*.json)"));
    if(fileName.isEmpty())
        return;

    QFile f(fileName);
    if(!f.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Failed saving file"), tr("<p><b>Couldn't save file:</b></p><p>%1</p><p><b>%2</b></p>").arg(fileName, f.errorString()));
        return;
    }

    QJsonArray array = KeyboardShortcutManager::exportJson();
    QJsonDocument document(array);
    QString jsonString = document.toJson();

    f.write(jsonString.toUtf8());
    f.close();
}

void WdgPreferencesPageKeyboardShortcuts::onResetAll() {
    if(QMessageBox::warning(this, tr("Reset all keyboard shortcuts"), tr("<p><b>Do you really want to reset all keyboard shortcuts?</b</p><p>Maybe you want to export your current shortcut configuration before.</p>"), QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
        return;

    _model->setAllShortcutsToDefault();
}
