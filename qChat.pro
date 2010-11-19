#-------------------------------------------------
#
# Project created by QtCreator 2010-09-22T18:13:18
#
#-------------------------------------------------

QT       += core gui network

TARGET = qChat
TEMPLATE = app
CONFIG += precompile_header

SOURCES += main.cpp\
        mainwindow.cpp \
        qUserList.cpp \
        qConfig.cpp \
        qPrivate.cpp \
        qUser.cpp \
        qChat.cpp \
        mt64.cpp \
        qRsa.cpp \
    qUpdater.cpp

HEADERS += mainwindow.h \
        qChat.h \
        qUserList.h \
        qConfig.h \
        qTypes.h \
        qPrivate.h \
        qUser.h \
        mt64.h \
        qchat_pch.h \
        qRsa.h \
    qUpdater.h

RESOURCES += \
        res.qrc

PRECOMPILED_HEADER = qchat_pch.h

FORMS += \
        qconfig.ui

LIBS += -lgmp
