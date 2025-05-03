#ifndef STYLEHANDLER_H
#define STYLEHANDLER_H

#include <QObject>
#include <QStyle>
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QStyleHints>

#include "Global/StyleManager.h"

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

public slots:
    static void applyStyle(const QString &id = StyleManager::currentStyle());
    static void applyColorScheme(const Qt::ColorScheme &colorScheme = StyleManager::currentColorScheme());
    static void applyAccentColor(const QString &id = StyleManager::currentAccentColorID());
    static void applyFont(const QString &fontFamily = StyleManager::currentUiFontFamily());

    static void onColorSchemeChange();

protected slots:
    static void applyPalette();

signals:
    void styleChanged(const QString &);
    void accentColorChanged(const QString &);

private:
    static inline QPalette _initPalette;

    static inline QString _systemStyleName;
    static inline QHash<QString, QStyle *> _styleClasses;

    static inline StyleConfig _currentStyle;
    static inline QString _currentAccentColor;
};

#endif // STYLEHANDLER_H
