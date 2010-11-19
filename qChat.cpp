#include "qChat.h"

#include "qConfig.h"
#include "qUserList.h"
#include "qUser.h"
#include "qUpdater.h"

/*получение иконки для статуса*/
QString statusIconStr(userStatus status)
{
    switch(status)
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

/*получение пути к иконке для статуса*/
QIcon statusIcon(userStatus status)
{
    return QIcon(statusIconStr(status));
}

qGeneralChat* qGeneralChat::i()
{
    static qGeneralChat* instance = 0;

    if(!instance)
        instance = new qGeneralChat;
    return instance;
}

/*обработка входящих пакетов*/
void qGeneralChat::processData()
{
    while (generalChatSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress addr;
        datagram.resize(generalChatSocket->pendingDatagramSize());
        generalChatSocket->readDatagram(datagram.data(), datagram.size(), &addr);

        if(datagram.size()<2) return;
        messageType msgType = static_cast<messageType>(datagram.at(0));
        userStatus usrStatus = static_cast<userStatus>(datagram.at(1));
        datagram.remove(0,2);

        switch(msgType)
        {
        case mtMessage:
            emit insertMessage(datagram.data(), true, userList[addr.toString()]);
            break;
        case mtOnlinePing:
            userList.updateUser(addr, datagram.data(), usrStatus);
            break;
        case mtWhoRequest:
            sendOnlinePing();
            break;
        case mtOnlineWarning:
            {
                if(datagram.size() < 4) return;
                quint32 ver = *((quint32*)datagram.constData());
                datagram.remove(0,4);
                quint32 syst = ver >> 24;
                ver &= 0xFFFFFF;
                qDebug() << datagram.data() << "ver: " << ver << " syst: " << syst;
                userList.updateUser(addr, datagram.data(), usrStatus);
                sendOnlinePing();
            }
            break;
        case mtOfflineWarning:
            userList.removeUser(addr);
            break;
        case mtSystemMessage:
            emit insertMessage(tr("<font color='red'>%1</font>").arg(datagram.data()), true, NULL);
            break;
        case mtBot:
            {
                int firstBR = datagram.indexOf("<br>");
                if(firstBR < 0)
                    emit insertMessage(tr("<font color='%3'><img src='%2'>%1</font>")
                                       .arg(datagram.data(), statusIconStr(usrStatus), "#005000"),true,NULL);
                else
                {
                    QString firstLine = datagram.left(firstBR);
                    QByteArray m = datagram.remove(0,firstBR);
                    QString otherLines = m.toBase64();
                    emit insertMessage(tr("<font color='%3'><img src='%2'>%1 <a href='qbot://#%4'>more...</font>")
                                       .arg(firstLine, statusIconStr(usrStatus), "#005000", otherLines),true,NULL);
                }
            }
            break;
        default:
            emit insertMessage(tr("<font color='red'><i>Unknown message (Type: %1, status: %2): %3</i></font>")
                               .arg(msgType).arg(usrStatus).arg(datagram.constData()),true,NULL);
        }
    }
}

/*отправка сообщения*/
void qGeneralChat::sendMessage(const QString& msg)
{
    QString originalMsg = msg;
    //запрет посылки html тегов
    if(!htmlTags) originalMsg.replace('<', "&lt;");

    //распознование ссылок
    originalMsg.replace(QRegExp("(^|[\\n])([\\w]*)((ht|f)tp(s?)://[\\w]+[^ \\\"\\n\\r\\t<]*)"),
                        "\\1\\2<a href=\"\\3\">\\3</a>");
    originalMsg.replace(QRegExp("(^|[\\n])([\\w]*)((www|ftp)\\.[^ \\,\\\"\\t\\n\\r<]*)"),
                        "\\1\\2<a href=\"http://\\3\">\\3</a>");
    originalMsg.replace(QRegExp("(^|[\\n])([a-z0-9&\\-_\\.]+)@([\\w\\-]+\\.([\\w\\-\\.]+)+)"),
                        "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>");
    //распознование magnet ссылки
    originalMsg.replace(QRegExp("(^|[\\n])(magnet:\\?xt=.*\\&xl=(.[0-9]*)\\&dn=)(.*)"),
                        "\\1<a href=\"\\2\\4\">\\4</a>");

    QByteArray dg;
    dg.append(mtMessage);
    dg.append(status);
    dg.append(originalMsg);
    generalChatSocket->writeDatagram(dg, broadcast, port);
}

void qGeneralChat::sendWhoRequest()
{
    QByteArray dg;
    dg.append(mtWhoRequest);
    dg.append(status);
    generalChatSocket->writeDatagram(dg, broadcast, port);
}

void qGeneralChat::sendOnlinePing()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.append(mtOnlinePing);
    dg.append(status);
    dg.append(nick);
    generalChatSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendOnlineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.append(mtOnlineWarning);
    dg.append(status);
    quint32 ver = qUpdater::versionFull();
    dg.append((char*)&ver, sizeof(unsigned int));
    dg.append(nick);
    generalChatSocket->writeDatagram(dg,broadcast,port);
}

void qGeneralChat::sendOfflineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.append(mtOfflineWarning);
    dg.append(usOffline);
    dg.append(nick);
    generalChatSocket->writeDatagram(dg,broadcast,port);
}

qGeneralChat::qGeneralChat(QObject* obj) : QObject(obj)
{
    generalChatSocket = new QUdpSocket(this);
    generalChatSocket->bind(port,QUdpSocket::ReuseAddressHint);

    connect(generalChatSocket,SIGNAL(readyRead()),this,SLOT(processData()));
}


qPrivateServer::qPrivateServer(QObject *parent) : QTcpServer(parent)
{
    listen(QHostAddress::Any, port+1);
}

void qPrivateServer::incomingConnection(int socket)
{
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

        connect(s,SIGNAL(readyRead()),u,SLOT(processData()));
        connect(s,SIGNAL(connected()),u,SLOT(connectReady()));
        connect(s,SIGNAL(disconnected()),u,SLOT(disconnected()));
        connect(s,SIGNAL(error(QAbstractSocket::SocketError)),u,SLOT(error1(QAbstractSocket::SocketError)));
        u->sendPublicKeyRequest();
        u->connected=true;
    }
}

