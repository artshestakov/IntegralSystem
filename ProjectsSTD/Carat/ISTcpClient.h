#pragma once
#ifndef _ISTCPCLIENT_H_INCLUDED
#define _ISTCPCLIENT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpClient
{
public:
    ISTcpClient();
    ~ISTcpClient();

    void AddData(const char *Data, int Size);
    void GetBuffer(std::vector<char> &Vector);

public:
    SOCKET Socket;
    std::string IPAddress;
    ISTcpPort Port;
    bool Authorized;

private:
    std::vector<std::vector<char>> Buffer;
};
//-----------------------------------------------------------------------------
#endif
