#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qChat.h"
#include "qUserList.h"
#include "qConfig.h"
#include "qTypes.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatArea;
    QLineEdit* msgLine;
    QPushButton* sendButton;

    void insertMessage(QString msg, bool insertTime = false, qUser* user = NULL);

private slots:
    void sendClick();
    void sendPing();

    void processData();
    void refreshClick();
    void configClick();
    void aboutClick();

public slots:
    void onlineCheck();
protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
