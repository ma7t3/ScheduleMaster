#ifndef CRASHDETECTOR_H
#define CRASHDETECTOR_H

#include <QObject>

#include "Global/Singleton.h"

/**
 * @class CrashDetector
 * @brief The CrashDetector class is responsible for detecting if the application crashed in the last session.
 *
 * This class provides a singleton instance to check for a crash occurrence.
 * It utilizes the SettingsManager to manage the crash detection.
 */
class CrashDetector : public Singleton<CrashDetector> {
    friend class Singleton<CrashDetector>;
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(CrashDetector)

private:
    /**
     * @brief Constructs a CrashDetector object.
     * @param parent The parent QObject.
     */
    CrashDetector() : Singleton<CrashDetector>() {}

    /**
     * @brief Destroys the CrashDetector object.
     *
     * This saves that the application was closed normally and didn't crash.
     */
    ~CrashDetector();

public:
    /**
     * @brief Initializes the CrashDetector instance.
     */
    static void init();

    /**
     * @brief Returns if a crash was detected.
     * @return True if a crash was detected, otherwise false.
     */
    static bool crashDetected();

private:
    static inline bool _lastCrashDetected = false; ///< Indicates the last crash detection status.
};

#endif // CRASHDETECTOR_H

