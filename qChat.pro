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
        qUserList.cpp \
        qConfig.cpp \
        qPrivate.cpp \
        qUser.cpp \
        qChat.cpp \
        mt64.cpp

HEADERS += mainwindow.h \
        qChat.h \
        qUserList.h \
        qConfig.h \
        qTypes.h \
        qPrivate.h \
        qUser.h \
        mt64.h

RESOURCES += \
        res.qrc


FORMS += \
        qconfig.ui
