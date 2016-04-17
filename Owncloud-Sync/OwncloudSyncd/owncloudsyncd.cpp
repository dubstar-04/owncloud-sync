#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
//#include <QObject>

#include "owncloudsyncd.h"

OwncloudSyncd::OwncloudSyncd()
{

    //QCoreApplication::setApplicationName("owncloud-sync");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    m_settingsFile =  QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/owncloud-sync/owncloud-sync.conf";

    qDebug() << QString("Retrieve setting from ") + m_settingsFile;

    QSettings settings(m_settingsFile);

    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();
    m_serverURL = settings.value("serverURL").toString();
    m_ssl = settings.value("ssl").toBool();
    m_timer = settings.value("timer").toInt();

    //Check if the settings are complete build a filesystemwatcher

    //QFileSystemWatcher watcher;

    watcher = new QFileSystemWatcher(this);

    watcher->addPath("/home/sandal/TestSyncFolder");

    QStringList directoryList = watcher->directories();

    Q_FOREACH(QString directory, directoryList)
            qDebug() << "Watching: " << directory <<"\n";

   connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncFolder(QString)));

}

/*OwncloudSyncd::~OwncloudSyncd()
{

}
*/

void OwncloudSyncd::syncFolder(const QString& str){

        qDebug() << QString("Starting Owncloud Sync to ") + m_serverURL + QString(" for user ") + m_username;
        qDebug() << QString("Syncing Folder: ") + str;

        // temp command for testing on desktop
        //owncloudcmd [OPTION] <source_dir> <server_url>
        // <source_dir> = str
        //QString server_directory = m_serverURL;  //match the local dir with the server dir


        //QString owncloudcmd = "owncloudcmd";
        //QString owncloudcmd = "/opt/click.ubuntu.com/Owncloud-Sync/current/lib/arm-linux-gnueabihf/bin/owncloudcmd";

        //QStringList arguments;
        //arguments << "-name" <<  m_username << "-password" << m_password  m_serverURL;



        //QProcess *owncloudsync = new QProcess();
        //owncloudsync->start(owncloudcmd, arguments);


        //Sync Complete - Save the current date and time
        qDebug() << str << " - Sync Completed: " << QDateTime::currentDateTime();
        QSettings settings(m_settingsFile);
        settings.setValue("lastSync", QDateTime::currentDateTime());

}
