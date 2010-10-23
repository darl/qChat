#ifndef QCHAT_H
#define QCHAT_H

#include <QObject>
#include <QTcpServer>
#include "qTypes.h"

class QUdpSocket;
class QString;
class QIcon;
class qUser;

QIcon statusIcons(userStatus st);
//unsigned char us2c(userStatus st) {return static_cast<unsigned char>(st);};

class qGeneralChat:public QObject
{
    Q_OBJECT

public:
    qGeneralChat(QObject * parent = 0);
    void sendMessage(const QString& msg);

public slots:
    void sendWhoRequest();
    void sendOnlinePing();
    void sendOnlineWarning();
    void sendOfflineWarning();

signals:
    void insertMessage(const QString& msg, bool insertTime, qUser* user);

private slots:
    void processData();

private:
    QUdpSocket* globalSocket;

};

class qPrivateServer:public QTcpServer
{
    Q_OBJECT
public:
    qPrivateServer(QObject *parent = 0);

    void incomingConnection(int socket);

private slots:
    void error1();

};

#endif // QCHAT_H
