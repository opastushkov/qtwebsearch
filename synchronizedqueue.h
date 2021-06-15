#ifndef SYNCHRONIZEDQUEUE_H
#define SYNCHRONIZEDQUEUE_H

#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

class SynchronizedQueue
{
private:
    QQueue<QString> queue;
    QMutex mtx;

public:
    SynchronizedQueue() { }

    void enqueue(const QString &el)
    {
        QMutexLocker locker(&mtx);
        queue.enqueue(el);
    }

    QString dequeue()
    {
        QMutexLocker locker(&mtx);

        return queue.dequeue();
    }

    int isEmpty()
    {
        QMutexLocker locker(&mtx);

        return queue.isEmpty();
    }

    bool contains(const QString &el)
    {
        QMutexLocker locker(&mtx);

        return queue.contains(el);
    }

};

#endif // SYNCHRONIZEDQUEUE_H
