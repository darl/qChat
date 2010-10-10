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
    mtSystemMessage
};

enum userStatus
{
    usOnline,
    usAway,
    usBusy,
    usOffline
};

struct qUser
{
    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;
};

#endif // QTYPES_H
