#ifndef QUSERLIST_H
#define QUSERLIST_H

#include "qChat.h"
#include "qTypes.h"
#include "mainwindow.h"


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
    QStringList clearOfflineUsers();

    qUser* operator[] (QString n);

signals:

public slots:

};

extern QUserListModel userList;

#endif // QUSERLIST_H
