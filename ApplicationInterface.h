#ifndef APPLICATIONINTERFACE_H
#define APPLICATIONINTERFACE_H

#include <QObject>

class QUndoStack;
class ProjectData;

class ApplicationInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(ApplicationInterface)

private:
    explicit ApplicationInterface() : QObject(nullptr) {}

public:
    static void init(ProjectData *projectData);

    static ApplicationInterface *instance();

    static ProjectData *projectData();

    static QUndoStack *undoStack();

signals:
    void newProject();
    void openProject();
    void openProjectFromFile(const QString &filePath);
    void saveProject();
    void saveProjectAs();
    void saveProjectToFile(const QString &filePath);
    void closeProject();
    void quitApplication();

    void removeProjectFromRecentList(const QString &filePath);

    void openPlugins();
    void openPreferences();
    void openConfiguration();
    void openProjectSettings();

private:
    static inline ProjectData *_projectData;
};

#endif // APPLICATIONINTERFACE_H
