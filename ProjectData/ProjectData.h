#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include "Busstop.h"
#include "Line.h"
#include "ProjectDataItemSet.h"

#include <QObject>
#include <QUuid>

class QUndoStack;

/**
 * @class ProjectData
 * @brief The ProjectData class is the master class that holds any relevant data of a ScheduleMaster project.
 *
 * One ProjectData instance always represents one ScheduleMaster projects. Since ScheduleMaster doesn't support multiple projects opened at the same time,
 * there is always only one ProjectData instance which is created within the MainWindow.
 */

class ProjectData : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new ProjectData object.
     * @param parent The QObject-parent
     */
    explicit ProjectData(QObject *parent = nullptr);

    /**
     * @brief Returns the file path of the project file that is currently loded.
     * This can be an empty string if the project was created new and never saved.
     *
     * See also isKnownFile() and setFilePath().
     * @return The file path.
     */
    QString filePath() const;

    /**
     * @brief Returns whether the project is associated with a project file.
     *
     * This is equivalent to calling `!filePath().isEmpty()`.
     * See also filePath() and setFilePath().
     * @return whether the project is associated with a project file or not.
     */
    bool isKnownFile() const;

    /**
     * @brief Sets the file path this project is associated with.
     *
     *  See also filePath() and isKnownFile().
     *  @param newFilePath The new file path to be set
     */

    void setFilePath(const QString &newFilePath);

    /**
     * @brief Returns a pointer to the project's undo stack
     * @return The QUndoStack pointer.
     */
    QUndoStack *undoStack() const;

    /**
     * @brief Resets the entire project data (e.g. if the project was closed). This also resets the undoStack.
     *
     * See also undoStack().
     */
    void reset();

    /**
     * @brief Returns the number of busstops in the project.
     *
     * See also busstops().
     * @return The number of busstops in the project
     */
    int busstopCount() const;

    /**
     * @brief Searches for a busstop by its UUID.
     * @param id The UUID of the busstop to search for.
     * @return A pointer to the busstop if it was found, otherwise nullptr.
     */
    Busstop *busstop(const QUuid &id) const;

    /**
     * @brief Searches for a busstop by its name.
     * @param name The name of the busstop to search for.
     * @return A pointer to the busstop if it was found, otherwise nullptr.
     */
    Busstop *findBusstopByName(const QString &name) const;

    /**
     * @brief Returns a list of all busstops in the project.
     *
     * See also busstopCount().
     * @return A ProjectDataItemSet of all busstops in the project.
     */
    PDISet<Busstop> busstops() const;

    /**
     * @brief Creates a new busstop object with an optionally given parent
     * @param parent The parent QObject of the busstop object. If not given, the ProjectData will take ownership.
     * @return The created busstop object
     */
    Busstop *createBusstop(QObject *parent = nullptr);

    /**
     * @brief Creates a new busstop object based on the given QJsonObject.
     * @param jsonObject The QJsonObject to read the busstop data from.
     * @return The created busstop object
     */
    Busstop *createBusstop(const QJsonObject &jsonObject);

    /**
     * @brief Adds a busstop to the project.
     * @param busstop The Busstop to add.
     */
    void addBusstop(Busstop *busstop);

    /**
     * @brief Removes a busstop from the project. This does nothing if the given busstop is not part of the project or is nullptr.
     * @param busstop The Busstop to remove.
     */
    void removeBusstop(Busstop *busstop);

    /**
     * @brief Removes a busstop from the project by its UUID. This does nothing if there was no busstop found that matches the given UUID.
     * @param id the Busstop's id to search for.
     */
    void removeBusstop(const QUuid &id);

    /**
     * @brief Returns the busstop that contains a given platform.
     *
     * This can return nullptr if the platform does not belong to any busstop.
     * @return The Busstop that contains the platform or nullptr if not found.
     */
    Busstop *busstopOfPlatform(BusstopPlatform *);

    /**
     * @brief Returns the number of lines in the project.
     *
     * See also lines().
     * @return The number of lines in the project
     */
    int lineCount() const;

    /**
     * @brief Searches for a line by its UUID.
     * @param id The UUID of the line to search for.
     * @return A pointer to the line if it was found, otherwise nullptr.
     */
    Line *line(const QUuid &id) const;

    /**
     * @brief Searches for a line by its name.
     * @param name The name of the line to search for.
     * @return A pointer to the line if it was found, otherwise nullptr.
     */
    Line *findLineByName(const QString &name) const;

    /**
     * @brief Returns a list of all lines in the project.
     *
     * See also lineCount().
     * @return A ProjectDataItemSet of all lines in the project.
     */
    PDISet<Line> lines() const;

    /**
     * @brief Creates a new line object with an optionally given parent
     * @param parent The parent QObject of the line object. If not given, the ProjectData will take ownership.
     * @return The created line object
     */
    Line *createLine(QObject *parent = nullptr);

    /**
     * @brief Creates a new line object based on the given QJsonObject.
     * @param jsonObject The QJsonObject to read the line data from.
     * @return The created line object
     */
    Line *createLine(const QJsonObject &jsonObject);

    /**
     * @brief Adds a line to the project.
     * @param line The Line to add.
     */
    void addLine(Line *line);

    /**
     * @brief Removes a line from the project. This does nothing if the given line is not part of the project or is nullptr.
     * @param line The Line to remove.
     */
    void removeLine(Line *line);

    /**
     * @brief Removes a line from the project by its UUID. This does nothing if there was no line found that matches the given UUID.
     * @param id the Line's id to search for.
     */
    void removeLine(const QUuid &id);

    /**
     * @brief Returns whether setJson() is currently beeing executed.
     * @return The execution status of setJson().
     */
    bool isLoadingJson() const;

    /**
     * @brief Converts the entire ProjectData to a JSON object (e.g. to save it to a file).
     * @return The QJsonObject
     */
    QJsonObject toJson() const;

    /**
     * @brief Reads a JSON object and fills the ProjectData with the data from the JSON object (e.g. when read in from a file).
     *
     * Depending on the size of the json object, this method can take a long time.
     * For this reason, you can specifiy a cancelRequested callback function pointer that will be called periodically and this function will cancel itself if the callback returned true.
     * @param jsonObject The QJsonObject to read from.
     * @param cancelRequested A callback function pointer to check periodically if a cancel is requested.
     * @return Returns true if the method finished successfully or false if it has been canceled before
     */
    bool setJson(const QJsonObject &jsonObject, std::function<bool()> cancelRequested = [](){return false;});

