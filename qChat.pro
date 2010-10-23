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
        qUserList.cpp \
        qConfig.cpp \
        qPrivate.cpp \
    qUser.cpp

HEADERS += mainwindow.h \
        qChat.h \
        qUserList.h \
        qConfig.h \
        qTypes.h \
        qPrivate.h \
    qUser.h

RESOURCES += \
        res.qrc


FORMS += \
        qconfig.ui
