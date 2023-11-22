#include <QApplication>
#include <QTranslator>

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(":/main/Translations/german.qm");
    a.installTranslator(&translator);

    QTranslator uiTranslator;
    uiTranslator.load(":/main/Translations/qtbase_de.qm");
    a.installTranslator(&uiTranslator);

    MainWindow w;
    w.show();
    return a.exec();
}
