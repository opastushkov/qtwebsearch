#include "websearchmodel.h"

#include <QAtomicInteger>
#include <QThreadPool>
#include <QEventLoop>

#include <synchronizedqueue.h>
#include <synchronizedhash.h>
#include <websearchview.h>
#include <worker.h>

#define DELAY (100)

WebSearchModel::WebSearchModel(WebSearchView *_parent, const QString &_startUrl,
                               const QString &_text, const uint64_t _urlsMaxCount,
                               const uint64_t _threadsMaxCount):
    parent(_parent),
    todoQueue(new SynchronizedQueue()),
    statusHash(new SynchronizedHash()),
    taskCounter(0),
    threadPool(new QThreadPool),
    startUrl(_startUrl),
    text(_text),
    urlsMaxCount(_urlsMaxCount),
    threadsMaxCount(_threadsMaxCount)
{
    connect(parent, SIGNAL(stopExecution()), this, SLOT(stopWork()), Qt::DirectConnection);
}

WebSearchModel::~WebSearchModel()
{
    delete todoQueue;
    delete statusHash;
    delete threadPool;
}

bool WebSearchModel::isWorkFinished() const
{
    // A work will be stopped if:
    // 1. A user presses STOP button
    // 2. The programm handles enough URLs
    // 3. The queue doesn't contain URLs and they won't be added from threads
    return isStopped || taskCounter >= urlsMaxCount ||
           (todoQueue->isEmpty() && threadPool->activeThreadCount() == 0);
}

QHash<QString, TaskStatus> WebSearchModel::getStatuses() const
{
    return statusHash->getStatuses();
}

void WebSearchModel::stopWork()
{
    isStopped = true;
    threadPool->clear();
}

void WebSearchModel::startSearch()
{
    threadPool->setMaxThreadCount(threadsMaxCount);
    todoQueue->enqueue(startUrl);
    do
    {
        if (todoQueue->isEmpty())
        {
            QThread::msleep(DELAY);
            continue;
        }

        auto url = todoQueue->dequeue();
        statusHash->insert(url, TaskStatus{InternalStatus::IN_PROGRESS, "-"});
        ++taskCounter;

        Worker *newTask = new Worker(url, text, taskCounter, todoQueue, statusHash);
        threadPool->start(newTask);
    }
    while(!isWorkFinished());

    threadPool->waitForDone();

    emit updateView();

    // If this task finishes execution we need to update QTableView in the last time.
    // We will wait for updateFinished signal in order to avoid access
    // to unavailable data from the View class.
    QEventLoop loop;
    connect(parent, SIGNAL(updateFinished()), &loop, SLOT(quit()));
    loop.exec();

    emit finished();
}
