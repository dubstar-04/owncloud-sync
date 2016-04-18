#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
//#include <QObject>

#include "owncloudsyncd.h"

OwncloudSyncd::OwncloudSyncd()
{
    //QCoreApplication::setApplicationName("owncloud-sync");
    m_settingsFile =  QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/owncloud-sync/owncloud-sync.conf";

    if( !QFile(m_settingsFile).exists()){
        qDebug() << "No Settings File - Quiting";
        QCoreApplication::quit();
    }

    qDebug() << QString("Retrieve settings from ") + m_settingsFile;

    QSettings settings(m_settingsFile, QSettings::IniFormat);

    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();
    m_serverURL = settings.value("serverURL").toString();
    m_ssl = settings.value("ssl").toBool();
    m_timer = settings.value("timer").toInt();

    qDebug() << "Username: " << m_username << " Server: " << m_serverURL;

    if (m_username.isEmpty() || m_password.isEmpty() || m_serverURL.isEmpty()){
        qWarning() << "Connection details missing  - Quiting";
        QCoreApplication::quit();
    }else{

    getSyncFolders();
    addPathsToWatchlist();

    }

}

void OwncloudSyncd::addPathsToWatchlist(){

    watcher = new QFileSystemWatcher(this);

    QMapIterator<QString, QString> i(m_folderMap);
    while (i.hasNext()) {
        i.next();
        //qDebug() << i.key() << ": " << i.value() << endl;

        if(QDir(i.key()).exists()){
            watcher->addPath(i.key());
            qDebug() << "Directory: " << i.key() << " Added to watchlist";
        }else{
            qDebug() << "Directory: " << i.key() << " Doesn't exist";
        }


    }

    int dirs = watcher->directories().length();

    qDebug() << QString::number(dirs) << " Directories added to watchlist";

    if(!dirs){
        QCoreApplication::quit();
    }

    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(syncFolder(QString)));

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
    //QSettings settings(m_settingsFile);
    //settings.setValue("lastSync", QDateTime::currentDateTime());

}
