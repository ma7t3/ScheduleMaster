#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H


#include <QUuid>
#include <QColor>

class global {

public:
    static const int releaseType = 1;
    static const int primaryVersion = 2;
    static const int secondaryVersion = 1;
    static const int tertiaryVersion = 0;

    static QString getNewID() {
        return QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    static QColor getContrastColor(QColor c) {
        c = c.toHsl();
        if(c.lightness() >= 128)
            return Qt::black;
        else
            return Qt::white;
    }
};

#endif // GLOBAL_H
