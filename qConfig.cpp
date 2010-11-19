#include "qConfig.h"
#include "ui_qConfig.h"


#include "qChat.h"

qConfig::qConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qconfig)
{
    ui->setupUi(this);

    connect(this,SIGNAL(accepted()),this,SLOT(apply()));

    adjustSize();
    ui->verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}

qConfig::~qConfig()
{
    delete ui;
}

void qConfig::showEvent ( QShowEvent * event )
{
    ui->addressEdit->setText(broadcast.toString());
    ui->portEdit->setText(tr("").setNum(port));
    ui->nickEditor->setText(nick);
    ui->statusEdit->setCurrentIndex(static_cast<int>(status));
    ui->invisibleModeEdit->setChecked(invisibleMode);
    ui->htmlTagsEdit->setChecked(htmlTags);
    ui->offlineUsersEdit->setChecked(showOfflineUsers);
    QDialog::showEvent(event);
}

void qConfig::apply()
{
    broadcast.setAddress(ui->addressEdit->text());
    if(!ui->portEdit->text().isEmpty())
        port = ui->portEdit->text().toInt();
    if(!ui->nickEditor->text().isEmpty())
        nick = ui->nickEditor->text();
    if(ui->statusEdit->currentIndex()>-1)
        status = static_cast<userStatus>(ui->statusEdit->currentIndex());
    invisibleMode = ui->invisibleModeEdit->isChecked();
    htmlTags = ui->htmlTagsEdit->isChecked();
    showOfflineUsers = ui->offlineUsersEdit->isChecked();
}

quint16 port = 49675;                                   //default port
QHostAddress broadcast("172.18.255.255");               //broadcasting mask

QString nick = QHostInfo::localHostName();              //default Nickname
userStatus status = usOnline;

bool invisibleMode = false;
bool htmlTags = false;
bool showOfflineUsers = false;

qConfig* configDialog;
