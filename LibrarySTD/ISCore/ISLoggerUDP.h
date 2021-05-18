#pragma once
#ifndef _ISLOGGERUDP_H_INCLUDED
#define _ISLOGGERUDP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISLoggerUDP
{
public:
    static ISLoggerUDP& Instance();

    const std::string& GetErrorString() const;
    bool Init();
    void Add(const std::string &String);

private:
    void Worker();
    void Test();

private:
    ISLoggerUDP();
    ~ISLoggerUDP();
    ISLoggerUDP(const ISLoggerUDP&) = delete;
    ISLoggerUDP(ISLoggerUDP&&) = delete;
    ISLoggerUDP& operator=(const ISLoggerUDP&) = delete;
    ISLoggerUDP& operator=(ISLoggerUDP&&) = delete;

private:
    std::string ErrorString;
    ISNamespace::Started Flag;
    ISCriticalSection CS;
    std::deque<std::string> Deque;
    ISSocket Socket;
    std::vector<ISSocketAddr *> Clients;
    std::queue<std::string> Queue;
};
//-----------------------------------------------------------------------------
#endif
