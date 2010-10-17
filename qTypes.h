#ifndef QTYPES_H
#define QTYPES_H

#include <QDateTime>
#include <QHostAddress>

enum messageType{
    mtMessage,
    mtOnlinePing,
    mtWhoRequest,
    mtOnlineWarning,
    mtOfflineWarning,
    mtSystemMessage,
    mtBot
};

enum userStatus
{
    usOnline,
    usAway,
    usBusy,
    usOffline,

    usCounterStrike = 10,
    usDota,
    usQuake3
};

struct qUser
{
    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;
};

#endif // QTYPES_H
