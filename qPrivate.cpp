#include "qPrivate.h"

#include "mt64.h"
#include <QtGui>
#include "qUserList.h"
#include "qConfig.h"

void qPrivate::sendClick()
{
    if(msgLine->text().isEmpty()) return;

    qUser* u;
    bool notConnectedWarn = false;
    foreach(u,confUserList)
    {
        if(!u->directConnect())
            notConnectedWarn = true;
        u->sendMessage(confID,msgLine->text());
    }

    if(notConnectedWarn)
        insertMessage("<font color='gray'>Probably this message hasn't been delivered:</font>");
    insertMessage(msgLine->text(),true,qUser::local());


    msgLine->clear();
    msgLine->setFocus();
}

void qPrivate::insertMessage(const QString& msg, bool insertTime, qUser* user)
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

void qPrivate::initUI()
{
    setMinimumWidth(250);
    setMinimumHeight(300);

    QPushButton* addConferenceButton = new QPushButton(QIcon(":/addusertoconference"),"Add user");
    addConferenceButton->setFlat(true);
    addConferenceButton->setFocusPolicy(Qt::NoFocus);
    QPushButton* userListToggle = new QPushButton(QIcon(":/conference"),"Conference user list");
    userListToggle->setFlat(true);
    userListToggle->setFocusPolicy(Qt::NoFocus);
    userListToggle->setCheckable(true);

    QHBoxLayout* hl = new QHBoxLayout();
    hl->addWidget(userListToggle,0,Qt::AlignLeft);
    hl->addWidget(addConferenceButton,0,Qt::AlignRight);

    users = new QListWidget(this);
    users->setMinimumHeight(100);
    users->hide();
    users->setFrameShape(QFrame::Panel);

    chatArea = new QTextBrowser(this);

    QSplitter* spl = new QSplitter(Qt::Vertical);
    spl->addWidget(users);
    spl->addWidget(chatArea);
    spl->setStretchFactor(0,0);
    spl->setStretchFactor(1,1);
    spl->setHandleWidth(1);
    spl->setChildrenCollapsible(false);
    QList<int> szs;
    szs<<100<<300;
    spl->setSizes(szs);

    msgLine = new QLineEdit(this);
    msgLine->setPlaceholderText(tr("Input message here"));

    QPushButton* sendButton = new QPushButton(QIcon(":/send"),tr("send"));

    QHBoxLayout* tb = new QHBoxLayout();
    tb->addWidget(msgLine);
    tb->addWidget(sendButton);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->addLayout(hl);
    vl->addWidget(spl,1);
    vl->addLayout(tb);
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);

    connect(userListToggle,SIGNAL(toggled(bool)),users,SLOT(setVisible(bool)));
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));

    setWindowIcon(QIcon(":/conference"));
    setWindowTitle("Conference with");
}

qPrivate::qPrivate(QWidget *parent) :
    QDialog(parent)
{
    initUI();
}

qPrivate::qPrivate(quint64 id, const QList<qUser*>& l) : confID(id), confUserList(l)
{
    initUI();

    /////////////////
    qUser* u;
    foreach(u,l)
    {
        new QListWidgetItem(statusIcons(u->status),u->nick,users);
        u->directConnect(); //заранее подключаемся
        //u->sendConfInfo(); //отправляем информацию о конференции
        setWindowTitle(windowTitle()+" "+u->nick);
    }
}

void qPrivate::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

quint64 qPrivateList::generateID()
{
      return genrand64_int64();
}

bool qPrivateList::privateWindowExist(quint64 confID)
{
    return confList.contains(confID);
}

qPrivate* qPrivateList::getPrivateWindow(quint64 confID)
{
    if(confList.contains(confID))
        return confList.value(confID);
    else
    {
        qPrivate* w = new qPrivate(confID, QList<qUser*>());
        confList.insert(confID,w);
        return w;
    }
}

qPrivate* qPrivateList::getPrivateWindow(const QList<qUser*>& confUserList)
{
    for(QHash<quint64, qPrivate*>::iterator i = confList.begin(); i != confList.end(); i++)
    {
        qPrivate* w = i.value();
        bool ok = true;
        qUser* u;
        foreach(u, confUserList)
        {
            if(!w->confUserList.contains(u))
            {
                ok = false;
                break;
            }
        }
        if(ok)
            return w;
    }
    quint64 confID = generateID();
    qPrivate* w = new qPrivate(confID, confUserList);
    confList.insert(confID,w);
    return w;
}

qPrivateList privateList;
