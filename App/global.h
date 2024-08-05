#ifndef GLOBAL_H
#define GLOBAL_H

#include <QColor>

class global {
public:
    static QColor getContrastColor(QColor c) {
        return sqrt(
            0.299 * pow(c.redF(), 2) +
            0.587 * pow(c.greenF(), 2) +
            0.114 * pow(c.blueF(), 2)
        ) >= 0.5 ? Qt::black : Qt::white;
    }
};

#endif // GLOBAL_H
