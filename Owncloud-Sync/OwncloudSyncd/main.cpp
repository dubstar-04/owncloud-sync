#include <QCoreApplication>
#include <QDebug>

#include "unistd.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    while(true){
        qDebug("Running...");
        sleep(3);
    }


    return a.exec();
}

