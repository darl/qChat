#include "qUser.h"

#include <QByteArray>

#include "qConfig.h"

qUser::qUser(QObject *obj): QObject(obj)
{
    socket = new QSslSocket(this);
    connected = false;
    connect(socket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(socket,SIGNAL(connected()),this,SLOT(connectReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));

}

void qUser::connectReady()
{
    connected = true;
}

void qUser::disconnected()
{
    connected = false;

}

void qUser::processData()
{
    QByteArray msg(socket->readAll());

    messageType mt = static_cast<messageType>(msg.at(0));

    qint64 confID = *((qint64*)(msg.data()+1));//magic

//    qUser* t = this;
    msg.remove(0,9);
    QString msg_r;
    msg_r = tr("PRIVATE(%2): %1").arg(QString(msg)).arg(confID);
    emit insertMessage(confID,msg_r,true,this);
}

void qUser::sendMessage(qint64 confID, QString& msg)
{
    if(!connected) return;
    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtMessage));
    baMsg.append((char*)&confID,8);
    baMsg.append(msg);

    socket->write(baMsg);
}

bool qUser::directConnect()
{
    if(connected) return true;
    socket->connectToHostEncrypted(address.toString(),port+1);
    return false;
}
