#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

#include <QFile>

class Logger : public QObject {
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    static void handler(QtMsgType type, const QMessageLogContext & context, const QString &message);

private:
    static QString fileName;
    static unsigned int counter;
};

#endif // LOGGER_H
