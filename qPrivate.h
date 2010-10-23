#ifndef QPRIVATE_H
#define QPRIVATE_H

#include <QDialog>

class QLineEdit;

class qPrivate : public QDialog
{
    Q_OBJECT
public:
    explicit qPrivate(QWidget *parent = 0);

signals:

public slots:
    void sendClick();

protected:
    QLineEdit* msgLine;
};

#endif // QPRIVATE_H
