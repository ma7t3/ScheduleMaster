#include <QApplication>
#include <QTranslator>

#include "localconfig.h"
#include "App/logger.h"

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);

    Logger logger(&a);
    LocalConfig pref(&a);

    qInfo() << "loading preferences...";
    
    if(LocalConfig::language() == LocalConfig::LanguageGerman) {
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
    qInfo() << "closing ScheduleMaster...";
    return result;
}
