#include "mainwindow.h"

#include <QApplication>
#include <stdbool.h>


extern "C" {
    #include "passenger_manager.h"
    #include "queue_manager.h"
    #include "driver_manager.h"
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
