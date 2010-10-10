#ifndef QCONFIG_H
#define QCONFIG_H

#include <QtGlobal>
#include <QDialog>

#include "qTypes.h"

class QHostAddress;

extern QHostAddress broadcast;
extern quint16 port;

extern QString nick;
extern userStatus status;

extern bool invisibleMode;
extern bool htmlTags;
extern bool showOfflineUsers;

namespace Ui {
    class qconfig;
}

class qConfig : public QDialog
{
    Q_OBJECT

protected:
    void showEvent ( QShowEvent * event );

public:
    explicit qConfig(QWidget *parent = 0);
    ~qConfig();

private:
    Ui::qconfig *ui;

private slots:
    void apply();
};

extern qConfig* configDialog;

#endif // QCONFIG_H
