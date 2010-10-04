#include "mainwindow.h"


void MainWindow::insertMessage(QString msg, bool insertTime, qUser* user)
{
    QString addMsg;

    if(chatArea->toPlainText().size()!=0)
        addMsg+=tr("<br>");

    if(insertTime)
        addMsg+=tr("<font color='gray'>%1</font> ").arg(QTime::currentTime().toString());

    if(user)
        addMsg+=tr("<a href='@%1'><b>%2</b></a>: ").arg(user->address.toString()).arg(user->nick);

    addMsg+=msg;

    bool end = (chatArea->verticalScrollBar()->value() == chatArea->verticalScrollBar()->maximum());

    QTextCursor c = chatArea->textCursor();
    c.movePosition(QTextCursor::End);
    c.insertHtml(addMsg);

    if(end) chatArea->verticalScrollBar()->setValue(chatArea->verticalScrollBar()->maximum());
}

//нажатие кнопки отправить
void MainWindow::sendClick()
{
    if(!msgLine->text().isEmpty())
        sendMessage(msgLine->text());

    msgLine->clear();
    msgLine->setFocus();
}

//обработка таймера onlinePingTimer
void MainWindow::sendPing()
{
    sendOnlinePing();
}

void MainWindow::onlineCheck()
{
    QHash<QString, qUser*>::iterator i = userList.userList.begin();
    while (i != userList.userList.end()) {
        qUser* u = *i;
        if (((u->status!=usOffline) || !showOfflineUsers) &&   //пользователь не в оффлайне
            (u->lastCheck.msecsTo(QDateTime::currentDateTime())>=20000))
        {    //20 секунд неактивности
            insertMessage(tr("<font color='gray'>%1 has gone offline(timeout)</font>").arg(u->nick),true);
            if(showOfflineUsers)
                u->status=usOffline;
            else
                i = userList.userList.erase(i);
        }
        else
        {
            ++i;
        }
    }
    emit userList.dataChanged(QModelIndex(),QModelIndex());
}

void MainWindow::refreshClick()
{
    onlineCheck();
    sendWhoRequest();
}

void MainWindow::configClick()
{
    configDialog->show();
}

void MainWindow::aboutClick()
{
    QMessageBox::about(this, tr("About qChat"),
                 tr("<img src=':/about'><b>qChat</b><br>"
                    "server-less chat client<br>"
                    "developed by Darl"));
}

void MainWindow::processData()
{
    while (globalSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress addr;
        datagram.resize(globalSocket->pendingDatagramSize());
        globalSocket->readDatagram(datagram.data(), datagram.size(),&addr);
        messageType mt = (messageType)(char)datagram[0];
        userStatus us = (userStatus)(char)datagram[1];
        datagram.remove(0,2);
        switch(mt)
        {
        case mtMessage:
            insertMessage(datagram.data(),true,userList[addr.toString()]);
            break;
        case mtOnlinePing:
            userList.updateUser(addr,datagram.data(),us);
            break;
        case mtWhoRequest:
            sendOnlinePing();
            break;
        case mtOnlineWarning:
            userList.updateUser(addr,datagram.data(),us);
            insertMessage(tr("<font color='gray'>%1 has come online</font>").arg(datagram.data()),true);
            sendOnlinePing();
            break;
        case mtOfflineWarning:
            userList.removeUser(addr);
            insertMessage(tr("<font color='gray'>%1 has gone offline</font>").arg(datagram.data()),true);
            break;
        case mtSystemMessage:
            //
            insertMessage(tr("<font color='red'>%1</font>").arg(datagram.data()),true);
            break;
        default:
            insertMessage(tr("<font color='red'><b>Unknown message (Type: %1, status: %2): %3</b></font>").arg(mt).arg(us).arg(datagram.data()),true);
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    chatArea = new QTextBrowser(this);
    chatArea->setOpenLinks(false);

    msgLine = new QLineEdit(this);
    msgLine->setPlaceholderText(tr("Message"));

    sendButton = new QPushButton(tr("send"));

    QToolBar* bb = new QToolBar(tr("Button bar"));
    bb->setObjectName("buttonBar");
    QPushButton* refreshButton = new QPushButton(QIcon(":/refresh"),"");
    refreshButton->setFlat(true);
    QPushButton* configButton = new QPushButton(QIcon(":/config"),"");
    configButton->setFlat(true);
    QPushButton* aboutButton = new QPushButton(QIcon(":/about"),"");
    aboutButton->setFlat(true);
    bb->addWidget(refreshButton);
    bb->addWidget(configButton);
    bb->addWidget(aboutButton);
    addToolBar(Qt::BottomToolBarArea,bb);

    QToolBar* tb = new QToolBar(tr("Send toolbar"));
    tb->setObjectName("sendToolBar");
    tb->addWidget(msgLine);
    tb->addWidget(sendButton);
    addToolBar(Qt::BottomToolBarArea,tb);

    QListView* onlineList = new QListView(this);
    onlineList->setUniformItemSizes(true);
    onlineList->setModel(&userList);
    userList.objectName();

    QDockWidget* ul = new QDockWidget(tr("User list"));
    ul->setObjectName("userListBar");
    ul->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    ul->setWidget(onlineList);
    addDockWidget(Qt::LeftDockWidgetArea,ul);

    setCentralWidget(chatArea);

    globalSocket = new QUdpSocket(this);
    globalSocket->bind(port,QUdpSocket::ReuseAddressHint);

    QTimer* onlinePingTimer = new QTimer(this);
    QTimer* onlineCheckTimer = new QTimer(this);

    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));
    connect(msgLine,SIGNAL(returnPressed()),this,SLOT(sendClick()));
    connect(globalSocket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(onlinePingTimer,SIGNAL(timeout()),this,SLOT(sendPing()));
    connect(onlineCheckTimer,SIGNAL(timeout()),this,SLOT(onlineCheck()));
    connect(refreshButton,SIGNAL(clicked()),this,SLOT(refreshClick()));
    connect(configButton,SIGNAL(clicked()),this,SLOT(configClick()));
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutClick()));

    onlinePingTimer->start(5000);
    onlineCheckTimer->start(15000);

    insertMessage(tr("<font color='gray'>qChat alpha - %1</font>").arg(QHostInfo::localHostName()));
    sendOnlineWarning();
    sendWhoRequest();

    QSettings settings("qChat");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("qChat");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    sendOfflineWarning();
    delete configDialog;
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{

}
