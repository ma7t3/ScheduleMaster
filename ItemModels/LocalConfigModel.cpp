#include "LocalConfigModel.h"

#include "Global/VariantConverter.h"

#include <QIcon>

LocalConfigModelSetting::LocalConfigModelSetting(const QString &id, LocalConfigModelSetting *parentSetting, QObject *parent) :
    QObject(parent), _deleted(false) {
    _id = id;

    _unkown = !SettingsManager::itemExists(id);

    if(!_unkown)
        _metaData = SettingsManager::item(id);

    _value = SettingsManager::value(id);

    const QStringList groupKeys = SettingsManager::groupSubGroups(id);
    const QStringList settingKeys = SettingsManager::keysInGroup(id);

    _parent = parentSetting;

    for(const QString &key : groupKeys) {
        LocalConfigModelSetting *setting = new LocalConfigModelSetting(_id + "/" + key, this, this);
        connect(setting, &LocalConfigModelSetting::settingChanged, this, &LocalConfigModelSetting::settingChanged);
        connect(setting, &LocalConfigModelSetting::settingRemoved, this, &LocalConfigModelSetting::settingRemoved);
        _children << setting;
    }

    for(const QString &key : settingKeys) {
        LocalConfigModelSetting *setting = new LocalConfigModelSetting(_id + "/" + key, this, this);
        connect(setting, &LocalConfigModelSetting::settingChanged, this, &LocalConfigModelSetting::settingChanged);
        connect(setting, &LocalConfigModelSetting::settingRemoved, this, &LocalConfigModelSetting::settingRemoved);
        _children << setting;
    }

    connect(SettingsManager::instance(), &SettingsManager::valueChanged, this, &LocalConfigModelSetting::onSettingChanged);
    connect(SettingsManager::instance(), &SettingsManager::keyRemoved,   this, &LocalConfigModelSetting::onSettingRemoved);
}

QString LocalConfigModelSetting::id() const {
    return _id;
}

bool LocalConfigModelSetting::isGroup() const {
    return _metaData.isGroup;
}

SettingsItem LocalConfigModelSetting::metaData() const {
    return _metaData;
}

bool LocalConfigModelSetting::isUnknown() const {
    return _unkown;
}

QVariant LocalConfigModelSetting::value() {
    return _value;
}

LocalConfigModelSetting *LocalConfigModelSetting::parentSetting() const {
    return _parent;
}

int LocalConfigModelSetting::childrenSettingsCount() const {
    return _children.count();
}

LocalConfigModelSetting *LocalConfigModelSetting::childSetting(const int &index) const {
    if(index < 0 || index >= _children.count())
        return nullptr;

    return _children[index];
}

QList<LocalConfigModelSetting *> LocalConfigModelSetting::childrenSettings() const {
    return _children;
}

int LocalConfigModelSetting::indexOfChildSetting(LocalConfigModelSetting *child) const {
    return _children.indexOf(child);
}

void LocalConfigModelSetting::removeChildSetting(LocalConfigModelSetting *child) {
    if(!child)
        return;

    _children.removeOne(child);
}

bool LocalConfigModelSetting::deleted() const {
    return _deleted;
}

void LocalConfigModelSetting::setDeleted() {
    _deleted = true;
}

void LocalConfigModelSetting::onSettingChanged(const QString &id, const QVariant &value) {
    if(id != _id)
        return;

    _value = value;
    emit settingChanged(this, _value);
}

void LocalConfigModelSetting::onSettingRemoved(const QString &id) {
    if(id != _id)
        return;

    emit settingRemoved(this);
}

LocalConfigModel::LocalConfigModel(QObject *parent) :
    QAbstractItemModel(parent) {
    reloadSettings();
}

QVariant LocalConfigModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
        case IDColumn:          return tr("ID");
        case DescriptionColumn: return tr("Description");
        case TypeColumn:        return tr("Type");
        case ValueColumn:       return tr("Value");
    }
    return QVariant();
}

QModelIndex LocalConfigModel::index(int row, int column, const QModelIndex &parent) const {
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    LocalConfigModelSetting *setting;

    if(!parent.isValid()) {
        setting = settingItem(row);
    } else {
        LocalConfigModelSetting *parentSetting = static_cast<LocalConfigModelSetting *>(parent.internalPointer());
        if(!parentSetting)
            setting = settingItem(row);
        else
            setting = parentSetting->childSetting(row);
    }

    if(setting)
        return createIndex(row, column, setting);
    else
        return QModelIndex();
}

QModelIndex LocalConfigModel::parent(const QModelIndex &index) const {
    if(!index.isValid())
        return QModelIndex();

    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(!setting)
        return QModelIndex();

    if(_settings.contains(setting))
        return QModelIndex();

    LocalConfigModelSetting *parent = setting->parentSetting();
    if(!parent)
        return QModelIndex();

    LocalConfigModelSetting *grandParent = parent->parentSetting();
    int row = grandParent ? grandParent->indexOfChildSetting(parent) : _settings.indexOf(parent);
    if(row == -1)
        return QModelIndex();

    return createIndex(row, 0, parent);
}

int LocalConfigModel::rowCount(const QModelIndex &parent) const {
    if(!parent.isValid())
        return _settings.count();

    if(parent.column() > 0)
        return 0;

    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(parent.internalPointer());
    if(!setting)
        return 0;

    return setting->childrenSettingsCount();
}

int LocalConfigModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 4;
}

