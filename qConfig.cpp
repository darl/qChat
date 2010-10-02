#include "qConfig.h"
#include "ui_qconfig.h"

qConfig::qConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qconfig)
{
    ui->setupUi(this);
}

qConfig::~qConfig()
{
    delete ui;
}

quint16 port = 49675;                                   //default port
QHostAddress broadcast(QObject::tr("172.18.255.255"));  //broadcasting mask

QString nick = QHostInfo::localHostName();                     //default Nickname
userStatus status = usOnline;

bool invisibleMode = false;
bool htmlTags = false;
bool showOfflineUsers = false;
