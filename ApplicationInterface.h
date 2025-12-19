#ifndef APPLICATIONINTERFACE_H
#define APPLICATIONINTERFACE_H

#include <QObject>

#include "src/projectdata/ProjectData.h"

class ApplicationInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(ApplicationInterface)

private:
    explicit ApplicationInterface() : QObject(nullptr) {}

public:
    static void init(ProjectData *projectData) {
        _projectData = projectData;
    }

    static ApplicationInterface *instance() {
        static ApplicationInterface instance;
        return &instance;
    }

    static ProjectData *projectData() {
        return _projectData;
    }

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
