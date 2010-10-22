#include "qPrivate.h"

#include <QtGui>
#include "qUserList.h"

void qPrivate::sendClick()
{
    qDebug() << "send";
    qUser* u = userList["172.18.0.184"];
    if(!u) return;
    u->directConnect();
    u->sendMessage(12345678,tr("test123"));
}

qPrivate::qPrivate(QWidget *parent) :
    QDialog(parent)
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


    QListView* userList = new QListView(this);
    userList->setMinimumHeight(100);
    userList->hide();
    userList->setFrameShape(QFrame::Panel);

    QTextBrowser* chatArea = new QTextBrowser(this);

    QSplitter* spl = new QSplitter(Qt::Vertical);
    spl->addWidget(userList);
    spl->addWidget(chatArea);
    spl->setStretchFactor(0,0);
    spl->setStretchFactor(1,1);
    spl->setHandleWidth(1);
    spl->setChildrenCollapsible(false);
    QList<int> szs;
    szs<<100<<300;
    spl->setSizes(szs);

    QLineEdit* msgLine = new QLineEdit(this);
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

    connect(userListToggle,SIGNAL(toggled(bool)),userList,SLOT(setVisible(bool)));
    connect(sendButton,SIGNAL(clicked()),this,SLOT(sendClick()));

    setWindowIcon(QIcon(":/conference"));
    setWindowTitle("Conference 0");
}
