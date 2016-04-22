#ifndef OWNCLOUDSYNCD_H
#define OWNCLOUDSYNCD_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QMap>
#include <QTimer>


class OwncloudSyncd : public QObject
{
Q_OBJECT

public:
    OwncloudSyncd();

//    ~OwncloudSyncd();

private slots:
void syncDir(const QString& str);
void syncDirs();

void loadDB(const QString& path);
void getSyncFolders();
void addPathsToWatchlist();

private:

    //QFileSystemWatcher watcher;
    QFileSystemWatcher * m_watcher;
    QTimer * m_timer;

    QString m_settingsFile;
    QString m_username;
    QString m_password;
    QString m_serverURL;
    bool m_ssl;
    bool m_mobileData;
    int m_syncInterval;

    QMap<QString, QString> m_folderMap;

    //QDateTime m_lastSync;
};

#endif // OWNCLOUDSYNCD_H
