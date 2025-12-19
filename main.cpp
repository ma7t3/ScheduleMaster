#include "src/ui/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTranslator>
#include <QSplashScreen>
#include <QThread>

#include "src/namespace.h"
#include "src/core/ApplicationInterfaceImpl.h"

#include "Global/Logger.h"
#include "Global/SettingsManager.h"
#include "Global/ActionController.h"
#include "Global/StyleHandler.h"
#include "Global/StyleManager.h"
#include "Global/LanguageManager.h"
#include "Global/DockManager.h"
#include "Global/WorkspaceManager.h"
#include "Global/FolderLocationManager.h"
#include "Global/ActionManager.h"
#include "Global/CrashDetector.h"
#include "Global/IconSetManager.h"
#include "Global/IconController.h"

QPair<QColor, QString> splashScreenConfig() {
    QString imagePath = ":/Splashscreen/slpashscreen_light.png";
    QColor messageColor = QColor(0, 0, 0);
    if(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        imagePath = ":/Splashscreen/splashscreen_dark.png";
        messageColor = QColor(255, 255, 255);
    }

    QDate now = QDate::currentDate();

    if(now.dayOfYear() > 300 && now.dayOfYear() < 310)
        imagePath = ":/Splashscreen/splashscreen_halloween.png";

    if(now.dayOfYear() > 330 && now.dayOfYear() < 365) {
        imagePath = ":/Splashscreen/splashscreen_christmas.png";
        messageColor = QColor(255, 255, 255);
    }

    return QPair<QColor, QString>(messageColor, imagePath);
}

void loadStartupPreferences(QApplication *a) {
    qInfo() << "Loading preferences...";

    // style
    qInfo() << "   Loading style handler...";
    StyleHandler::init();

    // language
    qInfo() << "   Loading language...";
    // TODO: Reimplement ui translations; Work with QLocale instead!
}

int main(int argc, char *argv[]) {
    QSettings set("ScheduleMaster", "ScheduleMaster");
    if(set.value("appearance.fontEngineGDI", true).toBool())
        qputenv("QT_QPA_PLATFORM", "windows:fontengine=gdi");

    qputenv("QT_SCALE_FACTOR", set.value("appearance.uiScale", 1.0).toString().toUtf8());

    QApplication a(argc, argv);
    a.setOverrideCursor(QCursor(Qt::WaitCursor));
    SM::ApplicationInterfaceImpl scheduleMasterApp(nullptr);

    QPair<QColor, QString> ssConfig = splashScreenConfig();
    QSplashScreen splashscreen(QPixmap(ssConfig.second));
    splashscreen.show();

    // init crash detector
    CrashDetector::init();

    // init logger
    Logger logger(&a);
    qInfo() << "Starting ScheduleMaster...";

    splashscreen.showMessage(QObject::tr("Loading settings and configuration..."), Qt::AlignBottom, ssConfig.first);
    SettingsManager::init();
    LanguageManager::init();
    IconSetManager::init();
    StyleManager::init();
    FolderLocationManager::init();
    ActionManager::init();
    DockManager::init();
    WorkspaceManager::init();

    IconController::init();
    ActionController::init();

    splashscreen.showMessage(QObject::tr("Loading preferences..."), Qt::AlignBottom, ssConfig.first);
    loadStartupPreferences(&a);

    splashscreen.showMessage(QObject::tr("Loading main window..."), Qt::AlignBottom, ssConfig.first);
    StyleHandler::applyStyle();
    StyleHandler::applyColorScheme();
    StyleHandler::applyAccentColor();
    MainWindow w;

#ifndef QT_DEBUG
    a.thread()->sleep(2);
#endif

    qInfo() << "Loading main window size and position...";
    bool ok = w.restoreGeometry(SettingsManager::value("general.mainWindowGeometry").toByteArray());
    if(!ok)
        w.showMaximized();
    else
        w.show();

    // now, apply font. This is necessary because otherwise some ui components (e.g. the menubar) don't take the font if it was set when the mainWindow isn't shown
    StyleHandler::applyFont();

    splashscreen.finish(&w);
    a.restoreOverrideCursor();

    int result = a.exec();
    SettingsManager::setValue("general.mainWindowGeometry", w.saveGeometry());
    qInfo() << "Closing ScheduleMaster...";
    return result;
}
