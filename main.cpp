#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("OpenInnovations 2.0");
    a.setWindowIcon(QIcon("icon.png"));
    w.setStyleSheet ("background-image: url(:/flag-rossiya-fon-lenta.jpg);");
    w.show();

    return a.exec();
}
