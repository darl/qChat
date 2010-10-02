#ifndef QTYPES_H
#define QTYPES_H

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

#endif // QTYPES_H
