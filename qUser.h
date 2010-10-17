#ifndef QUSER_H
#define QUSER_H

#include <QDateTime>
#include <QtNetwork>

#include "qTypes.h"

class qUser:public QObject
{
    Q_OBJECT

public:
    qUser(QObject* obj = 0);

    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;

    void sendMessage(qint64 confID, QString& msg);

    bool directConnect();
    bool onlineCheck();

protected:
    QSslSocket* socket;

signals:

public slots:

};
#endif // QUSER_H
