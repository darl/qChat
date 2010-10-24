#include "qUser.h"

#include <QByteArray>

#include <QMessageBox>

#include "qConfig.h"

qUser::qUser(QObject *obj): QObject(obj)
{
    socket = new QTcpSocket(this);
    //socket->ignoreSslErrors();
    connected = false;
    connect(socket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(socket,SIGNAL(connected()),this,SLOT(connectReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error1(QAbstractSocket::SocketError)));
   // connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(error2(QList<QSslError>)));
   // connect(socket,SIGNAL(encrypted()),this,SLOT(encryptReady()));
}

void qUser::error1(QAbstractSocket::SocketError err)
{
    qDebug() << "error1 " << err;
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

    quint64 confID = *((quint64*)(msg.data()+1));//magic

    msg.remove(0,9);
    QString msg_r;
    msg_r = tr("PRIVATE(%2): %1").arg(QString(msg)).arg(confID);
    emit insertMessage(confID,msg_r,true,this);
}

void qUser::sendMessage(quint64 confID, const QString& msg)
{
    qDebug()<<msg;
    //if(!connected) return;
    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtMessage));
    baMsg.append((char*)&confID,8);
    baMsg.append(msg);
    socket->write(baMsg);
}

bool qUser::directConnect()
{
    if(connected) return true;
    qDebug() << "connect try";
    qDebug() <<address.toString();
    //socket->setLocalCertificate("qchat.csr");
    //socket->setPrivateKey("qchat.key");
    socket->connectToHost(address.toString(),port+1);
    return false;
}
