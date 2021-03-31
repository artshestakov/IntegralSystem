#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCaratApplication
{
public:
    ISCaratApplication(int argc, char **argv);
    ~ISCaratApplication();

    std::string GetErrorString() const;

    bool Init();
    int Start();

private:
    void ShutdownController();
    void Shutdown();
    void Help();
    void Version();

private:
    std::string ErrorString;
    bool IsRunning;
    ISCriticalSection CriticalSection;
    ISVectorString Arguments;
    std::string FileShutdown;
    bool TCPServer;
};
//-----------------------------------------------------------------------------
#endif
