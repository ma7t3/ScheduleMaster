#ifndef PROJECTFILEHANDLER_H
#define PROJECTFILEHANDLER_H

#include <QThread>

class ProjectData;

/**
 * @brief The ProjectFileHandler class provides reading and writing ProjectData from/to files within a separate thread.
 */
class ProjectFileHandler : public QThread {
    Q_OBJECT
public:

    /**
     * @brief Creates a new ProjectFileHandler obejct.
     * @param projectData The ProjectData instance to work with
     * @param parent The QObject parent
     */
    explicit ProjectFileHandler(ProjectData *projectData, QObject *parent = nullptr);

    /**
     * @brief Destroys the ProjectFileHandler but requests an interuption before and than waits for finishing to ensure a safe ending of the current process.
     */
    ~ProjectFileHandler();

    /**
     * @brief The FinishReason enum defines a set of possible reasons why the thread finished and endet execution.
     */
    enum FinishReason {
        SuccessfulReason, ///< The thread finished because the work was successful and finished as planned.
        ErrorReason, ///< The thread finished because an error occured.
        CancelReason ///< The thread finished because it was canceled by the QThread::requestInteruption call.
    };

    /**
     * @brief Start reading the file from filePath into _projectData.
     *
     * This function will call beforeStart(filePath).
     * @param filePath The file path to read from
     */
    void readFile(const QString &filePath);

    /**
     * @brief Start saveing the _projectData to filePath.
     *
     * This function will call beforeStart(filePath).
     * @param filePath The file path to save to
     * @param compress Compress the data using qCompress?
     */
    void saveFile(const QString &filePath, const bool &compress);

protected:

    /**
     * @brief Runs the actual thread actitivity (reading or saving, based on the current _workMode value)
     */
    virtual void run() override;

    /**
     * @brief This function will be executed right before the run() method inside the main thread.
     *
     * It moves the _projectData to this thread and blocks it's signals and set's _filePath to filePath.
     * Then it will call start() itself.
     * @param filePath
     */
    void beforeStart(const QString &filePath);

    /**
     * @brief Checks if the next step can start and starts it if possible.
     *
     * This function emits the progressSetpChanged signal and returns true or otherwise if the thread should be interupted, sets the finishReason to CancelReason and return false.
     * @param text The description of the next step as displayed in the UI.
     * @return true if the next step can start or false if an interuption was requested. You should always return immideately from the run() method if this function return false.
     */
    bool startStep(const QString &text);

    /**
     * @brief Finishes the run() method.
     *
     * This function should always be called when ending the run method. It is executed in this thread.
     * @param reason The reason, why the thread finished.
     */
    void finishRun(const FinishReason &reason);

    /**
     * @brief The WorkMode enum describes, what activity the thread should do
     */
    enum WorkMode {
        ReadMode, ///< unknown activity, does nothing
        WriteMode, ///< writing _projectData to a file
        UnknownMode ///< reading from a file into _projectData
    };

signals:
    /**
     * @brief This signal is emited whenever aa new progress step starts.
     * @param text The step's text to display in the UI
     */
    void progressStepChanged(const QString &text);

    /**
     * @brief This signal is emited whenever the maximum count of events in the current step changes.
     * @param maximum The maximum count of events
     */
    void progressStepMaximum(const int &maximum);

    /**
     * @brief This signals is emited whenever the progress of the current step changes
     * @param value The new event index
     */
    void progressStepUpdate(const int &value);

private:

    /// The current work mode
    WorkMode _workMode;

    /// Should the saving file be compressed using qCompress?
    bool _compress;

    /// The file path to read from or save to
    QString _filePath;

    /// The ProjectData instance to write to/read from
    ProjectData *_projectData;

    /// Static definition of the file header in uncompressed files (depricated)
    static const inline QByteArray uncompressedHeader = "SMP0";

    /// Static definiton of the file header in compressed files
    static const inline QByteArray compressedHeader   = "SMP1";
};

#endif // PROJECTFILEHANDLER_H
