#ifndef VARIANTCONVERTER_H
#define VARIANTCONVERTER_H

#include <QObject>
#include <QVariant>

class VariantConverter : public QObject {
    Q_OBJECT
public:
    explicit VariantConverter(QObject *parent = nullptr);

    static QVariant convert(const QVariant &value, const QMetaType::Type &type);
    static QString  convertToString(const QVariant &value);
    static QVariant convertFromJson(const QJsonValue &value, const QMetaType::Type &type);

signals:
};

#endif // VARIANTCONVERTER_H
