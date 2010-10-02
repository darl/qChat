#ifndef QCHAT_H
#define QCHAT_H

#include "qTypes.h"
#include "qConfig.h"


extern QUdpSocket* globalSocket;

extern QString statusIcons[];

extern void sendMessage(QString msg);
extern void sendWhoRequest();
extern void sendOnlinePing();
extern void sendOnlineWarning();
extern void sendOfflineWarning();


#endif // QCHAT_H
