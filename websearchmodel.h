#ifndef WEBSEARCHMODEL_H
#define WEBSEARCHMODEL_H

#include <QString>
#include <QThread>
#include <QMutex>

#include <httpregex.h>

class QThreadPool;
template<typename T>
class QAtomicInteger;

class SynchronizedQueue;
class SynchronizedHash;
class WebSearchView;
class TaskStatus;

class WebSearchModel: public QObject
{
    Q_OBJECT
private:
    WebSearchView *parent;
    SynchronizedQueue *todoQueue;
    SynchronizedHash *statusHash;
    uint64_t taskCounter;
    QThreadPool *threadPool;
    QString startUrl;
    QString text;
    uint64_t urlsMaxCount;
    uint64_t threadsMaxCount;
    bool isStopped = false;

public:
    WebSearchModel(WebSearchView *parent, const QString &startUrl,
                   const QString &text, const uint64_t urlsMaxCount,
                   const uint64_t threadsMaxCount);

    virtual ~WebSearchModel();
    bool isWorkFinished() const;
    QHash<QString, TaskStatus> getStatuses() const;

public slots:
    void startSearch();
    void stopWork();

signals:
    void finished();
    void updateView();
};

#endif // WEBSEARCHMODEL_H
