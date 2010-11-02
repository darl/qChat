#ifndef QUSERLIST_H
#define QUSERLIST_H

#include "qchat_pch.h"

#include "qUser.h"


class QUserListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit QUserListModel(QObject *parent = 0);

    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    //для прозрачной работы с хеш таблицей
    qUser* operator[] (const QString& n);

signals:
    void statusChanged(qUser* u);
    void nowOnline(qUser* u);
    void nowOffline(qUser* u);

public slots:
    void updateUser(QHostAddress addr, QString nick, userStatus us);
    void removeUser(QHostAddress addr);
    void clearOfflineUsers();

protected:
    QHash<QString,qUser*> userList;
};

extern QUserListModel userList;

#endif // QUSERLIST_H
