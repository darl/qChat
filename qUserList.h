#ifndef QUSERLIST_H
#define QUSERLIST_H

#include <QAbstractListModel>
#include <QtNetwork>
#include "qChat.h"


struct qUser
{
    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;
};


class QUserListModel : public QAbstractListModel
{
    Q_OBJECT

protected:
    QHash<QString,qUser*> userList;

public:
    explicit QUserListModel(QObject *parent = 0);
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    void updateUser(QHostAddress addr, QString nick, userStatus us);
    void removeUser(QHostAddress addr);

    qUser* operator[] (QString n);

signals:

public slots:

};

extern QUserListModel userList;

extern void updateUser(QHostAddress addr, QString nick, userStatus us);
extern void removeUser(QHostAddress addr, QString nick, userStatus us);

#endif // QUSERLIST_H
