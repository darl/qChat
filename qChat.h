#ifndef QCHAT_H
#define QCHAT_H

#include <QtNetwork>
#include <QString>
#include <QObject>
#include <QStandardItemModel>
#include <QUdpSocket>
#include <QByteArray>

#include "qTypes.h"



extern QHostAddress broadcast;
extern quint16 port;

extern QString nick;
extern userStatus status;

extern QUdpSocket* globalSocket;

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
