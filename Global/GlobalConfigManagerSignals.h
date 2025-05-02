#ifndef GLOBALCONFIGMANAGERSIGNALS_H
#define GLOBALCONFIGMANAGERSIGNALS_H

#include <QObject>

class GlobalConfigManagerSignals : public QObject {
    Q_OBJECT
public:
    explicit GlobalConfigManagerSignals(QObject *parent) : QObject(parent) {}

signals:
    void itemAdded(const QString &id);
    void itemRemoved(const QString &id);
};

#endif // GLOBALCONFIGMANAGERSIGNALS_H
