#ifndef QTYPES_H
#define QTYPES_H

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

    usCounterStrike=6,
    usDota=7,
    usQuake3
};

#endif // QTYPES_H
