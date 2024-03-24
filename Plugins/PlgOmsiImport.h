#ifndef PLGOMSIIMPORT_H
#define PLGOMSIIMPORT_H

#include <QThread>
#include <QProgressBar>

#include "ProjectData/projectdata.h"

class PlgOmsiImport : public QThread {
    Q_OBJECT
public:
    PlgOmsiImport(QObject *parent, ProjectData *projectData = nullptr);
    void run() override;

    void setMapDirectory(const QString &path);
    void setTripMask(const QString &tripMask);
    void setSelectedTrips(const QStringList &selectedTrips);
    void setSelectedLines(const QStringList &selectedLines);

signals:
    void newFileStarted(int, QString);

private:
    WeekDays importWeekDays(const QString &str);

    int countCommonBusstopSequences(Route *, Route *) const;

    QString _mapDir;
    QRegularExpression _tripMask;
    QStringList _trips, _lines;
    ProjectData *projectData;
};

#endif // PLGOMSIIMPORT_H
