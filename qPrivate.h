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
    qPrivate(quint64 id, QList<qUser*> l);
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
    bool privateWindowExist(quint64 confID);        //проверка на наличие
    qPrivate* getPrivateWindow(quint64 confID);     //возвращает указатель на окно или создает новое
                                                    //вызывается, когда по сети пришло сообщение.
                                                    //если окно создано, то оно готово для добавления сообщения
    qPrivate* getPrivateWindow(const QList<qUser*>& confUserList);  //возвращает указатель на окно или создает новое
                                                                    //вызывается из главного окна
};

#endif // QPRIVATE_H
