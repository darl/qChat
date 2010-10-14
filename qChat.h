#ifndef QCHAT_H
#define QCHAT_H

#include "qTypes.h"

class QUdpSocket;
class QString;
class QIcon;

extern QUdpSocket* globalSocket;

QIcon statusIcons(userStatus st);

void sendMessage(const QString& msg);
void sendWhoRequest();
void sendOnlinePing();
void sendOnlineWarning();
void sendOfflineWarning();


#endif // QCHAT_H
