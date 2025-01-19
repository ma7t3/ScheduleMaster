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
