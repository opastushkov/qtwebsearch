#ifndef HTTPGET_H
#define HTTPGET_H

#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <QRegularExpression>

class QNetworkReply;

template<typename T>
class QAtomicInteger;

template<typename T>
class QAtomicPointer;

class SynchronizedQueue;
class SynchronizedHash;

class Worker : public QRunnable
{
public:
    Worker(const QString &url, const QString &text,
           uint64_t fileCounter,
           SynchronizedQueue *queue,
           SynchronizedHash *hash);

private:
    QString url;
    QString text;
    QString response;
    QRegularExpression httpRegex;
    SynchronizedQueue *todoQueue;
    SynchronizedHash *statusHash;
    uint64_t fileCounter;

public:
    void run() override;
    void tryDownload();
    void savePage();
    void findAllUrls();
    bool isErrorOccured(const QNetworkReply *reply);
    void findMatches() const;
};

#endif // HTTPGET_H
