#ifndef QCONFIG_H
#define QCONFIG_H

#include <QDialog>

namespace Ui {
    class qconfig;
}

class qConfig : public QDialog
{
    Q_OBJECT

public:
    explicit qConfig(QWidget *parent = 0);
    ~qConfig();

private:
    Ui::qconfig *ui;
};

#endif // QCONFIG_H
