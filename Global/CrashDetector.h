#ifndef CRASHDETECTOR_H
#define CRASHDETECTOR_H

#include <QObject>

class CrashDetector : public QObject {
    Q_OBJECT
private:
    explicit CrashDetector(QObject *parent);
    ~CrashDetector();

public:
    static CrashDetector *instance();
    static void init();

    CrashDetector(const CrashDetector &other) = delete;
    CrashDetector &operator=(const CrashDetector &other) = delete;

    static bool crashDetected();

signals:

private:
    static inline bool _lastCrashDetected = false;
};

#endif // CRASHDETECTOR_H
