#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

class QTextBrowser;
class QLineEdit;
class QPushButton;
class qUser;
class QUrl;
class qGeneralChat;
class qPrivateServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatArea;
    QLineEdit* msgLine;
    QPushButton* sendButton;
    QSystemTrayIcon* tray;

    void createUI();
    void createTray();
    void createTimers();

    void loadSettings();
    void saveSettings();

    qGeneralChat* general;

    qPrivateServer* ps;

private slots:
    void insertMessage(const QString& msg, bool insertTime = false, qUser* user = NULL);
    void insertMessage(qint64 confID ,const QString& msg, bool insertTime = false, qUser* user = NULL);

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


protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
