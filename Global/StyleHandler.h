#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

#include <QObject>
#include <QStyle>
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>

#include "Global/GlobalConfig.h"

class StyleHandler : public QObject {
    Q_OBJECT
private:
    /// Private construtor for the signleton instance
    explicit StyleHandler();

    /// Private destructor for the singleton instance
    ~StyleHandler();

public:
    /// Returns the singleton instance of the StyleHandler class.
    static StyleHandler *instance();

    static void init();

    StyleHandler(const StyleHandler &) = delete;
    StyleHandler & operator=(const StyleHandler &) = delete;

    static void registerStyleClass(const QString &id, QStyle *style);

    static void applyStyle(const QString &id);
    static void applyAccentColor(const QString &id);

signals:
    void styleChanged(QString);
    void accentColorChanged(QString);

private:
    static inline QString _systemStyleName;
    static inline QHash<QString, QStyle *> _styleClasses;
};

#endif // STYLEHANDLER_H
