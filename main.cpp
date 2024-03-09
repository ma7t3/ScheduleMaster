#include <QApplication>
#include <QTranslator>

#include "preferences.h"
#include "App/logger.h"

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);

    Logger logger(&a);

    qInfo() << "loading preferences...";

    if(Preferences::instance().language() == Preferences::LanguageGerman) {
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
