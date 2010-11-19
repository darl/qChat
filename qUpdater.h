#ifndef QUPDATER_H
#define QUPDATER_H

#include "qchat_pch.h"

class qUpdater
{
public:
    qUpdater();

    static QString versionStr();
    static quint32 version();
    static quint32 versionFull();
    static quint32 system();
    static QString systemStr();
};

#endif // QUPDATER_H
