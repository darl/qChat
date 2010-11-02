#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include "qConfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("cp1251"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("cp1251"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("cp1251"));

    MainWindow w;
    configDialog = new qConfig(&w);
    w.show();

    return a.exec();
}
