#include "qconfig.h"
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
