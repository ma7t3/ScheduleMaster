#include <QApplication>
#include <QTranslator>

#include "preferences.h"

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);

    if(Preferences::instance().language() == Preferences::LanguageGerman) {
        QTranslator *translator = new QTranslator;
        bool ok;
        ok = translator->load(":/Translations/german.qm");
        if(ok)
            a.installTranslator(translator);

        QTranslator *uiTranslator = new QTranslator;
        ok = uiTranslator->load(":/Translations/qtbase_de.qm");
        if(ok)
            a.installTranslator(uiTranslator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
