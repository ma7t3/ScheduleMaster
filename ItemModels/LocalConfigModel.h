#ifndef LOCALCONFIGMODEL_H
#define LOCALCONFIGMODEL_H

#include <QAbstractItemModel>

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"

class LocalConfigModelSetting : public QObject {
    Q_OBJECT
public:
    LocalConfigModelSetting(const QString &id, LocalConfigModelSetting *parentSetting = nullptr, QObject *parent = nullptr);

    QString id() const;
    bool isGroup() const;
    SMA::SettingsItem metaData() const;
    bool isUnknown() const;
    QVariant value();

    LocalConfigModelSetting *parentSetting() const;
    int childrenSettingsCount() const;
    LocalConfigModelSetting *childSetting(const int &index) const;
    QList<LocalConfigModelSetting *> childrenSettings() const;
    int indexOfChildSetting(LocalConfigModelSetting *child) const;
    void removeChildSetting(LocalConfigModelSetting *child);

    bool deleted() const;
    void setDeleted();

protected:
    void onSettingChanged(const QString &id, const QVariant &value);
    void onSettingRemoved(const QString &id);

signals:
    void settingChanged(LocalConfigModelSetting *setting, const QVariant &value);
    void settingRemoved(LocalConfigModelSetting *setting);

private:
    QString _id;
    SMA::SettingsItem _metaData;
    bool _unkown, _deleted;
    QVariant _value;
    LocalConfigModelSetting *_parent;
    QList<LocalConfigModelSetting *> _children;
};

class LocalConfigModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit LocalConfigModel(QObject *parent = nullptr);

    enum Columns {
        IDColumn,
        DescriptionColumn,
        TypeColumn,
        ValueColumn
    };

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString settingID(const QModelIndex &index) const;
    bool settingIsDeleted(const QModelIndex &index) const;
    bool settingIsGroup(const QModelIndex &index) const;
    QVariant settingValue(const QModelIndex &index) const;
    QStringList settingsIDList(const QModelIndexList &indexList);

public slots:
    void setSettingPreview(const QModelIndex &index, const QVariant &value);
    void endPreview();

    void reload();

protected:
    LocalConfigModelSetting *settingItem(const int &index) const;
    QModelIndex indexFromSetting(LocalConfigModelSetting *setting) const;
    void reloadSettings();

    static QString valueString(const QVariant &value);

    void onSettingChanged(LocalConfigModelSetting *setting);
    void onSettingRemoved(LocalConfigModelSetting *setting);

private:
    QList<LocalConfigModelSetting *> _settings;

    QVariant _previewValue;
    QModelIndex _previewIndex;
};

#endif // LOCALCONFIGMODEL_H
