#ifndef OWNCLOUDSYNCD_H
#define OWNCLOUDSYNCD_H

#include <QObject>
#include <QFileSystemWatcher>

class OwncloudSyncd
{
//Q_OBJECT
public:
    OwncloudSyncd();

private:
    void syncFolder(const QString& str);

    QFileSystemWatcher watcher;

    QString m_username;
    QString m_password;
    QString m_serverURL;
    bool m_ssl;
    int m_timer;
};

#endif // OWNCLOUDSYNCD_H
