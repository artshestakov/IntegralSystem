#pragma once
#ifndef _ISTCPCLIENT_H_INCLUDED
#define _ISTCPCLIENT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "ISDateTime.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISTcpClient
{
public:
    ISTcpClient();
    ~ISTcpClient();

    void AddData(const char *Data, int Size);
    void GetBuffer(std::vector<char> &Vector);

    ISTcpClientInfo GetInfo() const;

public:
    SOCKET Socket;
    std::string IPAddress;
    ISTcpPort Port;
    ISDateTime DTConnected; //Дата и время подключения
    bool Authorized;
    unsigned int UserID;
    bool UserSystem;

private:
    std::vector<std::vector<char>> Buffer;
};
//-----------------------------------------------------------------------------
#endif
