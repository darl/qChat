#include "mainwindow.h"

#include "qTypes.h"
#include "qChat.h"
#include "qUserList.h"
#include "qPrivate.h"
#include "qConfig.h"
#include "mt64.h"
#include "qRsa.h"
#include "qUpdater.h"

/*вставка сообщения в (QTextBrowser chatBrowser)
  на входе
   -сообщение с хтмл тегами
   -флаг добавления текущего времени
   -указатель на класс пользователя-отправителя*/
void MainWindow::insertMessage(const QString& msg, bool insertTime, qUser* userFrom)
{
    QString addMsg = "";
    QString originalMsg = msg;

    if(insertTime)
        addMsg += tr("<font color='%2'>%1</font> ").arg(QTime::currentTime().toString(), "gray" /*timestamp color*/ );

    //если есть отправитель
    if(userFrom)
    {
        //отправитель
        addMsg += tr("<a href='qchat://%1'><b>%2</b></a>: ").arg(userFrom->address.toString()).arg(userFrom->nick);
        //подсветка своего ника
        originalMsg.replace(QRegExp(tr("\\b(%1)\\b").arg(nick), Qt::CaseInsensitive),
                                      "<font style='background-color:yellow'>\\1</font>");
    }
    addMsg += originalMsg;

    bool buttom = (chatBrowser->verticalScrollBar()->value() == chatBrowser->verticalScrollBar()->maximum());

    chatBrowser->append(addMsg);

    //прокрутка вниз
    if(buttom) chatBrowser->verticalScrollBar()->setValue(chatBrowser->verticalScrollBar()->maximum());
}

/*нажатие кнопки отправить*/
void MainWindow::sendClick()
{
    if(msgEdit->text().isEmpty()) return;

    //запоминаем сообщение для быстрого ввода
    if(previusMessages.contains(msgEdit->text()))
        previusMessages.removeAll(msgEdit->text());
    previusMessages.push_back(msgEdit->text());
    currentMessage = previusMessages.end();

    general->sendMessage(msgEdit->text());

    msgEdit->clear();
    msgEdit->setFocus();
}

/*нажатие кнопки обновить*/
void MainWindow::refreshClick()
{
    userList.clearOfflineUsers();
    general->sendWhoRequest();
}

/*нажатие кнопки конференция*/
void MainWindow::conferenceClick()
{
    QList<QModelIndex> selected(userListView->selectionModel()->selectedIndexes());
    QList<qUser*> selectedUserList;
    QModelIndex i;

    //получение списка IP локальной машины
    QList<QHostAddress> localAddresses = QHostInfo::fromName(QHostInfo::localHostName()).addresses();

    foreach(i, selected)
    {
        qUser* u = userList[i.data(Qt::ToolTipRole).toString()];
        if(!u) continue;

        if(localAddresses.contains(u->address)) //выделен локальный пользователь
            continue;

        selectedUserList.append(u);
    }

    if(selectedUserList.isEmpty())
    {
        //сообщить об ошибке
        qDebug() << "Need to select at least one non-local user";
        return;
    }

    qPrivate* wnd = privateList.getPrivateWindow(selectedUserList);
    wnd->show();
}

/*нажатие кнопки настройки*/
void MainWindow::configClick()
{
    configDialog->exec();
}

/*нажатие кнопки о программе*/
void MainWindow::aboutClick()
{
    QMessageBox::about(this, tr("About qChat"),
                 tr("<font size='6'>qChat</font><small><i>alpha</i></small>"
                    "<p>developed by Darl</p>"
                    "<p>qChat is a small server-less chat client</p>"));
}

/*нажатие на ссылку в (QTextBrowser chatBrowser)*/
void MainWindow::linkClick(const QUrl& url)
{
    if(url.scheme() == "qchat") //нажатие на ник автора сообщения
    {
        qUser* user = userList[url.host()];
        if(!user) return;
        msgEdit->setText(tr("%1, %2").arg(user->nick, msgEdit->text()));
        msgEdit->setFocus();
    }
    else if(url.scheme() == "qbot") //нажатие на ссылку 'more...' у многострочного сообщения бота
    {
        chatBrowser->textCursor().deletePreviousChar(); // 'more...' удаляется за один вызов

        bool buttom = (chatBrowser->verticalScrollBar()->value() == chatBrowser->verticalScrollBar()->maximum());
        chatBrowser->textCursor().insertHtml(QByteArray::fromBase64(url.fragment().toAscii().data()));
        //прокрутка вниз
        if(buttom) chatBrowser->verticalScrollBar()->setValue(chatBrowser->verticalScrollBar()->maximum());
    }
}

