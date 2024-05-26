#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QDate>

#include <QJsonObject>
#include <QJsonArray>

class AppInfo : public QObject {
    Q_OBJECT
public:
    explicit AppInfo(QObject *parent = nullptr);

    enum BuildType {
        Release,
        Beta,
        Alpha,
        Unofficial
    };

    class AppVersion;

    static AppVersion *currentVersion();
    static BuildType currentBuildType();

    static int versionCount();
    static AppVersion *olderVersion(const int &index);
    static AppVersion *version(const QString &name);
    static bool fileFormatChangesSinceVersion(AppVersion *);

    static const inline QRegularExpression VersionNameRegex = QRegularExpression("^(?<major>[0-9]+)\\.(?<minor>[0-9]+)\\.(?<patch>[0-9]+)-(?<type>.+)$");

private:
    static inline QList<AppVersion *> _versions;

signals:
};

class AppInfo::AppVersion : public QObject {
    Q_OBJECT
public:
    explicit AppVersion(QObject *parent = nullptr);
    explicit AppVersion(QObject *parent = nullptr, const QJsonObject & = {});
    bool operator>(const AppInfo::AppVersion &other);

    QString name() const;
    QDate releaseDate() const;

    QJsonArray changelogCompact() const;
    QJsonObject changelogDetailed() const;

    QString changelogCompactHtml() const;
    QString changelogDetailedHtml(const int &headlineLevel) const;

    bool changedFileFormat() const;

    int majorRelease() const;
    int minorRelease() const;
    int patchLevel() const;
    QString buildID() const;
    BuildType buildType() const;

    bool isCurrentVersion() const;

protected:
    void fromJson(const QJsonObject &);

    QString changeLogToHtml(const QJsonArray &) const;
    QString changeLogItemToHtml(const QJsonObject &) const;

private:
    QString _name;
    QDate _releaseDate;
    QJsonArray _changelogCompact;
    QJsonObject _changelogDetailed;
    bool _changedFileFormat;

    int _versionNumber[3];
    BuildType _buildType;
    QString _buildID;

signals:
};

#endif // APPINFO_H
