#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>

class GlobalConfig : public QObject {
    Q_OBJECT
public:
    GlobalConfig(QObject *parent);
    static const QString &currentVersion();

private:
    static const QString _currentVersion;
};

#endif // GLOBALCONFIG_H
