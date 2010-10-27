#ifndef QUSER_H
#define QUSER_H

#include <QDateTime>
#include <QtNetwork>

#include "qTypes.h"
#include "qChat.h"

class qUser:public QObject
{
    Q_OBJECT

public:
    friend class qPrivateServer;
    qUser(QObject* obj = 0);

    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;

    void sendMessage(quint64 confID, const QString& msg);

    bool directConnect();
    bool onlineCheck() {return (status!=usOffline); }

    static qUser* local();

protected:
    QTcpSocket* socket;
    bool connected;

protected slots:
    void processData();
    void connectReady();
    void disconnected();
    void error1(QAbstractSocket::SocketError);

signals:
    //void insertMessage(quint64 confID,const QString& msg, bool insertTime, qUser* user);

public slots:

};
#endif // QUSER_H
