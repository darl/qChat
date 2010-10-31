#include "qUserList.h"

#include <QIcon>

#include "qChat.h"
#include "qConfig.h"
#include "qUser.h"

QUserListModel userList;

QUserListModel::QUserListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

Qt::ItemFlags QUserListModel::flags ( const QModelIndex & /*index*/ ) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant QUserListModel::data ( const QModelIndex & index, int role) const
{
    int r=index.row();
    QHash<QString, qUser*>::iterator i(userList.begin());
    i+=r;
    qUser* u = *i;
    switch (role)
    {
    case Qt::DisplayRole:
        return u->nick;
        break;
    case Qt::ToolTipRole:
        return u->address.toString();
        break;
    case Qt::DecorationRole:
        return statusIcons(u->status);
        break;
    }
    return QVariant::Invalid;
}

int QUserListModel::rowCount ( const QModelIndex & /*parent*/ ) const
{
    return userList.count();
}

QVariant QUserListModel::headerData ( int /*section*/, Qt::Orientation /*orientation*/, int /*role*/ ) const
{
    return QString("");
}

void QUserListModel::updateUser(QHostAddress addr, QString nick, userStatus us)
{
    qUser* user = 0;
    bool adding = false;

    if(userList.contains(addr.toString()))
        user = userList.value(addr.toString());
    else
    {
        beginInsertRows(index(0),0,0);
        user = new qUser;
        adding=true;
    }

    user->address = addr;
    user->lastCheck = QDateTime::currentDateTime();
    user->nick = nick;
    if(((!adding) && (user->status == usOffline)) || adding)
        emit nowOnline(user);  //вызываем сигнал при добавлении нового или возврате в онлайн старого пользователя
    if(user->status != us)
    {
        user->status = us;
        emit statusChanged(user);
    }

    userList[addr.toString()] = user;

    if(adding)
        endInsertRows();
    else
        emit dataChanged(QModelIndex(),QModelIndex());
}

void QUserListModel::removeUser(QHostAddress addr)
{
    if(!userList.contains(addr.toString())) return;

    userList.value(addr.toString())->status = usOffline;
    emit nowOffline(userList.value(addr.toString()));

    emit dataChanged(QModelIndex(),QModelIndex());
}

void QUserListModel::clearOfflineUsers()
{
    QHash<QString, qUser*>::iterator i = userList.begin();
    while (i != userList.end()) {
        qUser* u = *i;
        if(u->status!=usOffline)
            if (u->lastCheck.msecsTo(QDateTime::currentDateTime())>=20000)
            {    //20 секунд неактивности
                u->status=usOffline;
                emit nowOffline(u);
            }
        ++i;
    }
    emit dataChanged(QModelIndex(),QModelIndex());
}

qUser* QUserListModel::operator[] (const QString& n)
{
    if(userList.contains(n))
        return userList.value(n);
    else
        return NULL;
}
