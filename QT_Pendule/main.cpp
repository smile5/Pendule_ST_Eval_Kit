#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //w.setWindowIcon(QIcon(":"));
    w.setWindowTitle("Pendule à l'Heure !!!");
    return a.exec();
}
