#pragma once
#ifndef _ISTCPCLIENTS_H_INCLUDED
#define _ISTCPCLIENTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpClient.h"
//-----------------------------------------------------------------------------
class ISTcpClients
{
public:
    static ISTcpClients& Instance();

    void Add(ISTcpClient *TcpClient);
    bool Remove(ISTcpClient *TcpClient);
    ISTcpClientInfo GetInfo(unsigned int UserID);
    bool IsExistSocket(SOCKET Socket);
    bool IsExistUserID(unsigned int UserID);
    size_t GetCount();

private:
    ISTcpClients();
    ~ISTcpClients();
    ISTcpClients(const ISTcpClients&) = delete;
    ISTcpClients(ISTcpClients&&) = delete;
    ISTcpClients& operator=(const ISTcpClients&) = delete;
    ISTcpClients& operator=(ISTcpClients&&) = delete;

private:
    std::vector<ISTcpClient*> Clients;
    size_t Count;
    ISCriticalSection CS;
};
//-----------------------------------------------------------------------------
#endif
