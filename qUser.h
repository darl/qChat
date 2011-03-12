#ifndef QUSER_H
#define QUSER_H

#include "qchat_pch.h"

#include "qTypes.h"
#include "qChat.h"
#include "qRsa.h"

class qUser:public QObject
{
    Q_OBJECT

    friend class qPrivateServer;

public:
    qUser(QObject* obj = 0);

    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;

    void sendMessage(quint64 confID, const QString& msg);
    void sendPublicKeyRequest();
    void sendConfInfoRequest(quint64 confID);
    void sendConfInfo(quint64 confID);

    bool directConnect();

    //указатель на локального пользователя
    static qUser* local();

protected:
    QTcpSocket* socket;
    bool connected;
    qRsaKey key;

protected slots:
    void processData();
    void connectReady();
    void disconnected();
    void error(QAbstractSocket::SocketError);
};
#endif // QUSER_H
