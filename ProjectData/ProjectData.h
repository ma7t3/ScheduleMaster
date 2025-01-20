#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QObject>
#include <QUndoStack>
#include <QUuid>

#include "Busstop.h"

class ProjectData : public QObject {
    Q_OBJECT
public:
    explicit ProjectData(QObject *parent = nullptr);

    QString filePath() const;
    bool isKnownFile() const;
    void setFilePath(const QString &);

    QUndoStack *undoStack() const;

    void reset();

    int busstopCount() const;
    Busstop *busstop(const QUuid &id) const;
    Busstop *findBusstopByName(const QString &name) const;
    QList<Busstop *> busstops() const;
    void addBusstop(Busstop *);
    void removeBusstop(Busstop *);
    void removeBusstop(const QUuid &id);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:

private:
    QString _filePath;
    QUndoStack *_undoStack;

    QMap<QUuid, Busstop *> _busstops;
};

#endif // PROJECTDATA_H
