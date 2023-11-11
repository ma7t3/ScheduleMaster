#include <QApplication>

#include "Mainwindow.h"

// this is a comment

int main(int argc, char *argv[])
{
    //qDebug() << "Opened File: " << argv[1];
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
