#ifndef QCHAT_H
#define QCHAT_H

#include <QtNetwork>
#include <QString>
#include <QObject>
#include <QStandardItemModel>
#include <QUdpSocket>
#include <QByteArray>

class qUser;


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

extern quint16 port;
extern QHostAddress broadcast;

extern QString nick;
extern userStatus status;

extern QUdpSocket* globalSocket;

//extern QStandardItemModel onlineUserModel;

extern QString statusIcons[];

extern bool invisibleMode;
extern bool adminMode;
extern bool showOfflineUsers;

extern void sendMessage(QString msg);
extern void sendWhoRequest();
extern void sendOnlinePing();
extern void sendOnlineWarning();
extern void sendOfflineWarning();


#endif // QCHAT_H
