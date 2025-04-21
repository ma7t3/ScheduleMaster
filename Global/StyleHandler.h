#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

#include <QObject>
#include <QStyle>
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>

#include "Global/GlobalConfig.h"
#include "Global/LocalConfig.h"

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

    static void applyStyle(const QString &id = LocalConfig::style());
    static void applyAccentColor(const QString &id = LocalConfig::accentColorID());
    static void applyFont(const QString &fontFamily = LocalConfig::uiFontFamily());

protected:
    static void applyPalette();

signals:
    void styleChanged(QString);
    void accentColorChanged(QString);

private:
    static inline QPalette _initPalette;

    static inline QString _systemStyleName;
    static inline QHash<QString, QStyle *> _styleClasses;

    static inline Style _currentStyle;
    static inline QString _currentAccentColor;
};

#endif // STYLEHANDLER_H
