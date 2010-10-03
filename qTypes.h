#ifndef QTYPES_H
#define QTYPES_H

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtGui/QMainWindow>
#include <QTextBrowser>
#include <QLineEdit>
#include <QDialog>
#include <QAbstractListModel>
#include <QDateTime>
#include <QToolBar>
#include <QScrollBar>
#include <QPushButton>
#include <QListView>
#include <QTextCodec>
#include <QDockWidget>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>

enum messageType{
    mtMessage,
    mtOnlinePing,
    mtWhoRequest,
    mtOnlineWarning,
    mtOfflineWarning,
    mtSystemMessage
};

enum userStatus
{
    usOnline,
    usAway,
    usBusy,
    usOffline
};

struct qUser
{
    QHostAddress address;
    QString nick;
    QDateTime lastCheck;
    userStatus status;
};

#endif // QTYPES_H
