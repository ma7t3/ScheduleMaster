#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectData/ProjectDataItem.h"

class LineDirection : public virtual ProjectDataItem {
    Q_OBJECT
public:
    LineDirection(QObject *parent, const QString &id, const QString &description);
    LineDirection(QObject *parent, const QJsonObject &);
    LineDirection(const LineDirection &);
    LineDirection operator=(const LineDirection &);

    QString description() const;
    void setDescription(const QString &);

    QJsonObject toJson()  const;

protected:
    void copy(const LineDirection &);
    void fromJson(const QJsonObject &);

private:
    QString _description;
};

#endif // LINEDIRECTION_H
