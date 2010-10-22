#include "qUser.h"

#include <QByteArray>

//#include <QtDebug>

#include "qConfig.h"

qUser::qUser(QObject *obj): QObject(obj)
{
    socket = new QSslSocket(this);
    connected = false;
    connect(socket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(socket,SIGNAL(connected()),this,SLOT(connectReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error1(QAbstractSocket::SocketError)));

}

void qUser::error1(QAbstractSocket::SocketError err)
{
    qDebug() << "error1";
    qDebug() << err;
}

void qUser::connectReady()
{
    qDebug() <<"connected!";
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

void qUser::sendMessage(qint64 confID, const QString& msg)
{
    qDebug()<<connected;
    qDebug()<<msg;
    if(!connected) return;
    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtMessage));
    baMsg.append((char*)&confID,8);
    baMsg.append(msg);

    socket->write(baMsg);
}

bool qUser::directConnect()
{
    qDebug() << connected;
    if(connected) return true;
    qDebug() << "connect try";
    socket->connectToHostEncrypted(address.toString(),port+1);
    return false;
}
