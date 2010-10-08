#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qChat.h"
#include "qUserList.h"
#include "qConfig.h"
#include "qTypes.h"
#include "qPrivate.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatArea;
    QLineEdit* msgLine;
    QPushButton* sendButton;
    QSystemTrayIcon* tray;

    void insertMessage(QString msg, bool insertTime = false, qUser* user = NULL);

private slots:
    void sendClick();
    void sendPing();

    void processData();
    void refreshClick();
    void conferenceClick();
    void configClick();
    void aboutClick();
    void onlineCheck();
    void linkClick(const QUrl& url);
    void trayClick(QSystemTrayIcon::ActivationReason ar);
    void exitClick();


protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
