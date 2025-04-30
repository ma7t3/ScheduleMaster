#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>

class AppInfo : public QObject {
    Q_OBJECT
private:
    explicit AppInfo();

public:
    enum BuildType {
        ReleaseBuildType,
        BetaBuildType,
        AlphaBuildType,
        UnofficialBuildType
    };

    static AppInfo *instance();

    static QString currentVersionName();
    static BuildType currentBuildType();

signals:
};

#endif // APPINFO_H
