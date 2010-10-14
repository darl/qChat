#ifndef QCHAT_H
#define QCHAT_H

class QUdpSocket;
class QString;

extern QUdpSocket* globalSocket;

extern QString statusIcons[];

extern void sendMessage(const QString& msg);
extern void sendWhoRequest();
extern void sendOnlinePing();
extern void sendOnlineWarning();
extern void sendOfflineWarning();


#endif // QCHAT_H
