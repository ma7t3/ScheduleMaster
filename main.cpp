#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTranslator>
#include <QSplashScreen>
#include <QThread>

#include "Global/Logger.h"

QPair<QColor, QString> splashScreenConfig() {
    QString imagePath = ":/Splashscreen/slpashscreen_light.png";
    QColor messageColor = QColor(0, 0, 0);
    if(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark && LocalConfig::style() != LocalConfig::SystemStyle) {
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
    qInfo() << "   Loading ui style...";
    switch(LocalConfig::style()) {
        case LocalConfig::FusionStyle:    a->setStyle(QStyleFactory::create("Fusion"));       break;

        case LocalConfig::WindowsXpStyle: a->setStyle(QStyleFactory::create("Windows"));
                                          a->setStyleSheet("* {font-family:Tahoma;}");        break;

        default:                          a->setStyle(QStyleFactory::create("windowsvista")); break;
    }

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
    QDir logDir = LocalConfig::folderLocationPaths("base.logfile").first();

    // check if crash was detected. In this case copy logfile
    if(LocalConfig::crashDetected()) {
        const QDateTime now(QDateTime::currentDateTime());
        QString newFileName = logDir.path() + "/logfile_crash_" + now.toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
        if(QFile::copy(logDir.path() + "/logfile.txt", newFileName))
            LocalConfig::setLastLogfileName(newFileName);
    }

    LocalConfig::setCrashDetected(true);

    // init logger
    Logger logger(&a, logDir);
    qInfo() << "Starting ScheduleMaster...";

    splashscreen.showMessage("Loading localization...", Qt::AlignBottom, ssConfig.first);
    qInfo() << "Loading localization...";
    GlobalConfig::initLanguages();
    LocalConfig::initLocale();

    splashscreen.showMessage(QObject::tr("Loading global configuration..."), Qt::AlignBottom, ssConfig.first);
    GlobalConfig::init();

    splashscreen.showMessage(QObject::tr("Loading local configuration..."), Qt::AlignBottom, ssConfig.first);
    LocalConfig::init();

    splashscreen.showMessage(QObject::tr("Loading preferences..."), Qt::AlignBottom, ssConfig.first);
    loadStartupPreferences(&a);

    splashscreen.showMessage(QObject::tr("Loading main window..."), Qt::AlignBottom, ssConfig.first);
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

    splashscreen.finish(&w);
    a.restoreOverrideCursor();

    int result = a.exec();
    LocalConfig::setMainWindowGeomentry(w.saveGeometry());

    LocalConfig::setCrashDetected(false);
    qInfo() << "Closing ScheduleMaster...";
    return result;
}
