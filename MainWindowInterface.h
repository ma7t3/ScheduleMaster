#ifndef MAINWINDOWINTERFACE_H
#define MAINWINDOWINTERFACE_H

#include <QObject>

class MainWindowInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindowInterface)

private:
    explicit MainWindowInterface()
        : QObject(nullptr)
    {}

public:
    static MainWindowInterface *instance() {
        static MainWindowInterface instance;
        return &instance;
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

};

#endif // MAINWINDOWINTERFACE_H
