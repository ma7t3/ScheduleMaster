#ifndef LINE_H
#define LINE_H

#include "LineDirection.h"

#include <QColor>

class Line : public ProjectDataItem {
    Q_OBJECT
public:
    explicit Line(QObject *parent, const QUuid &id = QUuid());
    explicit Line(QObject *parent, const QJsonObject&);

    bool operator<(const Line &other) const;


    struct Data {
        QString name, description;
        QColor color;
        QVector<LineDirection *> directions;
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

    int directionCount() const;
    LineDirection *direction(const QUuid &id) const;
    QVector<LineDirection *> directions() const;
    void appendDirection(LineDirection *);
    void insertDirection(const int &index, LineDirection *);
    void removeDirection(LineDirection *);
    void removeDirection(const QUuid &id);


    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    Line::Data _data;
};

#endif // LINE_H
