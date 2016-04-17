#include <QCoreApplication>

#include "owncloudsyncd.h"

//#include "unistd.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    OwncloudSyncd sync;

    return a.exec();
}


