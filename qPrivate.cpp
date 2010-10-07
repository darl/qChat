#include "qPrivate.h"


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


//    QIrExpander* ie = new QIrExpander();
    QListView* userList = new QListView(this);
    userList->setMinimumHeight(100);
    userList->setMaximumHeight(100);

    QTextBrowser* chatArea = new QTextBrowser(this);

    QLineEdit* msgLine = new QLineEdit(this);
    msgLine->setPlaceholderText(tr("Input message here"));

    QPushButton* sendButton = new QPushButton(tr("send"));

    QToolBar* tb = new QToolBar(tr("Send toolbar"));
    tb->setObjectName("sendToolBar");
    tb->addWidget(msgLine);
    tb->addWidget(sendButton);
//    addToolBar(Qt::BottomToolBarArea,tb);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->addLayout(hl);
    vl->addWidget(userList,0);
    vl->addWidget(chatArea,1);
    vl->addWidget(tb);
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);

    connect(userListToggle,SIGNAL(toggled(bool)),userList,SLOT(setHidden(bool)));
    userListToggle->setChecked(true);

    chatArea->insertHtml("blablabla<br>hslfjsdf<br><b>hhd</b>");
}
