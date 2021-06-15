#ifndef TASKSTATUS
#define TASKSTATUS

#include <QNetworkReply>

enum class InternalStatus
{
    IN_PROGRESS,
    DOWNLOADING,
    INTERNAL_ERROR,
    NETWORK_ERROR,
    TEXT_FOUND,
    TEXT_NOT_FOUND,
};

struct TaskStatus
{
    InternalStatus internalStatus;
    // responseStatus contains a description of HTTP error
    // If internalStatus != NETWORK_ERROR responseStatus will be qeual to '-'
    QString responseStatus;
};

#endif // TASKSTATUS

