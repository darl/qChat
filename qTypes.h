#ifndef QTYPES_H
#define QTYPES_H

enum messageType{
    mtMessage,
    mtOnlinePing,
    mtWhoRequest,
    mtOnlineWarning,
    mtOfflineWarning,
    mtSystemMessage,
    mtBot,

    mtUpdateInfo,
    mtUpdateRequest,

    mtConferenceInfo,
    mtConferenceInfoRequest,

    mtPublicKey,
    mtPublicKeyRequest
};

enum userStatus
{
    usOnline,
    usAway,
    usBusy,
    usOffline,

    usCounterStrike=6,
    usDota,
    usQuake3,

    usQuiz
};

#endif // QTYPES_H
