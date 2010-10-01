#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDockWidget>
#include <QTime>
#include <QtNetwork/QUdpSocket>
#include <QToolBar>
#include <QScrollBar>
#include <QListView>
#include <QSplitter>
#include <QStandardItemModel>

#include "qChat.h"
#include "qUserList.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTextBrowser* chatArea;
    QLineEdit* msgLine;
    QPushButton* sendButton;

    void insertMessage(QString msg, bool insertTime = false, QString nick = "", QHostAddress from = QHostAddress::Null);

private slots:
    void sendClick();
    void sendPing();
    void processData();

protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
