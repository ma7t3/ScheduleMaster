#ifndef JAVASCRIPTINTERFACE_H
#define JAVASCRIPTINTERFACE_H

#include <QObject>
#include <QJSEngine>
#include <QJSValue>

#include "ProjectData/ProjectDataItemSet.h"
#include "ProjectData/ProjectDataItemList.h"

class JavaScriptInterface : public QObject {
    Q_OBJECT
public:
    explicit JavaScriptInterface(QObject *parent = nullptr) : QObject{parent} {}

    template<typename T>
    static QJSValue projectDataItemToJsObject(T *item, const QObject *engineHost) {
        QJSEngine *engine = qjsEngine(engineHost);
        return item ? engine->newQObject(item->jsInterface()) : QJSValue::NullValue;
    }

    template<typename T>
    static QJSValue projectDataItemsToJsArray(PDISet<T> items, const QObject *engineHost) {
        QJSEngine *engine = qjsEngine(engineHost);
        QJSValue array = engine->newArray(items.count());

        int i = 0;
        for(T *item : items) {
            array.setProperty(i, engine->newQObject(item->jsInterface()));
            i++;
        }

        return array;
    }

    template<typename T>
    static QJSValue projectDataItemsToJsArray(PDIList<T> items, const QObject *engineHost) {
        QJSEngine *engine = qjsEngine(engineHost);
        QJSValue array = engine->newArray(items.count());

        int i = 0;
        for(T *item : items) {
            array.setProperty(i, engine->newQObject(item->jsInterface()));
            i++;
        }

        return array;
    }

signals:
};

#endif // JAVASCRIPTINTERFACE_H
