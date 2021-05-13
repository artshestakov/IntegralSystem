#pragma once
#ifndef _ISTCPCLIENT_H_INCLUDED
#define _ISTCPCLIENT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "ISDateTime.h"
//-----------------------------------------------------------------------------
class ISTcpClient
{
public:
    ISTcpClient();
    ~ISTcpClient();

    void AddData(const char *Data, int Size);
    void GetBuffer(std::vector<char> &Vector);

public:
	ISSocket Socket;
    std::string IPAddress;
    ISTcpPort Port;
    ISDateTime DTConnected; //Дата и время подключения
    bool Authorized;
    unsigned int UserID;
    unsigned int UserGroupID;
    bool UserSystem;
    std::string UserLogin;

private:
    std::vector<std::vector<char>> Buffer;
};
//-----------------------------------------------------------------------------
#endif
