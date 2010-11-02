#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qchat_pch.h"
#include "qChat.h"
#include "qUser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatBrowser;
    QLineEdit* msgEdit;
    QPushButton* sendButton;
    QSystemTrayIcon* systemTray;
    QListView* userListView;

    void createUI();
    void createTray();
    void createTimers();

    void loadSettings();
    void saveSettings();

    qGeneralChat* general;

    qPrivateServer* privateServer;

    //список отправленных сообщений для быстрого ввода
    QList<QString> previusMessages;
    QList<QString>::iterator currentMessage;

private slots:
    void insertMessage(const QString& msg, bool insertTime = false, qUser* userFrom = NULL);

    void nowOnline(qUser* u);
    void nowOffline(qUser* u);

    void sendClick();
    void refreshClick();
    void conferenceClick();
    void configClick();
    void aboutClick();
    void linkClick(const QUrl& url);
    void trayClick(QSystemTrayIcon::ActivationReason);
    void exitClick();

    void setOnlineStatus();
    void setAwayStatus();
    void setBusyStatus();
protected:
    bool eventFilter(QObject *obj, QEvent *event);

public:
    MainWindow(QWidget *parent = 0);

};

#endif // MAINWINDOW_H
