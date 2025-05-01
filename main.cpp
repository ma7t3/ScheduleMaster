#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTranslator>
#include <QSplashScreen>
#include <QThread>

#include "Global/Logger.h"
#include "Global/ActionShortcutMapper.h"
#include "Global/StyleHandler.h"
#include "Global/LanguagesHandler.h"

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
    QSettings set("", "appearance.fontEngineGDI");
    if(set.value("useGdiEngine", true).toBool())
        qputenv("QT_QPA_PLATFORM", "windows:fontengine=gdi");

    QApplication a(argc, argv);

    a.setOverrideCursor(QCursor(Qt::WaitCursor));

    QPair<QColor, QString> ssConfig = splashScreenConfig();
    QSplashScreen splashscreen(QPixmap(ssConfig.second));
    splashscreen.show();

    // get logfile dir
    QDir logDir = LocalConfig::folderLocationPaths("logfile").first();

    // check if crash was detected. In this case copy logfile
    if(LocalConfig::crashDetected()) {
        const QDateTime now(QDateTime::currentDateTime());
        QString newFileName = logDir.path() + "/logfile_crash_" + now.toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
        if(QFile::copy(logDir.path() + "/logfile.txt", newFileName))
            LocalConfig::setLastLogfileName(newFileName);
    }

    // init logger
    Logger logger(&a, logDir);
    qInfo() << "Starting ScheduleMaster...";

    splashscreen.showMessage("Loading localization...", Qt::AlignBottom, ssConfig.first);
    qInfo() << "Loading localization...";
    LanguagesHandler::init();
    LocalConfig::initLocale();

    splashscreen.showMessage(QObject::tr("Loading global configuration..."), Qt::AlignBottom, ssConfig.first);
    GlobalConfig::init();

    splashscreen.showMessage(QObject::tr("Loading local configuration..."), Qt::AlignBottom, ssConfig.first);
    LocalConfig::init();

    splashscreen.showMessage(QObject::tr("Init shortcut mapper..."), Qt::AlignBottom, ssConfig.first);
    ActionShortcutMapper::init();

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
    bool ok = w.restoreGeometry(LocalConfig::mainWindowGeometry());
    if(!ok)
        w.showMaximized();
    else
        w.show();

    // now, apply font. This is necessary because otherwise some ui components (e.g. the menubar) don't take the font if it was set when the mainWindow isn't shown
    StyleHandler::applyFont();

    splashscreen.finish(&w);
    a.restoreOverrideCursor();

    int result = a.exec();
    LocalConfig::setMainWindowGeomentry(w.saveGeometry());

    LocalConfig::setCrashDetected(false);
    qInfo() << "Closing ScheduleMaster...";
    return result;
}
