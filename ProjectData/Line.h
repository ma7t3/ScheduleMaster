#ifndef LINE_H
#define LINE_H

#include "ProjectDataItem.h"

#include <QColor>

class Line : public ProjectDataItem {
    Q_OBJECT
public:
    Line(QObject *parent, const QUuid &id = QUuid());
    Line(QObject *parent, const QJsonObject&);

    bool operator<(const Line &other) const;


    struct Data {
        QString name, description;
        QColor color;
        // directions, routes, trips
    };

    Data data() const;
    void setData(const Data &);

    QString name() const;
    void setName(const QString &);

    QString description() const;
    void setDescription(const QString &);

    QColor color() const;
    void setColor(const QColor &);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    Line::Data _data;
};

#endif // LINE_H
