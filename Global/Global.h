#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>

class Global : public QObject {
    Q_OBJECT
private:
    explicit Global();
    ~Global();

public:
    static Global *instance();

    static QColor contrastColor(const QColor &);
    /* add general application-wide methods here */

    static QJsonArray stringListToJsonArray(const QStringList &list);
    static QStringList jsonArrayToStringList(const QJsonArray &array);

    static void blockSignalsRecursive(QObject *object, const bool &block);

    static QKeySequence parseKeyboardShortcutConfigString(const QJsonValue &value);

    static const inline QString ProjectDataRootObjectName = "PROJECT_DATA_ROOT";

    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;
};

#endif // GLOBAL_H
