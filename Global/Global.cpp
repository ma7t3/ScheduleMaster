#include "Global.h"

Global::Global() :
    QObject(nullptr) {
}

Global::~Global() {

}

Global *Global::instance() {
    static Global instance;
    return &instance;
}

QColor Global::contrastColor(const QColor &c) {
    return sqrt(
       0.299 * pow(c.redF(), 2) +
       0.587 * pow(c.greenF(), 2) +
       0.114 * pow(c.blueF(), 2)
       ) >= 0.5 ? Qt::black : Qt::white;
}

QJsonArray Global::stringListToJsonArray(const QStringList &list) {
    return QJsonArray::fromStringList(list);
}

QStringList Global::jsonArrayToStringList(const QJsonArray &array) {
    QStringList list;
    for(const QJsonValue &val : array)
        list << val.toString();
    return list;
}

QVariant Global::convertVariant(const QVariant &value, QMetaType::Type type) {
    switch (type) {
    case QMetaType::QByteArray:
        return QByteArray(value.toByteArray());

    case QMetaType::QStringList:
        return value.toStringList();

    case QMetaType::QDate:
        return QDate::fromString(value.toString(), Qt::ISODate);

    case QMetaType::QTime:
        return QTime::fromString(value.toString(), Qt::ISODate);

    case QMetaType::QDateTime:
        return QDateTime::fromString(value.toString(), Qt::ISODate);

    case QMetaType::QColor:
        return QColor(value.toString());


    case QMetaType::QSize:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 2)
                return QSize(parts[0].toInt(), parts[1].toInt());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 2)
                return QSize(arr[0].toInt(), arr[1].toInt());
        }
        return QSize();

    case QMetaType::QPoint:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 2)
                return QPoint(parts[0].toInt(), parts[1].toInt());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 2)
                return QPoint(arr[0].toInt(), arr[1].toInt());
        }
        return QPoint();

    case QMetaType::QRect:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 4)
                return QRect(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 4)
                return QRect(arr[0].toInt(), arr[1].toInt(), arr[2].toInt(), arr[3].toInt());
        }
        return QRect();

    case QMetaType::QUuid:
        return QUuid(value.toString());

    case QMetaType::QUrl:
        return QUrl(value.toString());

    case QMetaType::QLocale:
        return QLocale(value.toString());

    default:
        break;
    }

    return value;
}

QVariant Global::convertJsonToVariant(const QJsonValue &value, QMetaType::Type type) {
    return convertVariant(value.toVariant(), type);
}
