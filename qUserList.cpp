#include "qUserList.h"

QUserListModel userList;


QUserListModel::QUserListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}


Qt::ItemFlags QUserListModel::flags ( const QModelIndex & index ) const
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
        return QIcon(statusIcons[u->status]);
        break;
    }
    return QVariant::Invalid;
}

int QUserListModel::rowCount ( const QModelIndex & parent ) const
{
    return userList.count();
}

QVariant QUserListModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    return QString("");
}

void QUserListModel::updateUser(QHostAddress addr, QString nick, userStatus us)
{
    qUser* user;
    bool adding=false;
    if(userList.contains(addr.toString()))
        user = userList.value(addr.toString());
    else
    {
        beginInsertRows(index(0),0,0);
        user = new qUser;
        adding=true;
    }

    user->address=addr;
    user->lastCheck = QDateTime::currentDateTime();
    user->nick = nick;
    user->status = us;

    userList[addr.toString()] = user;
    if(adding)
        endInsertRows();
    else
        emit dataChanged(QModelIndex(),QModelIndex());
}

void QUserListModel::removeUser(QHostAddress addr)
{
    beginRemoveRows(index(0),0,0);
    userList.remove(addr.toString());
    endRemoveRows();
}

qUser* QUserListModel::operator[] (QString n)
{
    return userList[n];
}