QVariant LocalConfigModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && (role != Qt::DecorationRole || index.column() != 0)))
        return QVariant();

    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(index == _previewIndex && role == Qt::DisplayRole) {
        return valueString(_previewValue);
    }

    if(!setting)
        return QVariant();

    bool unknownSetting = setting->isUnknown();
    SettingsItem metaData = setting->metaData();
    bool isGroup = setting->isGroup();

    if(role == Qt::DecorationRole) {
        if(isGroup)
            return QIcon(":/Icons/classic/folder-open.ico");
        else if(unknownSetting)
            return QIcon(":/Icons/classic/file-close.ico");
        else
            return QIcon(":/Icons/classic/file-gear.ico");
    }

    switch(index.column()) {
        case IDColumn:          return setting->id().split("/").last();
        case DescriptionColumn: return metaData.description.split("\n").first();
        case TypeColumn:        return isGroup ? tr("group") : unknownSetting ? setting->value().typeName() : QMetaType(metaData.type).name();
        case ValueColumn:       return isGroup ? tr("(%n item(s))", "", setting->childrenSettingsCount()) : valueString(setting->value());
    }

    return QVariant();
}

QString LocalConfigModel::settingID(const QModelIndex &index) const {
    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(!setting)
        return "";

    return setting->id();
}

bool LocalConfigModel::settingIsDeleted(const QModelIndex &index) const {
    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(!setting)
        return true;

    return setting->deleted();
}

bool LocalConfigModel::settingIsGroup(const QModelIndex &index) const {
    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(!setting)
        return false;

    return setting->isGroup();
}

QVariant LocalConfigModel::settingValue(const QModelIndex &index) const {
    LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
    if(!setting)
        return QVariant();

    return setting->value();
}

QStringList LocalConfigModel::settingsIDList(const QModelIndexList &indexList) {
    QStringList list;
    for(const QModelIndex index : indexList) {
        LocalConfigModelSetting *setting = static_cast<LocalConfigModelSetting *>(index.internalPointer());
        if(!setting)
            continue;

        list << setting->id();
    }
    return list;
}

void LocalConfigModel::setSettingPreview(const QModelIndex &index, const QVariant &value) {
    if(!index.isValid()) {
        _previewIndex = QModelIndex();
        _previewValue = QVariant();
        return;
    }

    _previewValue = value;
    QModelIndex previewIndex = index.siblingAtColumn(ValueColumn);
    _previewIndex = previewIndex;
    emit dataChanged(previewIndex, previewIndex, {Qt::DisplayRole});
}

void LocalConfigModel::endPreview() {
    _previewValue = QVariant();
}

void LocalConfigModel::reload() {
    beginResetModel();
    reloadSettings();
    endResetModel();
}

LocalConfigModelSetting *LocalConfigModel::settingItem(const int &index) const {
    if(index < 0 || index >= _settings.count())
        return nullptr;

    return _settings[index];
}

QModelIndex LocalConfigModel::indexFromSetting(LocalConfigModelSetting *setting) const {
    if (!_settings.contains(setting)) {
        LocalConfigModelSetting *parent = setting->parentSetting();
        if (!parent)
            return QModelIndex();

        int row = parent->indexOfChildSetting(setting);
        return createIndex(row, 0, setting);
    }

    int row = _settings.indexOf(setting);
    return createIndex(row, 0, setting);
}

void LocalConfigModel::reloadSettings() {
    endPreview();
    _settings.clear();

    const QStringList groups = SettingsManager::groups();
    const QStringList settings = SettingsManager::keys();

    for(const QString &key : groups) {
        LocalConfigModelSetting *setting = new LocalConfigModelSetting(key, nullptr, this);
        connect(setting, &LocalConfigModelSetting::settingChanged, this, &LocalConfigModel::onSettingChanged);
        connect(setting, &LocalConfigModelSetting::settingRemoved, this, &LocalConfigModel::onSettingRemoved);
        _settings << setting;
    }

    for(const QString &key : settings) {
        LocalConfigModelSetting *setting = new LocalConfigModelSetting(key, nullptr, this);
        connect(setting, &LocalConfigModelSetting::settingChanged, this, &LocalConfigModel::onSettingChanged);
        connect(setting, &LocalConfigModelSetting::settingRemoved, this, &LocalConfigModel::onSettingRemoved);
        _settings << setting;
    }
}

QString LocalConfigModel::valueString(const QVariant &value) {
    QString string = VariantConverter::convertToString(value);
    int typeID = value.typeId();

    if(typeID == QMetaType::QByteArray)
        string = tr("(binary)");

    if(typeID == QMetaType::QStringList) {
        string = tr("(%n item(s))", "", value.toStringList().count());
    }

    return string;
}

void LocalConfigModel::onSettingChanged(LocalConfigModelSetting *setting) {
    QModelIndex index = indexFromSetting(setting).siblingAtColumn(ValueColumn);
    if(!index.isValid())
        return;

    emit dataChanged(index, index, {Qt::DisplayRole});
}

void LocalConfigModel::onSettingRemoved(LocalConfigModelSetting * setting) {
    if (!setting)
        return;

    setting->setDeleted();

    LocalConfigModelSetting *parent = setting->parentSetting();

    int row = parent ? parent->indexOfChildSetting(setting)
                     : _settings.indexOf(setting);

    QModelIndex parentIndex = parent ? indexFromSetting(parent) : QModelIndex();

    beginRemoveRows(parentIndex, row, row);

    if (parent)
        parent->removeChildSetting(setting);
    else
        _settings.removeAt(row);

    endRemoveRows();
}
