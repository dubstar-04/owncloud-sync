#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>
#include <QNetworkConfigurationManager>
#include <QTimer>

//#include <QObject>
#include <unistd.h>

#include "owncloudsyncd.h"

OwncloudSyncd::OwncloudSyncd()
{
    //QCoreApplication::setApplicationName("owncloud-sync");
    m_settingsFile =  QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/owncloud-sync/owncloud-sync.conf";

    if( !QFile(m_settingsFile).exists()){
        qDebug() << "No Settings File - Quiting";
        //QCoreApplication::quit();
    }

    qDebug() << QString("Retrieve settings from ") + m_settingsFile;

    QSettings settings(m_settingsFile, QSettings::IniFormat);

    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();
    m_serverURL = settings.value("serverURL").toString();
    m_ssl = settings.value("ssl").toBool();
    m_mobileData = settings.value("mobileData").toBool();
    m_timer = settings.value("timer").toInt();

    qDebug() << "Username: " << m_username << " Server: " << m_serverURL;

    if (m_username.isEmpty() || m_password.isEmpty() || m_serverURL.isEmpty()){
        qWarning() << "Connection details missing  - Quiting";
        //QCoreApplication::quit();
    }else{

        getSyncFolders();
        //addPathsToWatchlist();

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(syncFolder(QString)));
        timer->start(m_timer);

    }

}

void OwncloudSyncd::addPathsToWatchlist(){

    m_watcher = new QFileSystemWatcher(this);

    QMapIterator<QString, QString> i(m_folderMap);
    while (i.hasNext()) {
        i.next();
        //qDebug() << i.key() << ": " << i.value() << endl;

        if(QDir(i.key()).exists()){
            m_watcher->addPath(i.key());
            //m_watcher->removePath(i.key() + "/.csync_journal.db");
            qDebug() << "Directory: " << i.key() << " Added to watchlist";
        }else{
            qDebug() << "Directory: " << i.key() << " Doesn't exist";
        }


    }

    int dirs = m_watcher->directories().length();

    if(!dirs){
        qDebug() << " No Directories Configured - Quitting";
        return;
        //QCoreApplication::quit();
    }

    qDebug() << QString::number(dirs) << " Directories added to watchlist";
    connect(m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncFolder(QString)));

}

void OwncloudSyncd::loadDB(const QString& path){

    qDebug() << "Attempting to access DB: " << path;

    //QSqlDatabase db = QSqlDatabase::database();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }

}

void OwncloudSyncd::getSyncFolders()
{
    //Path should be: //home/phablet/.local/share
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    path += "/owncloud-sync/Databases";
    qDebug() << "Writeable Path: " << path;

    //Find the Database Name
    QStringList nameFilter("*.sqlite");
    QDir directory(path);
    QStringList dbName = directory.entryList(nameFilter);

    qDebug() << "DB Name: " << dbName.at(0);

    path += "/" + dbName.at(0);

    qDebug() << "Attempting to access DB: " << path;

    //QSqlDatabase db = QSqlDatabase::database();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    //db.open();

    if (!db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
        qDebug() << "Database Tables:" << db.tables(QSql::AllTables).at(0);

        QSqlQuery query(db);

        query.exec("SELECT local, remote FROM SyncFolders");

        while (query.next()) {
            //qDebug() << query.value(0).toString();
            //qDebug() << query.value(1).toString();
            m_folderMap.insert(query.value(0).toString(), query.value(1).toString());

        }
    }

    db.close();
}

