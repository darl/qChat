#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

class QTextBrowser;
class QLineEdit;
class QPushButton;
class qUser;
class QUrl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatArea;
    QLineEdit* msgLine;
    QPushButton* sendButton;
    QSystemTrayIcon* tray;

    void insertMessage(const QString& msg, bool insertTime = false, qUser* user = NULL);

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
    void trayClick(QSystemTrayIcon::ActivationReason);
    void exitClick();


protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
