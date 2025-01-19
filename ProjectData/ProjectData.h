#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <QObject>
#include <QUndoStack>

class ProjectData : public QObject {
    Q_OBJECT
public:
    explicit ProjectData(QObject *parent = nullptr);

    QString filePath() const;
    bool isKnownFile() const;
    void setFilePath(const QString &);

    QUndoStack *undoStack() const;

    void reset();

signals:

private:
    QString _filePath;
    QUndoStack *_undoStack;
};

#endif // PROJECTDATA_H
