#include "qUpdater.h"

#define QCHATVERSION_STR "0.1.0  alpha"

#ifdef Q_OS_WIN32
    #define QCHATSYSTEM 0x00
#elif defined(Q_OS_LINUX)
    #define QCHATSYSTEM 0x01
#else
    #define QCHATSYSTEM 0xFF
#endif

#define QCHATVERSION    0x000100

QString qUpdater::versionStr()
{
    return QCHATVERSION_STR;
}

quint32 qUpdater::version()
{
    return QCHATVERSION;
}

quint32 qUpdater::versionFull()
{
    return (QCHATSYSTEM << 24) | (QCHATVERSION & 0xFFFFFF);
}

QString qUpdater::systemStr()
{
    switch(QCHATSYSTEM)
    {
    case 0x00:
        return "Windows";
    case 0x01:
        return "Linux";
    default:
        return "unknown system";
    }
}

quint32 qUpdater::system()
{
    return QCHATSYSTEM;
}

qUpdater::qUpdater()
{
}
