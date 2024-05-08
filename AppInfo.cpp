#include "AppInfo.h"

#include "localconfig.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QApplication>

QList<AppInfo::AppVersion *> AppInfo::_versions;

const QRegularExpression AppInfo::VersionNameRegex = QRegularExpression("^[V|v](?<major>[0-9]+)\\.(?<minor>[0-9]+)\\.(?<patch>[0-9]+)-(?<type>.+)$");

AppInfo::AppInfo(QObject *parent) : QObject(parent) {

    QFile f(":versions.json");
    if(!f.open(QFile::ReadOnly)) {
        qWarning() << "Couldn't load app version history";
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);
    if(error.error != 0) {
        qWarning() << "Couldn't load app version history - invalid json: " << error.errorString();
        return;
    }

    QJsonArray versions = doc.array();
    for(int i = 0; i < versions.count(); i++) {
        QJsonObject obj = versions[i].toObject();
        if(!obj.value("enabled").toBool() || !VersionNameRegex.match(obj.value("name").toString()).hasMatch())
            continue;

        _versions << new AppVersion(qApp, obj);
    }

    std::sort(_versions.begin(), _versions.end(), [](AppVersion *a, AppVersion *b) {return *a > *b;});

    f.close();
    qInfo() << "load app version history.";
}

AppInfo::AppVersion *AppInfo::currentVersion() {
    return _versions.first();
}

AppInfo::BuildType AppInfo::currentBuildType() {
    return currentVersion()->buildType();
}

int AppInfo::versionCount() {
    return _versions.count();
}

AppInfo::AppVersion *AppInfo::olderVersion(const int &index) {
    if(index < 0 || index >= _versions.count())
        return nullptr;

    return _versions[index];
}

AppInfo::AppVersion::AppVersion(QObject *parent) : QObject(parent) {}

AppInfo::AppVersion::AppVersion(QObject *parent, const QJsonObject &obj) : QObject(parent) {
    fromJson(obj);
}

bool AppInfo::AppVersion::operator>(const AppVersion &other) {
    if(majorRelease() != other.majorRelease())
        return majorRelease() > other.majorRelease();

    if(minorRelease() != other.minorRelease())
        return majorRelease() > other.minorRelease();

    if(patchLevel() != other.patchLevel())
        return patchLevel() > other.patchLevel();

    if(releaseDate() != other.releaseDate())
        return releaseDate() > other.releaseDate();

    return true;
}

QString AppInfo::AppVersion::name() const {
    return _name;
}

QDate AppInfo::AppVersion::releaseDate() const {
    return _releaseDate;
}

QJsonArray AppInfo::AppVersion::changelogCompact() const {
    return _changelogCompact;
}

QJsonObject AppInfo::AppVersion::changelogDetailed() const {
    return _changelogDetailed;
}

QString AppInfo::AppVersion::changelogCompactHtml() const {
    return changeLogToHtml(_changelogCompact);
}

QString AppInfo::AppVersion::changelogDetailedHtml(const int &headlineLevel) const {
    QString headlineTag = "h" + QString::number(headlineLevel);
    QString text = "";

    text += "<" + headlineTag + ">" + tr("Bugfixes") + "</" + headlineTag + ">";
    text += changeLogToHtml(_changelogDetailed.value("bugfixes").toArray());

    text += "<" + headlineTag + ">" + tr("Improvements") + "</" + headlineTag + ">";
    text += changeLogToHtml(_changelogDetailed.value("improvements").toArray());

    text += "<" + headlineTag + ">" + tr("New Features") + "</" + headlineTag + ">";
    text += changeLogToHtml(_changelogDetailed.value("new_features").toArray());

    text += "<" + headlineTag + ">" + tr("Miscellaneous") + "</" + headlineTag + ">";
    text += changeLogToHtml(_changelogDetailed.value("miscellaneous").toArray());

    return text;
}

bool AppInfo::AppVersion::changedFileFormat() const {
    return _changedFileFormat;
}

int AppInfo::AppVersion::majorRelease() const {
    return _versionNumber[0];
}

int AppInfo::AppVersion::minorRelease() const {
    return _versionNumber[1];
}

int AppInfo::AppVersion::patchLevel() const {
    return _versionNumber[2];
}

QString AppInfo::AppVersion::buildID() const {
    return _buildID;
}

 AppInfo::BuildType AppInfo::AppVersion::buildType() const {
    return _buildType;
}

void AppInfo::AppVersion::fromJson(const QJsonObject &obj) {
    _name = obj.value("name").toString();
    _releaseDate = QDate::fromString(obj.value("release_date").toString("1970-01-01"), "yyyy-MM-dd");

    _changelogCompact  = obj.value("changelog_compact").toArray();
    _changelogDetailed = obj.value("changelog_detailed").toObject();

    _buildID = obj.value("build_id").toString();

    QRegularExpression regex(VersionNameRegex);
    QRegularExpressionMatch match = regex.match(_name);
    if(match.hasMatch()) {
        _versionNumber [0] = match.captured("major").toInt();
        _versionNumber [1] = match.captured("minor").toInt();
        _versionNumber [2] = match.captured("patch").toInt();
        QString type = match.captured("type");
        if(type == "release") _buildType = Release;
        else if(type == "beta") _buildType = Beta;
        else if(type == "alpha") _buildType = Alpha;
        else _buildType = Unofficial;
    }
}

QString AppInfo::AppVersion::changeLogToHtml(const QJsonArray &arr) const {
    if(arr.isEmpty())
        return "<i>" + tr("none") + "</i>";

    QString str = "<ul>";

    for(int i = 0; i < arr.count(); i++) {
        QJsonObject current = arr[i].toObject();
        str += changeLogItemToHtml(current);

        if(current.contains("sub_items")) {
            str += changeLogToHtml(current.value("sub_items").toArray());
        }
    }

    str += "</ul>";

    return str;
}

QString AppInfo::AppVersion::changeLogItemToHtml(const QJsonObject &obj) const {
    QString text;

    if(LocalConfig::language() == LocalConfig::German && obj.contains("de-DE"))
        text = obj.value("de-DE").toString();
    else
        text = obj.value("en-US").toString();

    return "<li>" + text + "</li>";
}
