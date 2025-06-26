#include "VariantConverter.h"

#include <QDateTime>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QUuid>
#include <QUrl>
#include <QJsonValue>

VariantConverter::VariantConverter(QObject *parent) :
    QObject(parent) {
}

QVariant VariantConverter::convert(const QVariant &value, const QMetaType::Type &type) {
    if(value.typeId() == QMetaType::UnknownType) {
        qWarning() << "Cannot from convert unkown type!";
        return QVariant();
    }

    if(value.typeId() == type)
        return value;

    if (value.canConvert(QMetaType(type))) {
        QVariant converted = value;
        converted.convert(QMetaType(type));
        return converted;
    }

    switch (type) {
    case QMetaType::Bool:
        return value.toBool();

    case QMetaType::Int:
        return value.toInt();

    case QMetaType::Float:
        return value.toFloat();

    case QMetaType::QString:
        return value.toString();

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

    case QMetaType::QSizeF:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 2)
                return QSizeF(parts[0].toFloat(), parts[1].toFloat());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 2)
                return QSizeF(arr[0].toFloat(), arr[1].toFloat());
        }
        return QSizeF();

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

    case QMetaType::QPointF:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 2)
                return QPointF(parts[0].toFloat(), parts[1].toFloat());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 2)
                return QPointF(arr[0].toFloat(), arr[1].toFloat());
        }
        return QPointF();

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

    case QMetaType::QRectF:
        if (value.typeId() == QMetaType::QString) {
            QStringList parts = value.toString().split(',');
            if (parts.size() == 4)
                return QRectF(parts[0].toFloat(), parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
        } else if (value.typeId() == QMetaType::QVariantList) {
            QVariantList arr = value.toList();
            if (arr.size() == 4)
                return QRectF(arr[0].toFloat(), arr[1].toFloat(), arr[2].toFloat(), arr[3].toFloat());
        }
        return QRectF();

    case QMetaType::QUuid:
        return QUuid(value.toString());

    case QMetaType::QUrl:
        return QUrl(value.toString());

    case QMetaType::QLocale:
        return QLocale(value.toString());

    default:
        break;
    }

    qWarning().noquote() << "Conversion failed! Cannot convert from " << value.typeName() << " to " << QMetaType(type).name();
    return QVariant();
}

QString VariantConverter::convertToString(const QVariant &value) {
    bool canConvert = QMetaType::canConvert(value.metaType(), QMetaType(QMetaType::QString));
    if(canConvert)
        return value.toString();

    switch(value.typeId()) {
    case QMetaType::QPoint: case QMetaType::QPointF:
        return QString("QPoint(%1, %2)").arg(value.toPointF().x()).arg(value.toPointF().y());

    case QMetaType::QSize: case QMetaType::QSizeF:
        return QString("QSize(%1x%2)").arg(value.toSizeF().width()).arg(value.toSizeF().height());

    case QMetaType::QRect: case QMetaType::QRectF:
        return QString("QRect(%1, %2, %3x%4)").arg(value.toRectF().x()).arg(value.toRectF().y()).arg(value.toRectF().width()).arg(value.toRectF().height());
    }


    return QString("[%1]").arg(value.typeName());
}

QVariant VariantConverter::convertFromJson(const QJsonValue &value, const QMetaType::Type &type) {
    return convert(value.toVariant(), type);
}
