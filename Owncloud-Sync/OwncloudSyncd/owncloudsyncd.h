#ifndef OWNCLOUDSYNCD_H
#define OWNCLOUDSYNCD_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QSettings>


class OwncloudSyncd : public QObject
{
Q_OBJECT

public:
    OwncloudSyncd();

//    ~OwncloudSyncd();

private slots:
void syncFolder(const QString& str);

private:

    //QFileSystemWatcher watcher;
    QFileSystemWatcher * watcher;

    QString m_settingsFile;
    QString m_username;
    QString m_password;
    QString m_serverURL;
    bool m_ssl;
    int m_timer;
    //QDateTime m_lastSync;
};

#endif // OWNCLOUDSYNCD_H
