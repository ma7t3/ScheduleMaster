#include <QApplication>
#include <QDir>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTranslator>
#include <QSplashScreen>
#include <QThread>
#include <QSettings>

#include "namespace.h"
#include "core/ApplicationInterfaceImpl.h"
#include "core/CrashDetectorImpl.h"
#include "core/SettingsServiceImpl.h"
#include "core/AppearanceServiceImpl.h"
#include "core/ActionServiceImpl.h"
#include "ui/MainWindow.h"

QPair<QColor, QString> splashScreenConfig() {
    QString imagePath = ":/splashscreen/slpashscreen_light.png";
    QColor messageColor = QColor(0, 0, 0);
    if(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        imagePath = ":/splashscreen/splashscreen_dark.png";
        messageColor = QColor(255, 255, 255);
    }

    QDate now = QDate::currentDate();

    if(now.dayOfYear() > 300 && now.dayOfYear() < 310)
        imagePath = ":/splashscreen/splashscreen_halloween.png";

    if(now.dayOfYear() > 330 && now.dayOfYear() < 365) {
        imagePath = ":/splashscreen/splashscreen_christmas.png";
        messageColor = QColor(255, 255, 255);
    }

    return QPair<QColor, QString>(messageColor, imagePath);
}

int main(int argc, char *argv[]) {
    QSettings set("ScheduleMaster", "ScheduleMaster");

#ifdef Q_OS_WIN
    if(set.value("appearance.fontEngineGDI", true).toBool()) {
        qputenv("QT_QPA_PLATFORM", "windows:fontengine=gdi");
    }
#endif

    qputenv("QT_SCALE_FACTOR", set.value("appearance.uiScale", 1.0).toString().toUtf8());

    QApplication a(argc, argv);
    a.setOverrideCursor(QCursor(Qt::WaitCursor));
    SM::ApplicationInterfaceImpl appInterface(nullptr);

#ifndef QT_DEBUG
    a.thread()->sleep(2);
#endif

    SM::AppearanceServiceImpl::instance()->applyAppearance();
    a.restoreOverrideCursor();

    int result = a.exec();
    qInfo() << "Closing ScheduleMaster...";
    return result;
}
