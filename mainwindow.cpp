#include "mainwindow.h"

#include <QtGui>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostInfo>

#include "qTypes.h"
#include "qChat.h"
#include "qUserList.h"
#include "qPrivate.h"
#include "qConfig.h"
#include "mt64.h"

void MainWindow::insertMessage(const QString& msg, bool insertTime, qUser* user)
{
    QString addMsg;
    QString originalMsg = msg;

    if(chatArea->toPlainText().size()!=0)
        addMsg+=tr("<br>");

    if(insertTime)
        addMsg+=tr("<font color='gray'>%1</font> ").arg(QTime::currentTime().toString());

    if(user)
    {
        //подсветка своего ника
        originalMsg.replace(QRegExp(tr("\\b(%1)\\b").arg(nick),Qt::CaseInsensitive),"<font style='background-color:yellow'>\\1</font>");
        //отправитель
        addMsg+=tr("<a href='qchat://%1'><b>%2</b></a>: ").arg(user->address.toString()).arg(user->nick);
    }

    addMsg+=originalMsg;

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
        general->sendMessage(msgLine->text());

    msgLine->clear();
    msgLine->setFocus();
}

void MainWindow::refreshClick()
{
    userList.clearOfflineUsers();
    general->sendWhoRequest();
}

void MainWindow::conferenceClick()
{
    QList<QModelIndex> mil(onlineList->selectionModel()->selectedIndexes());
    QList<qUser*> ul;
    QList<QModelIndex>::iterator i;
    QList<QHostAddress> la = QHostInfo::fromName(QHostInfo::localHostName()).addresses();
    for(i = mil.begin();i!=mil.end();i++)
    {
        qUser* u = userList[i->data(Qt::ToolTipRole).toString()];
        if(!u) continue;    //на всякий случай

        if(la.contains(u->address)) //выделен локальный пользователь
            continue;

        ul.append(u);
    }

    if(ul.isEmpty())
    {
        //сообщить об ошибке
        QMessageBox::warning(this,"Conference","Need to select at least one non local person");
        return;
    }
    qPrivate* wnd = privateList.getPrivateWindow(ul);
    wnd->show();
}

void MainWindow::configClick()
{
    configDialog->show();
}

void MainWindow::aboutClick()
{
    QMessageBox::about(this, tr("About qChat"),
                 tr("<h2>qChat</h2>"
                    "<p>developed by Darl<p>"
                    "<p>qChat is a small server-less chat client</p>"));
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
    saveSettings();

    general->sendOfflineWarning();
    tray->hide();
    delete configDialog;
    qApp->quit();
}

void MainWindow::nowOnline(qUser* u)
{
    insertMessage(tr("<font color='gray'>%1 has come online</font>").arg(u->nick),true,NULL);
}

void MainWindow::nowOffline(qUser* u)
{
    insertMessage(tr("<font color='gray'>%1 has gone offline</font>").arg(u->nick),true,NULL);
}

void MainWindow::createUI()
{
    chatArea = new QTextBrowser(this);
    chatArea->setOpenExternalLinks(true);   //ссылки будут открываться внешне, а не будут пытаться открыться внутри виджета
    QDesktopServices::setUrlHandler("qchat",this,"linkClick"); //это какая-то магия...
    chatArea->setFocusPolicy(Qt::ClickFocus);

    QToolBar* bb = new QToolBar(tr("Button bar"));
    bb->setObjectName("buttonBar");
    QToolButton* refreshButton = new QToolButton();
    refreshButton->setIcon(QIcon(":/refresh"));
    refreshButton->setFocusPolicy(Qt::NoFocus);
    QToolButton* conferenceButton = new QToolButton();
    conferenceButton->setIcon(QIcon(":/conference"));
    conferenceButton->setFocusPolicy(Qt::NoFocus);
    QToolButton* configButton = new QToolButton();
    configButton->setIcon(QIcon(":/config"));
    configButton->setFocusPolicy(Qt::NoFocus);
    QToolButton* aboutButton = new QToolButton();
    aboutButton->setIcon(QIcon(":/about"));
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

    onlineList = new QListView(this);
    onlineList->setUniformItemSizes(true);
    onlineList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    onlineList->setModel(&userList);

    QDockWidget* ul = new QDockWidget(tr("User list"));
    ul->setObjectName("userListBar");
    ul->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    ul->setWidget(onlineList);
    addDockWidget(Qt::LeftDockWidgetArea,ul);

    setCentralWidget(chatArea);

    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));
    connect(msgLine,SIGNAL(returnPressed()),this,SLOT(sendClick()));
    connect(refreshButton,SIGNAL(clicked()),this,SLOT(refreshClick()));
    connect(conferenceButton,SIGNAL(clicked()),this,SLOT(conferenceClick()));
    connect(configButton,SIGNAL(clicked()),this,SLOT(configClick()));
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutClick()));
}

