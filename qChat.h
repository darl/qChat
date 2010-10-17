#ifndef QCHAT_H
#define QCHAT_H

#include "qTypes.h"

class QUdpSocket;
class QString;
class QIcon;

QIcon statusIcons(userStatus st);
//unsigned char us2c(userStatus st) {return static_cast<unsigned char>(st);};

class qGeneralChat:public QObject
{
    Q_OBJECT

public:
    qGeneralChat(QObject * parent = 0);
    virtual ~qGeneralChat();
    void sendMessage(const QString& msg);
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

#endif // QCHAT_H
