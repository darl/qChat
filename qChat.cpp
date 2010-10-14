#include "qChat.h"

#include <QtGui>
#include <QtNetwork>

#include "qConfig.h"

QUdpSocket* globalSocket;

QIcon statusIcons(userStatus st)
{
    switch(st)
    {
    case usOnline:
        return QIcon(":/online");
    case usAway:
        return QIcon(":/away");
    case usBusy:
        return QIcon(":/busy");
    case usOffline:
        return QIcon(":/offline");
    default:
        return QIcon();
    }
}

void sendMessage(const QString& msg)
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

void sendWhoRequest()
{
    QByteArray dg;
    dg.insert(0,mtWhoRequest);
    dg.insert(1,status);
    globalSocket->writeDatagram(dg,broadcast,port);
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
