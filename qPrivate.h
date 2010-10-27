#ifndef QPRIVATE_H
#define QPRIVATE_H

#include <QDialog>
#include <QHash>
#include <QList>

class QLineEdit;
class qUser;
class QListWidget;
class QTextBrowser;

class qPrivate : public QDialog
{
    Q_OBJECT
public:
    friend class qUser;
    explicit qPrivate(QWidget *parent = 0);
    qPrivate(quint64 id, const QList<qUser*>& l);
    quint64 confID;
    QList<qUser*> confUserList;

signals:

public slots:
    void sendClick();

protected:
    QLineEdit* msgLine;
    QListWidget* users;
    QTextBrowser* chatArea;

    void initUI();
    void insertMessage(const QString& msg, bool insertTime = false, qUser* user = 0);

    void closeEvent(QCloseEvent *event);
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
                                                    //вызвавший должен сам заполнить список
    qPrivate* getPrivateWindow(const QList<qUser*>& confUserList);  //возвращает указатель на окно или создает новое
                                                                    //вызывается из главного окна
};

extern qPrivateList privateList;

#endif // QPRIVATE_H
