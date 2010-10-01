#-------------------------------------------------
#
# Project created by QtCreator 2010-09-22T18:13:18
#
#-------------------------------------------------

QT       += core gui network

TARGET = qChat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qChat.cpp \
        qUserList.cpp

HEADERS += mainwindow.h \
        qChat.h \
        qUserList.h

RESOURCES += \
    res.qrc

win32 {
	RC_FILE = ico.rc
}
