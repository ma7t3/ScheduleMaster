#include "AppInfo.h"

AppInfo::AppInfo() : QObject(nullptr) {

}

AppInfo *AppInfo::instance() {
    static AppInfo instance;
    return &instance;
}

QString AppInfo::currentVersionName() {
    return "DEMO";
}

AppInfo::BuildType AppInfo::currentBuildType() {
    return ReleaseBuildType;
}