/*нажатие на иконку в трее*/
void MainWindow::trayClick(QSystemTrayIcon::ActivationReason activationReason)
{
    if(activationReason == QSystemTrayIcon::Trigger) //одиночное нажатие
    {
        if(!isActiveWindow() && isVisible())
        {
            raise();
            activateWindow();
        }
        else
            setVisible(!isVisible());

        if(isVisible())
        {
            raise();
            activateWindow();
        }
    }
    else if(activationReason == QSystemTrayIcon::Context) //правый клик
    {
        QMenu* trayMenu = new QMenu(this);

        trayMenu->addAction(QIcon(":/chat-white"), tr("Show qChat"), this, SLOT(show()));
        trayMenu->addSeparator();

        QMenu* statusMenu = trayMenu->addMenu(statusIcon(status),"Status");
            statusMenu->addAction(QIcon(":/online"), tr("Online"), this, SLOT(setOnlineStatus()));
            statusMenu->addAction(QIcon(":/away"), tr("Away"), this, SLOT(setAwayStatus()));
            statusMenu->addAction(QIcon(":/busy"), tr("Busy"), this, SLOT(setBusyStatus()));
        trayMenu->addAction(QIcon(":/config"), tr("Config"), this, SLOT(configClick()));
        trayMenu->addSeparator();

        trayMenu->addAction(QIcon(":/close"),tr("Exit"),this,SLOT(exitClick()));

        trayMenu->exec(QCursor::pos());
        trayMenu->deleteLater();
    }
}

/*нажатие кнопки выход в контекстном меню иконки трея*/
void MainWindow::exitClick()
{
    saveSettings();

    general->sendOfflineWarning();

    systemTray->hide();
    qApp->quit();
}

/*сигнал о входе пользователя в чат*/
void MainWindow::nowOnline(qUser* u)
{
    insertMessage(tr("<font color='%2'>%1 has come online</font>").arg(u->nick, "gray" /*цвет сообщения*/),true,NULL);
}

/*сигнал о выходе пользователя из чата*/
void MainWindow::nowOffline(qUser* u)
{
    insertMessage(tr("<font color='%2'>%1 has gone offline</font>").arg(u->nick, "gray" /*цвет сообщения*/),true,NULL);
}

/*выбор статуса в меню трея*/
void MainWindow::setOnlineStatus()
{
    status = usOnline;
    general->sendOnlinePing();
}

/*выбор статуса в меню трея*/
void MainWindow::setAwayStatus()
{
    status = usAway;
    general->sendOnlinePing();
}

/*выбор статуса в меню трея*/
void MainWindow::setBusyStatus()
{
    status = usBusy;
    general->sendOnlinePing();
}

/*построение интерфейса*/
void MainWindow::createUI()
{
    chatBrowser = new QTextBrowser(this);
    chatBrowser->setOpenExternalLinks(true);   //ссылки будут открываться внешне, а не будут пытаться открыться внутри виджета
    QDesktopServices::setUrlHandler("qchat",this,"linkClick"); //нажатие на имя пользователя
    QDesktopServices::setUrlHandler("qbot",this,"linkClick");   //нажатие на 'more' в сообщении бота
    chatBrowser->setFocusPolicy(Qt::ClickFocus);
    chatBrowser->document()->setMaximumBlockCount(2000); //максимальное число видимых сообщений

    QToolBar* buttonBar = new QToolBar(tr("Buttons"));
    buttonBar->setObjectName("buttonBar");
    QPushButton* refreshButton = new QPushButton(QIcon(":/refresh"), "");
    refreshButton->setFlat(true);
    refreshButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* conferenceButton = new QPushButton(QIcon(":/conference"), "");
    conferenceButton->setFlat(true);
    conferenceButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* configButton = new QPushButton(QIcon(":/config"), "");
    configButton->setFlat(true);
    configButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* aboutButton = new QPushButton(QIcon(":/about"), "");
    aboutButton->setFlat(true);
    aboutButton->setFocusPolicy(Qt::NoFocus);
    buttonBar->addWidget(refreshButton);
    buttonBar->addWidget(conferenceButton);
    buttonBar->addWidget(configButton);
    buttonBar->addWidget(aboutButton);
    addToolBar(Qt::BottomToolBarArea, buttonBar);

    msgEdit = new QLineEdit(this);
    msgEdit->setPlaceholderText(tr("Input message here"));
    msgEdit->installEventFilter(this);

    sendButton = new QPushButton(QIcon(":/send"),tr("send"));
    sendButton->setFocusPolicy(Qt::NoFocus);

    QToolBar* messageBar = new QToolBar(tr("Message"));
    messageBar->setObjectName("messageBar");
    messageBar->addWidget(msgEdit);
    messageBar->addWidget(sendButton);
    addToolBar(Qt::BottomToolBarArea,messageBar);

    userListView = new QListView(this);
    userListView->setUniformItemSizes(true);
    userListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    userListView->setModel(&userList);

    QDockWidget* userListBar = new QDockWidget(tr("User list"));
    userListBar->setObjectName("userListBar");
    userListBar->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    userListBar->setWidget(userListView);
    addDockWidget(Qt::LeftDockWidgetArea,userListBar);

    setCentralWidget(chatBrowser);

    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));
    connect(msgEdit,SIGNAL(returnPressed()),this,SLOT(sendClick()));
    connect(refreshButton,SIGNAL(clicked()),this,SLOT(refreshClick()));
    connect(conferenceButton,SIGNAL(clicked()),this,SLOT(conferenceClick()));
    connect(configButton,SIGNAL(clicked()),this,SLOT(configClick()));
    connect(aboutButton,SIGNAL(clicked()),this,SLOT(aboutClick()));
}