void OwncloudSyncd::syncFolder(const QString& localPath){

    qDebug() << "\n"<< endl;

    /*
    QStringList files = watcher->files();

    qDebug() << files.size() << "Files To Check";

    bool filesToSync = false;

    for(int i = 0; i < files.size(); i++){
        qDebug() << "Sync File: " << files.at(i);
        QFileInfo fileInfo(files.at(i));
        if(!fileInfo.isHidden() || fileInfo.isDir()){
            filesToSync = true;
            break;
        }
    }

    if(!filesToSync){
        qDebug() << "Only Hidden Files - Quitting";
        return;
    }
    */

    m_watcher->blockSignals(true);

    if (QFile(localPath + "/.csync_journal.db-shm").exists() ||
          QFile(localPath + "/.csync_journal.db-wal").exists()  ){

        qDebug() << "Delete Stale Database File";

        QFile::remove(localPath + "/.csync_journal.db-shm");
        QFile::remove(localPath + "/.csync_journal.db-wal");

    }

    //Create a connection manager, establish is a data connection is avaiable
    QNetworkConfigurationManager mgr;
    qDebug() << "Network Connection Type: " << mgr.defaultConfiguration().bearerTypeName();
    qDebug() << "Mobile Data Sync: " << m_mobileData;

    QList<QNetworkConfiguration> activeConfigs = mgr.allConfigurations(QNetworkConfiguration::Active);
    if (!activeConfigs.count()){
        qWarning() << "No Data Connection Available  - Quiting";
        return;
    } else {

        QNetworkConfiguration::BearerType connType = mgr.defaultConfiguration().bearerType();
        if(!m_mobileData){
            if(connType != QNetworkConfiguration::BearerEthernet && connType != QNetworkConfiguration::BearerWLAN){
                qDebug() << "No Sync on Mobile Data - Check User Settings - Quitting";
                return;
            }
        }

        //Either mobile data sync is allowed or Ethernet or Wifi is available
    }

    QString protocol;

    if(m_ssl){
        protocol = "https://";
    }else{
        protocol = "https://";
    }

    QString remotePath = protocol + m_serverURL + QStringLiteral("/remote.php/webdav") + m_folderMap.value(localPath);
    qDebug() << "Starting Owncloud Sync from " << localPath << " to " << remotePath;

    /* Needs more work
    QUrl url(remotePath);

    if (!url.isValid()) {
        qDebug() << QString("Remote Path Seems Invalid: %1").arg(url.toString());
    }else{
        qDebug() << "Remote Path Seems Valid";
    }
    */

    QString owncloudcmd;

    if( QFile("/opt/click.ubuntu.com/owncloud-sync/current/Owncloud-Sync/lib/arm-linux-gnueabihf/bin/owncloudcmd").exists()){
        owncloudcmd = "/opt/click.ubuntu.com/owncloud-sync/current/Owncloud-Sync/lib/arm-linux-gnueabihf/bin/owncloudcmd";
        qDebug() << "Using Arm owncloudcmd Binary - Mobile?";
    }else{
        owncloudcmd = "owncloudcmd";
        qDebug() << "Using Local owncloudcmd Binary - Desktop?";
    }


    QStringList arguments;
    arguments << "--user" << m_username << "--password" << m_password << "--silent" << "--non-interactive" << localPath << remotePath;



    QProcess *owncloudsync = new QProcess();
    //Retrieve all debug from process
    owncloudsync->setProcessChannelMode(QProcess::ForwardedChannels);
    owncloudsync->start(owncloudcmd, arguments);
    //Wait for the sync to complete. Dont time out.
    owncloudsync->waitForFinished(-1);

    //sleep(10);
    QDateTime maxWait = QDateTime::currentDateTime();
    maxWait.addSecs(30);

    while (QFile(localPath + "/.csync_journal.db-shm").exists()){

        qDebug() << "Waiting For Sync To Complete: " << QDateTime::currentDateTime();

        if(QDateTime::currentDateTime() > maxWait){
            qDebug() << "maxWait Reached - Quitting Loop";
            break;

        }

    }


    //sleep(10);
    m_watcher->blockSignals(false);
    //Sync Complete - Save the current date and time
    qDebug() << localPath << " - Sync Completed: " << QDateTime::currentDateTime();
    //QSettings settings(m_settingsFile);
    //settings.setValue("lastSync", QDateTime::currentDateTime());

}
