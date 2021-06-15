#include "worker.h"

#include <QNetworkAccessManager>
#include <QAtomicInteger>
#include <QNetworkReply>
#include <QEventLoop>
#include <QThread>
#include <QFile>
#include <QUrl>

#include <httpregex.h>
#include <synchronizedqueue.h>
#include <synchronizedhash.h>

Worker::Worker(const QString &_url, const QString &_text, uint64_t _counter,
               SynchronizedQueue *_queue, SynchronizedHash *_hash) :
    QRunnable(),
    url(_url),
    text(_text),
    httpRegex(URL_REGEX),
    todoQueue(_queue),
    statusHash(_hash),
    fileCounter(_counter)
{
}

void Worker::run()
{
    tryDownload();
    InternalStatus status = statusHash->getInternalStatus(url);
    if (status != InternalStatus::INTERNAL_ERROR && status != InternalStatus::NETWORK_ERROR)
    {
        findAllUrls();
        findMatches();
    }
}

void Worker::findMatches() const
{
    response.contains(text, Qt::CaseInsensitive)
            ? statusHash->setStatus(url, InternalStatus::TEXT_FOUND)
            : statusHash->setStatus(url, InternalStatus::TEXT_NOT_FOUND);
}

void Worker::tryDownload()
{
    statusHash->setStatus(url, InternalStatus::DOWNLOADING);
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (isErrorOccured(reply))
        statusHash->setStatus(url, InternalStatus::NETWORK_ERROR, reply->errorString());
    else
    {
        response = reply->readAll();
        savePage();
    }

    reply->deleteLater();
}

void Worker::savePage()
{
    QFile file("page_" + QString::number(fileCounter) + ".html");
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream st(&file);
        st << response << endl;
    }
}

bool Worker::isErrorOccured(const QNetworkReply *reply)
{
    return reply->error() != QNetworkReply::NoError;
}

void Worker::findAllUrls()
{
    auto iterator = httpRegex.globalMatch(response);
    while (iterator.hasNext())
    {
        auto match = iterator.next();
        auto newUrl = match.captured(0);
        if (!todoQueue->contains(newUrl) && !statusHash->contains(newUrl))
            todoQueue->enqueue(newUrl);
    }
}
