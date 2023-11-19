#include <QApplication>
#include <QTranslator>

#include "Mainwindow.h"

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);

    QTranslator translator;
    qDebug() << translator.load(":/main/Translations/german.qm");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
