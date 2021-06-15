#ifndef SYNCHRONIZEDMAP_H
#define SYNCHRONIZEDMAP_H

#include <QHash>
#include <QMutex>
#include <QMutexLocker>

#include <taskstatus.h>

class QNetworkReply;

class SynchronizedHash
{
private:
    QHash<QString, TaskStatus> hash;
    QMutex mtx;

public:
    SynchronizedHash() { }

    void insert(const QString &url, const TaskStatus &el)
    {
        QMutexLocker locker(&mtx);
        hash.insert(url, el);
    }

    void setStatus(const QString &url, const InternalStatus &internalStatus,
                   const QString &responseStatus = "-")
    {
        QMutexLocker locker(&mtx);
        if (!hash.contains(url))
        {
            qWarning() << "Status table doesn't contain the key: " << url;
            return;
        }

        hash[url].internalStatus = internalStatus;
        hash[url].responseStatus = responseStatus;
    }

    QHash<QString, TaskStatus> getStatuses()
    {
        QMutexLocker locker(&mtx);

        return hash;
    }

    bool contains(const QString &url)
    {
        QMutexLocker locker(&mtx);

        return hash.contains(url);
    }

    InternalStatus getInternalStatus(const QString &url)
    {
        QMutexLocker locker(&mtx);
        if (!hash.contains(url))
        {
            qWarning() << "Status table doesn't contain the key: " << url;
            return InternalStatus::INTERNAL_ERROR;
        }

        return hash[url].internalStatus;
    }
};

#endif // SYNCHRONIZEDMAP_H