void MainWindow::createTray()
{
    QMenu* trayMenu = new QMenu();
    trayMenu->addAction(QIcon(":/chat-white"),"Show qChat",this,SLOT(show()));
    trayMenu->addSeparator();
    QMenu* statusMenu = trayMenu->addMenu(statusIcons(status),"Status");
        statusMenu->addAction(QIcon(":/online"),"Online");
        statusMenu->addAction(QIcon(":/away"),"Away");
        statusMenu->addAction(QIcon(":/busy"),"Busy");
    trayMenu->addAction(QIcon(":/config"),"Config",this,SLOT(configClick()));
    trayMenu->addSeparator();
    trayMenu->addAction(QIcon(":/close"),"Exit",this,SLOT(exitClick()));

    tray = new QSystemTrayIcon(QIcon(":/chat"),this);
    tray->setContextMenu(trayMenu);
    tray->show();

    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClick(QSystemTrayIcon::ActivationReason)));

}

void MainWindow::createTimers()
{
    QTimer* onlinePingTimer = new QTimer(this);
    QTimer* onlineCheckTimer = new QTimer(this);

    connect(onlinePingTimer,SIGNAL(timeout()),general,SLOT(sendOnlinePing()));
    connect(onlineCheckTimer,SIGNAL(timeout()),&userList,SLOT(clearOfflineUsers()));

    onlinePingTimer->start(5000);
    onlineCheckTimer->start(10000);
}

void MainWindow::loadSettings()
{
    QSettings settings("qChat");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    nick = settings.value("nick",QHostInfo::localHostName()).toString();
    status = static_cast<userStatus>(settings.value("status",usOnline).toInt());
    broadcast.setAddress(settings.value("broadcast","172.18.255.255").toString());
    port = settings.value("port",49675).toInt();
}

void MainWindow::saveSettings()
{
    //сохранение настроек
    QSettings settings("qChat");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("nick",nick);
    settings.setValue("status",status);
    settings.setValue("broadcast",broadcast.toString());
    settings.setValue("port",port);
    //тестовые функции не сохраняются
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createUI();


    general = new qGeneralChat();
    connect(general,SIGNAL(insertMessage(QString,bool,qUser*)),this,SLOT(insertMessage(QString,bool,qUser*)));


    loadSettings();

    createTimers();

    createTray();

    connect(&userList,SIGNAL(nowOnline(qUser*)),this,SLOT(nowOnline(qUser*)));
    connect(&userList,SIGNAL(nowOffline(qUser*)),this,SLOT(nowOffline(qUser*)));

    ps = new qPrivateServer(this);

    //вставка сообщения "qChat alpha - %hostname%"
    insertMessage(tr("<font color='gray'>qChat alpha - %1</font>").arg(QHostInfo::localHostName()));

    setWindowTitle(tr("qChat - general"));
    setWindowFlags(Qt::Window |Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowIcon(QIcon(":/chat"));

    general->sendOnlineWarning();
    general->sendWhoRequest();

    //инициализация генератора числом миллисекунд с 1970 года
    init_genrand64(QDateTime::currentDateTime().currentMSecsSinceEpoch());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

MainWindow::~MainWindow()
{

}
