#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("cp1251"));
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("cp1251"));
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("cp1251"));
    MainWindow w;
    w.show();

    return a.exec();
}
