#include <QApplication>
#include <QTranslator>

#include <QStyleFactory>
#include <QStyleHints>

#include "localconfig.h"
#include "App/logger.h"

#include "Mainwindow.h"

#include "AppInfo.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    LocalConfig localConfig(&a);
    AppInfo appInfo(&a);

    QDir logDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";

    if(LocalConfig::crashDetected()) {
        const QDateTime dt(QDateTime::currentDateTime());
        QString newFileName = logDir.path() + "/logfile_crash_" + dt.toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
        if(QFile::copy(logDir.path() + "/logfile.txt", newFileName))
            LocalConfig::setLastLogfileName(newFileName);
    }

    Logger logger(&a, logDir);

    qInfo() << "loading preferences...";
    qInfo() << "   loading stylesheet...";
    if(LocalConfig::style() == LocalConfig::Fusion)
        a.setStyle(QStyleFactory::create("Fusion"));
    else if(LocalConfig::style() == LocalConfig::WindowsXpStyle) {
        a.setStyle(QStyleFactory::create("Windows"));
        a.setStyleSheet("* {font-family:Tahoma;}");
    } else
        a.setStyle(QStyleFactory::create("windowsvista"));
    
    qInfo() << "   loading language...";
    if(LocalConfig::language() == LocalConfig::German) {
        qInfo() << "loading german translation...";
        QTranslator *translator = new QTranslator;
        bool ok;
        ok = translator->load(":/Translations/german.qm");
        if(ok)
            a.installTranslator(translator);
        else
            qWarning() << "failed loading german translation!";

        QTranslator *uiTranslator = new QTranslator;
        ok = uiTranslator->load(":/Translations/qtbase_de.qm");
        if(ok)
            a.installTranslator(uiTranslator);
        else
            qWarning() << "failed loading german ui translation!";
    }

    MainWindow w;
    w.show();

    int result = a.exec();
    LocalConfig::setCrashDetected(false);
    qInfo() << "closing ScheduleMaster...";
    return result;
}