signals:

    /**
     * @brief This signal is emited once when loading the project data from a json object unsing the setJson method.
     * @param maximum The total amount of elements inside the json that will be loaded.
     */
    void progressMaximum(const int &maximum);

    /**
     * @brief This signal is emited while loading the project data from a json object using the setJson method.
     * @param currentProgress The amount of elements that have been loaded so far.
     */
    void progressUpdate(const int &currentProgress);

    /**
     * @brief cleared This signal is emited whenever the clear() method was called after all items have been removed and the object returned to an empty state.
     */
    void cleared();

    /**
     * @brief busstopAdded This signal is emited whenever a busstop is added to the project.
     */
    void busstopAdded(Busstop *);

    /**
     * @brief busstopRemoved This signal is emited whenever a busstop is removed from the project.
     */
    void busstopRemoved(Busstop *);

    /**
     * @brief lineAdded This signal is emited whenever a line is added to the project.
     */
    void lineAdded(Line *);

    /**
     * @brief lineRemoved This signal is emited whenever a line is removed from the project.
     */
    void lineRemoved(Line *);

private:
    /**
     * @brief The file path of the project file that is currently loaded. This can be an empty string if the file was never saved.
     *
     * See also filePath(), setFilePath() and isKnownFile().
     */
    QString _filePath;

    /**
     * @brief The undo stack of the project.
     *
     * See also undoStack().
     */
    QUndoStack *_undoStack;

    /**
     * @brief A hash that contains all busstops of the project. The key is the UUID of the busstop.
     *
     * See also busstopCount(), busstops(), busstop(), addBusstop() and removeBusstop().
     */
    PDISet<Busstop> _busstops;

    /**
     * @brief A hash that contains all lines of the project. The key is the UUID of the line.
     *
     * See also lineCount(), lines(), line(), addLine() and removeLine().
     */
    PDISet<Line> _lines;

    /**
     * @brief A boolean that is set to true while setJson is being executed, otherwise false.
     *
     * See also isLoadingJson() and setJson().
     */
    bool _loadingJson;
};

#endif // PROJECTDATA_H
