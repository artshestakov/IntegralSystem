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
    ISCaratApplication();
    ~ISCaratApplication();

    const std::string& GetErrorString() const;

    bool Init(); //Инициализация
    int Start(); //Запуск службы

    void ShowVersion();
    void Shutdown();
    void ConfigCreate();

private:
    bool CheckRunning(bool &AlreadyRunning);
    void ShutdownController();

private:
    std::string ErrorString;
    bool IsRunning;
    ISCriticalSection CriticalSection;
    std::string FileShutdown;
    bool TCPServer;
};
//-----------------------------------------------------------------------------
#endif
