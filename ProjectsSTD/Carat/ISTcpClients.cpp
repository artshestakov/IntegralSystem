#include "ISTcpClients.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpClients::ISTcpClients()
    : Count(0)
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISTcpClients::~ISTcpClients()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISTcpClients& ISTcpClients::Instance()
{
    static ISTcpClients TcpClients;
    return TcpClients;
}
//-----------------------------------------------------------------------------
void ISTcpClients::Add(ISTcpClient *TcpClient)
{
    CRITICAL_SECTION_LOCK(&CS);
    Clients.emplace_back(TcpClient);
    ++Count;
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
bool ISTcpClients::Remove(ISTcpClient *TcpClient)
{
    bool Result = false;
    CRITICAL_SECTION_LOCK(&CS);
    for (size_t i = 0; i < Count; ++i)
    {
        if (Clients[i] == TcpClient)
        {
            Clients.erase(Clients.begin() + i);
            delete TcpClient;
            TcpClient = nullptr;
            Result = true;
            --Count;
            break;
        }
    }
    CRITICAL_SECTION_UNLOCK(&CS);
    return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpClients::IsExist(SOCKET Socket)
{
    bool Result = false;
    CRITICAL_SECTION_LOCK(&CS);
    for (size_t i = 0; i < Count; ++i)
    {
        if (Clients[i]->Socket == Socket)
        {
            Result = true;
            break;
        }
    }
    CRITICAL_SECTION_UNLOCK(&CS);
    return Result;
}
//-----------------------------------------------------------------------------
size_t ISTcpClients::GetCount()
{
    size_t Result = 0;
    CRITICAL_SECTION_LOCK(&CS);
    Result = Count;
    CRITICAL_SECTION_UNLOCK(&CS);
    return Count;
}
//-----------------------------------------------------------------------------
