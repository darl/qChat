#ifndef QPRIVATE_H
#define QPRIVATE_H

#include "qchat_pch.h"
#include "qChat.h"
#include "qUser.h"

class qPrivate : public QDialog
{
    Q_OBJECT

    friend class qUser;

public:
    qPrivate(quint64 id, const QList<qUser*>& l);

protected:
    QLineEdit* msgLine;
    QListWidget* users;
    QTextBrowser* chatArea;

    QList<qUser*> confUserList;
    quint64 confID;

    void initUI();
    void insertMessage(const QString& msg, bool insertTime = false, qUser* user = 0);

    void closeEvent(QCloseEvent *event);

private slots:
    void sendClick();
};

class qPrivateList
{
public:
    //проверка на наличие
    bool privateWindowExist(quint64 confID);

    //возвращает указатель на окно или создает новое
    //вызывается, когда по сети пришло сообщение.
    //если окно создано, то оно готово для добавления сообщения
    //вызвавший должен сам заполнить список
    qPrivate* getPrivateWindow(quint64 confID);

    //возвращает указатель на окно или создает новое
    //принимает список пользователей
    //вызывается из главного окна
    qPrivate* getPrivateWindow(const QList<qUser*>& confUserList);

private:
    QHash<quint64, qPrivate*> confList;

    //генерация псевдослучайного числа(quint64)
    quint64 generateID();
};

extern qPrivateList privateList;

#endif // QPRIVATE_H
