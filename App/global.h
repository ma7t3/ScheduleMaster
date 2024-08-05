#ifndef GLOBAL_H
#define GLOBAL_H

#include <QColor>

class global {
public:
    static QColor getContrastColor(QColor c) {
        c = c.toHsl();
        if(c.lightness() >= 128)
            return Qt::black;
        else
            return Qt::white;
    }
};

#endif // GLOBAL_H