/*создание иконки в трее*/
void MainWindow::createTray()
{
    systemTray = new QSystemTrayIcon(QIcon(":/chat"),this);
    systemTray->setToolTip(tr("qChat"));
    systemTray->show();

    connect(systemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
}

/*создание таймеров
  для нахождения вышедших из сети пользователей*/
void MainWindow::createTimers()
{
    QTimer* onlinePingTimer = new QTimer(this);
    QTimer* onlineCheckTimer = new QTimer(this);

    connect(onlinePingTimer,SIGNAL(timeout()),general,SLOT(sendOnlinePing()));
    connect(onlineCheckTimer,SIGNAL(timeout()),&userList,SLOT(clearOfflineUsers()));

    onlinePingTimer->start(5000);
    onlineCheckTimer->start(10000);
}

/*загрузка настроек
  вызвется при выходе из программы*/
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

/*сохранение настроек*/
void MainWindow::saveSettings()
{
    QSettings settings("qChat");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("nick",nick);
    settings.setValue("status",status);
    settings.setValue("broadcast",broadcast.toString());
    settings.setValue("port",port);
}

/*создание окна*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createUI();
    loadSettings();
    createTray();

    general = new qGeneralChat();
    connect(general,SIGNAL(insertMessage(QString,bool,qUser*)),this,SLOT(insertMessage(QString,bool,qUser*)));
    privateServer = new qPrivateServer(this);

    createTimers();

    connect(&userList,SIGNAL(nowOnline(qUser*)),this,SLOT(nowOnline(qUser*)));
    connect(&userList,SIGNAL(nowOffline(qUser*)),this,SLOT(nowOffline(qUser*)));

    insertMessage(tr("<font color='gray'>qChat - %1 (%2)</font>").arg(qUpdater::versionStr(), qUpdater::systemStr()));

    setWindowTitle(tr("qChat"));
    setWindowFlags(Qt::Window |Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowIcon(QIcon(":/chat"));

    general->sendOnlineWarning();

    //инициализация генератора псевдослучайных чисел числом миллисекунд с 1970 года
    init_genrand64(QDateTime::currentDateTime().currentMSecsSinceEpoch());
}

/*обработка нажатий вверх/вниз для поля ввода сообщения*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == msgEdit)
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            switch(keyEvent->key())
            {
            case Qt::Key_Up:
                if(previusMessages.isEmpty())
                    return true;
                if(currentMessage != previusMessages.begin())
                    currentMessage--;
                else
                    currentMessage = previusMessages.end();

                if(currentMessage != previusMessages.end())
                {
                    msgEdit->setText(*currentMessage);
                    msgEdit->selectAll();
                }
                else
                    msgEdit->clear();
                return true;
            case Qt::Key_Down:
                if(previusMessages.isEmpty())
                    return true;
                if(currentMessage != previusMessages.end())
                    currentMessage++;
                else
                    currentMessage = previusMessages.begin();

                if(currentMessage != previusMessages.end())
                {
                    msgEdit->setText(*currentMessage);
                    msgEdit->selectAll();
                }
                else
                    msgEdit->clear();
                return true;
            default:
                return QObject::eventFilter(obj, event);
            }

        }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
