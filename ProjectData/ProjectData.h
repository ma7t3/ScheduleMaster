#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QObject>
#include <QUndoStack>
#include <QUuid>

#include "Busstop.h"

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
     * This is equivalent to calling !filePath().isEmpty().
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
     * @return A QList of all busstops in the project.
     */
    QList<Busstop *> busstops() const;

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
     * @brief Converts the entire ProjectData to a JSON object (e.g. to save it to a file).
     * @return The QJsonObject
     */
    QJsonObject toJson() const;

protected:
    /**
     * @brief Reads a JSON object and fills the ProjectData with the data from the JSON object (e.g. when read in from a file).
     * @param jsonObject The QJsonObject to read from.
     */
    void fromJson(const QJsonObject &jsonObject);

signals:

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
    QHash<QUuid, Busstop *> _busstops;
};

#endif // PROJECTDATA_H
