#include "qChat.h"

quint16 port = 49675;                                   //default port
QHostAddress broadcast(QObject::tr("172.18.255.255"));  //broadcasting mask

QString nick = QHostInfo::localHostName();                     //default Nickname
userStatus status = usOnline;

QUdpSocket* globalSocket;

QString statusIcons[] = {":/online",":/away",":/busy",":/offline"};

bool invisibleMode = false;
bool adminMode = false;
bool showOfflineUsers = false;



void sendMessage(QString msg)
{
    //запрет посылки html тегов
    if(!adminMode) msg.replace('<',"&lt;");

    //распознование ссылок
    msg.replace(QRegExp("(^|[\\n ])([\\w]*)((ht|f)tp(s?)://[\\w]+[^ \\\"\\n\\r\\t<]*)"),"\\1\\2<a href=\"\\3\">\\3</a>");
    msg.replace(QRegExp("(^|[\\n ])([\\w]*)((www|ftp)\\.[^ \\,\\\"\\t\\n\\r<]*)"),"\\1\\2<a href=\"http://\\3\">\\3</a>");
    msg.replace(QRegExp("(^|[\\n ])([a-z0-9&\\-_\\.]+)@([\\w\\-]+\\.([\\w\\-\\.]+)+)"),"\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>");

    QByteArray dg=msg.toAscii();
    dg.insert(0,mtMessage);
    dg.insert(1,status);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void sendWhoRequest()
{
    QByteArray dg;
    dg.insert(0,mtWhoRequest);
    dg.insert(1,status);
}

void sendOnlinePing()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOnlinePing);
    dg.insert(1,status);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void sendOnlineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOnlineWarning);
    dg.insert(1,status);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}

void sendOfflineWarning()
{
    if(invisibleMode) return;
    QByteArray dg;
    dg.insert(0,mtOfflineWarning);
    dg.insert(1,usOffline);
    dg.insert(2,nick);
    globalSocket->writeDatagram(dg,broadcast,port);
}
