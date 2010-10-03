#include "mainwindow.h"


void MainWindow::insertMessage(QString msg, bool insertTime, QString nick, QHostAddress from)
{
    QString addMsg;

    if(chatArea->toPlainText().size()!=0)
        addMsg+=tr("<br>");

    if(insertTime)
        addMsg+=tr("<font color='gray'>%1</font> ").arg(QTime::currentTime().toString());

    if(nick != "")
        addMsg+=tr("<a href='@%1'><b>%2</b></a>: ").arg(from.toString()).arg(nick);

    addMsg+=msg;

    bool end = (chatArea->verticalScrollBar()->value() == chatArea->verticalScrollBar()->maximum());

    QTextCursor c = chatArea->textCursor();
    c.movePosition(QTextCursor::End);
    c.insertHtml(addMsg);

    if(end) chatArea->verticalScrollBar()->setValue(chatArea->verticalScrollBar()->maximum());
}

void MainWindow::sendClick()
{
    if(!msgLine->text().isEmpty())
        sendMessage(msgLine->text());

    msgLine->clear();
    msgLine->setFocus();
}

void MainWindow::sendPing()
{
    sendOnlinePing();
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
            insertMessage(datagram.data(),true,userList[addr.toString()]->nick,addr);
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

    QToolBar* tb = new QToolBar(tr("Send toolbar"));
    tb->setObjectName("sendToolBar");
    tb->addWidget(msgLine);
    tb->addWidget(sendButton);
    addToolBar(Qt::BottomToolBarArea,tb);

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
    addToolBar(Qt::RightToolBarArea,bb);

    QListView* onlineList = new QListView(this);
    onlineList->setUniformItemSizes(true);
    onlineList->setModel(&userList);
    userList.objectName();

    QDockWidget* tb2 = new QDockWidget(tr("User list"));
    tb2->setObjectName("userListBar");
    tb2->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    tb2->setWidget(onlineList);
    addDockWidget(Qt::LeftDockWidgetArea,tb2);

    setCentralWidget(chatArea);

    globalSocket = new QUdpSocket(this);
    globalSocket->bind(port,QUdpSocket::ReuseAddressHint);

    QTimer* onlinePingTimer = new QTimer(this);

    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));
    connect(msgLine,SIGNAL(returnPressed()),this,SLOT(sendClick()));
    connect(globalSocket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(onlinePingTimer,SIGNAL(timeout()),this,SLOT(sendPing()));

    onlinePingTimer->start(5000);

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
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{

}
