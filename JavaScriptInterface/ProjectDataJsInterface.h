#ifndef PROJECTDATAJSINTERFACE_H
#define PROJECTDATAJSINTERFACE_H

#include <QObject>
#include <QJSValue>

class ProjectData;

class ProjectDataJsInterface : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QJSValue busstops READ busstops NOTIFY busstopsChanged)
    Q_PROPERTY(QJSValue lines READ lines NOTIFY linesChanged)

public:
    explicit ProjectDataJsInterface(QObject *parent = nullptr);

    void setTarget(ProjectData *target);

    QString filePath() const;
    QJSValue busstops() const;
    QJSValue lines() const;

    Q_INVOKABLE QJSValue busstop(const QString &id) const;
    Q_INVOKABLE QJSValue line(const QString &id) const;

signals:
    void filePathChanged();
    void busstopsChanged();
    void linesChanged();

private:
    ProjectData *_target;
};

#endif // PROJECTDATAJSINTERFACE_H
