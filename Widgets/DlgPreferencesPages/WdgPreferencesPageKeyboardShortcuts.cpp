#include "WdgPreferencesPageKeyboardShortcuts.h"
#include "ui_WdgPreferencesPageKeyboardShortcuts.h"

WdgPreferencesPageKeyboardShortcuts::WdgPreferencesPageKeyboardShortcuts(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageKeyboardShortcuts),
    _sortFilterProxyModel(new KeyboardShortcutsSortFilterProxyModel(this)),
    _model(new KeyboardShortcutsModel(this)) {
    ui->setupUi(this);

    _restoreDefaultShortcutAction = ui->twShortcuts->addAction(QIcon(":/Icons/Undo.ico"),      tr("Restore Default"));
    _removeShortcutAction         = ui->twShortcuts->addAction(QIcon(":/Icons/Delete.ico"),    tr("Remove Shortcut"));
    QAction *separatorAction       = ui->twShortcuts->addAction(""); separatorAction->setSeparator(true);
    _copyIDAction                 = ui->twShortcuts->addAction(QIcon(":/Icons/Duplicate.ico"), tr("Copy ID"));

    ui->twShortcuts->setContextMenuPolicy(Qt::ActionsContextMenu);

    _sortFilterProxyModel->setSourceModel(_model);
    _sortFilterProxyModel->setSortRole(Qt::DisplayRole);

    ActionShortcutMapper::map(_copyIDAction, "application.preferences.keyboardShortcuts.copyID");

    ui->twShortcuts->setModel(_sortFilterProxyModel);

    ui->twShortcuts->setColumnWidth(0, 400);

    ui->twShortcuts->setUniformRowHeights(true);
    ui->twShortcuts->setStyleSheet("QTreeView::item { height: 30px; }");

    ui->twShortcuts->setSortingEnabled(true);
    ui->twShortcuts->header()->setSortIndicatorShown(true);
    ui->twShortcuts->header()->setSortIndicator(0, Qt::AscendingOrder);

    ui->gbCurrentAction->setVisible(false);

    connect(ui->leSearch,       &QLineEdit::textChanged,       _sortFilterProxyModel, &KeyboardShortcutsSortFilterProxyModel::setFilterText);
    connect(ui->cbOnlyModified, &QCheckBox::checkStateChanged, _sortFilterProxyModel, &KeyboardShortcutsSortFilterProxyModel::setFilterModifiedOnly);

    connect(_restoreDefaultShortcutAction, &QAction::triggered,   this, &WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut);
    connect(_removeShortcutAction,         &QAction::triggered,   this, &WdgPreferencesPageKeyboardShortcuts::onRemoveShortcut);
    connect(_copyIDAction,                 &QAction::triggered,   this, &WdgPreferencesPageKeyboardShortcuts::onCopyID);

    connect(ui->tbID,                      &QToolButton::clicked, this, &WdgPreferencesPageKeyboardShortcuts::onCopyID);
    connect(ui->tbRestoreDefault,          &QToolButton::clicked, this, &WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut);

    connect(ui->twShortcuts->selectionModel(), &QItemSelectionModel::currentChanged, this, &WdgPreferencesPageKeyboardShortcuts::onCurrentIndexChanged);

    connect(ui->kseShortcut, &QKeySequenceEdit::keySequenceChanged, this, &WdgPreferencesPageKeyboardShortcuts::onShortcutChanged);

    connect(ui->pbImport, &QPushButton::clicked, this, &WdgPreferencesPageKeyboardShortcuts::onImport);
    connect(ui->pbExport, &QPushButton::clicked, this, &WdgPreferencesPageKeyboardShortcuts::onExport);
}

WdgPreferencesPageKeyboardShortcuts::~WdgPreferencesPageKeyboardShortcuts() {
    delete ui;
}

void WdgPreferencesPageKeyboardShortcuts::reloadPreferences() {
    _model->reload();
}

void WdgPreferencesPageKeyboardShortcuts::savePreferences() {
    _model->saveShortcuts();
}

void WdgPreferencesPageKeyboardShortcuts::discardPreviewPreferences() {}

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

    const KeyboardShortcut metadata = _model->metaData(mappedIndex);
    const QKeySequence     shortcut = _model->shortcut(mappedIndex);

    ui->lIcon->setPixmap(QPixmap(metadata.icon).scaled(20, 20));
    ui->lName->setText(metadata.description);
    ui->lID->setText(metadata.id);

    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(shortcut);
    ui->kseShortcut->blockSignals(false);

    ui->gbCurrentAction->setVisible(true);
}

void WdgPreferencesPageKeyboardShortcuts::onRestoreDefaultShortcut() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id;
    QKeySequence shortcut = GlobalConfig::keyboardShortcutDefaultKeySequence(id);
    _model->setModifiedShortcut(id, shortcut);
    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(shortcut);
    ui->kseShortcut->blockSignals(false);
}

void WdgPreferencesPageKeyboardShortcuts::onRemoveShortcut() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id;
    _model->setModifiedShortcut(id, QKeySequence());
    ui->kseShortcut->blockSignals(true);
    ui->kseShortcut->setKeySequence(QKeySequence());
    ui->kseShortcut->blockSignals(false);
}

void WdgPreferencesPageKeyboardShortcuts::onCopyID() {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const KeyboardShortcut shortcut = _model->metaData(current);
    QApplication::clipboard()->setText(shortcut.id);
}

void WdgPreferencesPageKeyboardShortcuts::onShortcutChanged(const QKeySequence &shortcut) {
    QModelIndex current = _sortFilterProxyModel->mapToSource(ui->twShortcuts->currentIndex());
    if(!current.isValid())
        return;

    const QString id = _model->metaData(current).id;
    _model->setModifiedShortcut(id, shortcut);
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
    LocalConfig::importKeyboardShortcutsFromJson(array);
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

    QJsonArray array = LocalConfig::exportKeyboardShortcutsToJson();
    QJsonDocument document(array);
    QString jsonString = document.toJson();

    f.write(jsonString.toUtf8());
    f.close();
}
