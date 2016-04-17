#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#include "owncloudsyncd.h"

OwncloudSyncd::OwncloudSyncd()
{

    //QCoreApplication::setApplicationName("owncloud-sync");

    QString settingsFile =  QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/owncloud-sync/owncloud-sync.conf";

    qDebug() << QString("Retrieve setting from ") + settingsFile;

    QSettings settings(settingsFile, QSettings::IniFormat);

    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();
    m_serverURL = settings.value("serverURL").toString();
    m_ssl = settings.value("ssl").toBool();
    m_timer = settings.value("timer").toInt();

    //Check if the settings are complete build a filesystemwatcher
    watcher.addPath("/home/sandal/TestSyncFolder");

    QStringList directoryList = watcher.directories();

    Q_FOREACH(QString directory, directoryList)
            qDebug() << "Watching: " << directory <<"\n";

    //QObject::connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncFolder(QString)));

}

void OwncloudSyncd::syncFolder(const QString& str){

        qDebug() << QString("Starting Owncloud Sync to ") + m_serverURL + QString(" for user ") + m_username;

        // temp command for testing on desktop
        QString owncloudcmd = "owncloudcmd";
        //QString owncloudcmd = "/opt/click.ubuntu.com/Owncloud-Sync/current/lib/arm-linux-gnueabihf/bin/owncloudcmd";

        QStringList arguments;
        arguments << "-name" << "-password";

        QProcess *owncloudsync = new QProcess();
        owncloudsync->start(owncloudcmd, arguments);

}
