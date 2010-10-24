#include "qChat.h"

#include <QtGui>
#include <QtNetwork>

#include "qConfig.h"
#include "qUserList.h"
#include "qUser.h"

QString statusIconsStr(userStatus st)
{
    switch(st)
    {
    case usOnline:
        return (":/online");
    case usAway:
        return (":/away");
    case usBusy:
        return (":/busy");
    case usOffline:
        return (":/offline");
    case usCounterStrike:
        return (":/cs");
    case usDota:
        return (":/dota");
    case usQuake3:
        return (":/q3");
    case usQuiz:
        return (":/quiz");
    default:
        return (":/unknown");
    }
}

QIcon statusIcons(userStatus st)
{
    return QIcon(statusIconsStr(st));
}

void qGeneralChat::processData()
{
    while (globalSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress addr;
        datagram.resize(globalSocket->pendingDatagramSize());
        globalSocket->readDatagram(datagram.data(), datagram.size(),&addr);
        messageType mt = static_cast<messageType>(datagram.at(0));
        userStatus us = static_cast<userStatus>(datagram.at(1));
        datagram.remove(0,2);
        switch(mt)
        {
        case mtMessage:
            emit insertMessage(datagram.data(),true,userList[addr.toString()]);
            break;
        case mtOnlinePing:
            userList.updateUser(addr,datagram.data(),us);
            break;
        case mtWhoRequest:
            sendOnlinePing();
            break;
        case mtOnlineWarning:
            userList.updateUser(addr,datagram.data(),us);
            sendOnlinePing();
            break;
        case mtOfflineWarning:
            userList.removeUser(addr);
            break;
        case mtSystemMessage:
            emit insertMessage(tr("<font color='red'>%1</font>").arg(datagram.data()),true,NULL);
            break;
        case mtBot:
            emit insertMessage(QString::number(us),true,NULL);// для отладки с одним нехорошим человеком
            emit insertMessage(tr("<font color='#005500'><img src='%2'>%1</font>").arg(datagram.data()).arg(statusIconsStr(us)),true,NULL);
            break;
        default:
            emit insertMessage(tr("<font color='red'><b>Unknown message (Type: %1, status: %2): %3</b></font>").arg(mt).arg(us).arg(datagram.data()),true,NULL);
        }
    }
}

void qGeneralChat::sendMessage(const QString& msg)
{
    QString originalMsg = msg;
    //запрет посылки html тегов
    if(!htmlTags) originalMsg.replace('<',"&lt;");

    //распознование ссылок
    originalMsg.replace(QRegExp("(^|[\\n ])([\\w]*)((ht|f)tp(s?)://[\\w]+[^ \\\"\\n\\r\\t<]*)"),"\\1\\2<a href=\"\\3\">\\3</a>");
    originalMsg.replace(QRegExp("(^|[\\n ])([\\w]*)((www|ftp)\\.[^ \\,\\\"\\t\\n\\r<]*)"),"\\1\\2<a href=\"http://\\3\">\\3</a>");
    originalMsg.replace(QRegExp("(^|[\\n ])([a-z0-9&\\-_\\.]+)@([\\w\\-]+\\.([\\w\\-\\.]+)+)"),"\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>");

    QByteArray dg=originalMsg.toAscii();
    dg.insert(0,mtMessage);
    dg.insert(1,status);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendWhoRequest()
{
    QByteArray dg;
    dg.insert(0,mtWhoRequest);
    dg.insert(1,status);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendOnlinePing()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOnlinePing);
    dg.insert(1,status);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendOnlineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOnlineWarning);
    dg.insert(1,status);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendOfflineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOfflineWarning);
    dg.insert(1,usOffline);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}

qGeneralChat::qGeneralChat(QObject* obj) : QObject(obj)
{
    globalSocket = new QUdpSocket(this);
    globalSocket->bind(port,QUdpSocket::ReuseAddressHint);

    connect(globalSocket,SIGNAL(readyRead()),this,SLOT(processData()));
}


qPrivateServer::qPrivateServer(QObject *parent) : QTcpServer(parent)
{
    listen(QHostAddress::Any, port+1);
    qDebug() << "server started";
    qDebug() << port;
}

void qPrivateServer::incomingConnection(int socket)
{
    qDebug() << "new connection";

    QTcpSocket* s = new QTcpSocket();
    s->setSocketDescriptor(socket);


    if(qUser* u = userList[s->peerAddress().toString()])
    {
        if(u->connected)
        {
            s->disconnect();
            s->deleteLater();
            return;
        }
        u->socket->deleteLater();
        u->socket = s;
        //s->ignoreSslErrors();
        //s->setLocalCertificate("qchat.csr");
        //QSslConfiguration a;
        //s->setPrivateKey("qchat.key");
        //s->setSslConfiguration(a);
        //s->setCiphers("DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:AES256-SHA:EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:DES-CBC3-SHA:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:AES128-SHA:RC4-SHA:RC4-MD5:EDH-RSA-DES-CBC-SHA:EDH-DSS-DES-CBC-SHA:DES-CBC-SHA:EXP-EDH-RSA-DES-CBC-SHA:EXP-EDH-DSS-DES-CBC-SHA:EXP-DES-CBC-SHA:EXP-RC2-CBC-MD5:EXP-RC4-MD5");

        //s->setCaCertificates(PathToCACertDir("/etc/ssl/certs");
        connect(s,SIGNAL(readyRead()),u,SLOT(processData()));
        connect(s,SIGNAL(connected()),u,SLOT(connectReady()));
        connect(s,SIGNAL(disconnected()),u,SLOT(disconnected()));
        connect(s,SIGNAL(error(QAbstractSocket::SocketError)),u,SLOT(error1(QAbstractSocket::SocketError)));
        //connect(s,SIGNAL(sslErrors(QList<QSslError>)),u,SLOT(error2(QList<QSslError>)));
        //connect(s,SIGNAL(encrypted()),u,SLOT(encryptReady()));
        //u->connected=true; //!!!!!!
    }
}

