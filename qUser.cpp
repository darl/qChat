#include "qUser.h"

#include "qchat_pch.h"

#include "qConfig.h"
#include "qPrivate.h"
#include "qUserList.h"


qUser::qUser(QObject *obj): QObject(obj)
{
    socket = new QTcpSocket(this);

    connected = false;
    connect(socket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(socket,SIGNAL(connected()),this,SLOT(connectReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));

}

void qUser::error(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}


void qUser::connectReady()
{
    connected = true;
    sendPublicKeyRequest();
}


void qUser::disconnected()
{
    connected = false;
    key = qRsaKey();
}

void qUser::processData()
{
    QByteArray msg(socket->readAll());

    //как минимум 1 байт мы получили
    messageType mt = static_cast<messageType>(msg.at(0));
    msg.remove(0,1);

    if(msg.size()<8) return;
    quint64 confID = *((quint64*)(msg.constData())); //magic
    msg.remove(0,8);

    qPrivate* w = 0;

    switch(mt)
    {
    case mtMessage:
        if(!privateList.privateWindowExist(confID))
            sendConfInfoRequest(confID);
        w = privateList.getPrivateWindow(confID);
        w->insertMessage(qRsa::decrypt(msg,qRsaKey::local()),true,this);
        w->show();
        break;
    case mtConferenceInfo:
        {
            QTextStream strm(msg);
            QList<qUser*> ul;
            QString line;
            qUser* u;
            QList<QHostAddress> la = QHostInfo::fromName(QHostInfo::localHostName()).addresses();
            do {
                line = strm.readLine();
                u = userList[line];
                if(!u) continue;

                if(la.contains(u->address)) //найден локальный пользователь
                    continue;

                ul.append(u);
            } while (!line.isNull());
            if(ul.isEmpty())
            {
                qDebug() <<"empty conf list";
                return;
            }
            w = privateList.getPrivateWindow(confID);
            w->confUserList = ul;
            w->setWindowTitle("Conference with");
            w->users->clear();
            foreach(u,ul)
            {
                new QListWidgetItem(statusIcon(u->status),u->nick,w->users);
                u->directConnect(); //заранее подключаемся
                w->setWindowTitle(w->windowTitle()+" "+u->nick);
            }
        }
        break;
    case mtConferenceInfoRequest:
        sendConfInfo(confID);
        break;
    case mtPublicKey:
        key = qRsaKey(msg);
        break;
    case mtPublicKeyRequest:
        {
            QByteArray ba;
            ba.append(static_cast<char>(mtPublicKey));
            ba.append((char*)&confID,8);
            ba.append(qRsaKey::local().publicKey().toBase64());
            ba.append(':');
            ba.append(qRsaKey::local().module().toBase64());
            if(socket->waitForConnected(100))
                socket->write(ba);
        }
        break;
    default:
        qWarning() << "undifined message type";
    }

}

qUser* qUser::local()
{
    QList<QHostAddress> la = QHostInfo::fromName(QHostInfo::localHostName()).addresses();

    for(QList<QHostAddress>::iterator i = la.begin();i!=la.end();i++)
        if(userList[i->toString()])
            return userList[i->toString()];
    return 0;
}

void qUser::sendMessage(quint64 confID, const QString& msg)
{
    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtMessage));
    baMsg.append((char*)&confID, 8);
    baMsg.append(qRsa::encrypt(msg, key));
    if(socket->waitForConnected(100))
        socket->write(baMsg);
}

void qUser::sendPublicKeyRequest()
{
    QByteArray baMsg;
    quint64 cid = 0;
    baMsg.append(static_cast<char>(mtPublicKeyRequest));
    baMsg.append((char*)&cid,8);
    socket->write(baMsg);
}

void qUser::sendConfInfoRequest(quint64 confID)
{
    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtConferenceInfoRequest));
    baMsg.append((char*)&confID,8);
    if(socket->waitForConnected(100))
        socket->write(baMsg);
}

void qUser::sendConfInfo(quint64 confID)
{
    if(!privateList.privateWindowExist(confID))
        return;

    qPrivate* w = privateList.getPrivateWindow(confID);
    QList<qUser*> ul(w->confUserList);
    ul.append(qUser::local());

    QByteArray baMsg;
    baMsg.append(static_cast<char>(mtConferenceInfo));
    baMsg.append((char*)&confID,8);

    qUser* u;
    foreach(u,ul)
    {
        baMsg.append(u->address.toString()+"\n");
    }

    if(socket->waitForConnected(100))
        socket->write(baMsg);
}

bool qUser::directConnect()
{
    if(connected) return true;
    socket->connectToHost(address.toString(),port+1);
    return false;
}
