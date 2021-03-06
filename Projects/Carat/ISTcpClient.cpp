#include "ISTcpClient.h"
//-----------------------------------------------------------------------------
ISTcpClient::ISTcpClient()
    : Socket(0),
    Port(0),
    DTConnected(ISDateTime::CurrentDateTime()),
    Authorized(false),
    UserID(0),
    UserGroupID(0),
    UserSystem(false),
    UserLogin("unknown")
{

}
//-----------------------------------------------------------------------------
ISTcpClient::~ISTcpClient()
{

}
//-----------------------------------------------------------------------------
void ISTcpClient::AddData(const char *Data, int Size)
{
    Buffer.emplace_back(std::vector<char>(Data, Data + Size));
}
//-----------------------------------------------------------------------------
void ISTcpClient::GetBuffer(std::vector<char> &Vector)
{
    for (size_t i = 0, c = Buffer.size(); i < c; ++i)
    {
        Vector.insert(Vector.end(), Buffer[i].begin(), Buffer[i].end());
    }
    Buffer.clear();
}
//-----------------------------------------------------------------------------
