#ifndef QCHAT_H
#define QCHAT_H

#include "qchat_pch.h"
#include "qTypes.h"
#include "qUser.h"

class qUser;

//получение иконки из статуса
QIcon statusIcon(userStatus st);

class qGeneralChat:public QObject
{
    Q_OBJECT

public:
    static qGeneralChat* i();

public slots:
    void sendWhoRequest();
    void sendOnlinePing();
    void sendOnlineWarning();
    void sendOfflineWarning();

    void sendMessage(const QString& msg);

signals:
    void insertMessage(const QString& msg, bool insertTime, qUser* user);

private slots:
    void processData();

private:
    qGeneralChat(QObject * parent = 0);

    QUdpSocket* generalChatSocket;
};

class qPrivateServer:public QTcpServer
{
    Q_OBJECT
public:
    qPrivateServer(QObject *parent = 0);

    void incomingConnection(int socket);
};

#endif // QCHAT_H
