#ifndef QPRIVATE_H
#define QPRIVATE_H

#include <QDialog>
#include <QHash>
#include <QList>

class QLineEdit;
class qUser;

class qPrivate : public QDialog
{
    Q_OBJECT
public:
    explicit qPrivate(QWidget *parent = 0);
    quint64 confID;
    QList<qUser*> confUserList;

signals:

public slots:
    void sendClick();

protected:
    QLineEdit* msgLine;
};

class qPrivateList
{
private:
    QHash<quint64, qPrivate*> confList;

    quint64 generateID();

public:
    qPrivate* getPrivateWindow(quint64 confID);
    qPrivate* getPrivateWindow(const QList<qUser*>& confUserList);

};

#endif // QPRIVATE_H
