#include "qPrivate.h"


qPrivate::qPrivate(QWidget *parent) :
    QDialog(parent)
{
    setMinimumWidth(250);
    setMinimumHeight(300);

    QPushButton* addConferenceButton = new QPushButton(QIcon(":/addusertoconference"),"Add user",this);
    addConferenceButton->setFlat(true);
    addConferenceButton->setFocusPolicy(Qt::NoFocus);
    QDialogButtonBox* bb = new QDialogButtonBox(this);
    bb->addButton(addConferenceButton,QDialogButtonBox::ActionRole);

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
    vl->addWidget(bb);
    vl->addWidget(userList,0);
    vl->addWidget(chatArea,1);
    vl->addWidget(tb);
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);
    //
    chatArea->insertHtml("blablabla<br>hslfjsdf<br><b>hhd</b>");
}
