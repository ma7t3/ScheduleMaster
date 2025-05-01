#ifndef GLOBALCONFIGHANDLERSIGNALS_H
#define GLOBALCONFIGHANDLERSIGNALS_H

#include <QObject>

class GlobalConfigHandlerSignals : public QObject {
    Q_OBJECT
public:
    explicit GlobalConfigHandlerSignals(QObject *parent) : QObject(parent) {}

signals:
    void itemAdded(const QString &id);
    void itemRemoved(const QString &id);
};

#endif // GLOBALCONFIGHANDLERSIGNALS_H
