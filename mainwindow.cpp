#include "mainwindow.h"


void MainWindow::insertMessage(QString msg, bool insertTime, qUser* user)
{
    QString addMsg;

    if(chatArea->toPlainText().size()!=0)
        addMsg+=tr("<br>");

    if(insertTime)
        addMsg+=tr("<font color='gray'>%1</font> ").arg(QTime::currentTime().toString());

    if(user)
        addMsg+=tr("<a href='qchat://%1'><b>%2</b></a>: ").arg(user->address.toString()).arg(user->nick);

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
    QStringList torem(userList.clearOfflineUsers());
    foreach (QString str, torem)
        insertMessage(tr("<font color='gray'>%1 has gone offline(timeout)</font>").arg(str),true);
}

void MainWindow::refreshClick()
{
    onlineCheck();
    sendWhoRequest();
}

void MainWindow::conferenceClick()
{
    qPrivate* wnd = new qPrivate();
    wnd->show();
}

void MainWindow::configClick()
{
    configDialog->show();
}

void MainWindow::aboutClick()
{
    //placeholder
    QMessageBox::about(this, tr("About qChat"),
                 tr("<img src=':/about'><b>qChat</b><br>"
                    "server-less chat client<br>"
                    "developed by Darl"));
}

void MainWindow::linkClick(const QUrl& url)
{
    qUser* us = userList[url.host()];
    if(!us) return;
    msgLine->setText(us->nick+", "+msgLine->text());
    msgLine->setFocus();
}

void MainWindow::trayClick(QSystemTrayIcon::ActivationReason ar)
{
    if(ar==QSystemTrayIcon::Trigger)
        setVisible(!isVisible());
    if(isVisible())
    {
        raise();
        activateWindow();
    }
}

void MainWindow::exitClick()
{
    QSettings settings("qChat");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    sendOfflineWarning();
    tray->hide();
    delete configDialog;
    exit(0);
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
    chatArea->setOpenExternalLinks(true);   //ссылки будут открываться внешне, а не будут пытаться открыться внутри виджета
    QDesktopServices::setUrlHandler("qchat",this,"linkClick"); //это какая-то магия...
    chatArea->setFocusPolicy(Qt::ClickFocus);

    QToolBar* bb = new QToolBar(tr("Button bar"));
    bb->setObjectName("buttonBar");
    QPushButton* refreshButton = new QPushButton(QIcon(":/refresh"),"");
    refreshButton->setFlat(true);
    refreshButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* conferenceButton = new QPushButton(QIcon(":/conference"),"");
    conferenceButton->setFlat(true);
    conferenceButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* configButton = new QPushButton(QIcon(":/config"),"");
    configButton->setFlat(true);
    configButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* aboutButton = new QPushButton(QIcon(":/about"),"");
    aboutButton->setFlat(true);
    aboutButton->setFocusPolicy(Qt::NoFocus);
    bb->addWidget(refreshButton);
    bb->addWidget(conferenceButton);
    bb->addWidget(configButton);
    bb->addWidget(aboutButton);
    addToolBar(Qt::BottomToolBarArea,bb);

    msgLine = new QLineEdit(this);
    msgLine->setPlaceholderText(tr("Input message here"));

    sendButton = new QPushButton(QIcon(":/send"),tr("send"));
    sendButton->setFocusPolicy(Qt::NoFocus);

    QToolBar* tb = new QToolBar(tr("Send toolbar"));
    tb->setObjectName("sendToolBar");
    tb->addWidget(msgLine);
    tb->addWidget(sendButton);
    addToolBar(Qt::BottomToolBarArea,tb);

    QListView* onlineList = new QListView(this);
    onlineList->setUniformItemSizes(true);
    onlineList->setModel(&userList);

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

    QMenu* trayMenu = new QMenu();
    trayMenu->addAction(QIcon(":/chat-white"),"Show qChat",this,SLOT(show()));
    trayMenu->addSeparator();
    QMenu* statusMenu = trayMenu->addMenu(QIcon(statusIcons[status]),"Status");
        statusMenu->addAction(QIcon(":/online"),"Online");
        statusMenu->addAction(QIcon(":/away"),"Away");
        statusMenu->addAction(QIcon(":/busy"),"Busy");
    trayMenu->addAction(QIcon(":/config"),"Config",configDialog,SLOT(show()));
    trayMenu->addSeparator();
    trayMenu->addAction(QIcon(":/close"),"Exit",this,SLOT(exitClick()));

    tray = new QSystemTrayIcon(QIcon(":/chat"),this);
    tray->setContextMenu(trayMenu);
    tray->show();

    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));
    connect(msgLine,SIGNAL(returnPressed()),this,SLOT(sendClick()));
    connect(globalSocket,SIGNAL(readyRead()),this,SLOT(processData()));
    connect(onlinePingTimer,SIGNAL(timeout()),this,SLOT(sendPing()));
    connect(onlineCheckTimer,SIGNAL(timeout()),this,SLOT(onlineCheck()));
    connect(refreshButton,SIGNAL(clicked()),this,SLOT(refreshClick()));
    connect(conferenceButton,SIGNAL(clicked()),this,SLOT(conferenceClick()));
    connect(configButton,SIGNAL(clicked()),this,SLOT(configClick()));
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutClick()));
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClick(QSystemTrayIcon::ActivationReason)));

    onlinePingTimer->start(5000);
    onlineCheckTimer->start(15000);

    insertMessage(tr("<font color='gray'>qChat alpha - %1</font>").arg(QHostInfo::localHostName()));
    setWindowTitle(tr("qChat - %1").arg(nick));
    setWindowFlags(Qt::Window |Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowIcon(QIcon(":/chat"));
    sendOnlineWarning();
    sendWhoRequest();

    QSettings settings("qChat");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

MainWindow::~MainWindow()
{

}
