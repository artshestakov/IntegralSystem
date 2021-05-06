#include "ISTcpConnector.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
    : ErrorString(STRING_NO_ERROR),
    Socket(0)
{

}
//-----------------------------------------------------------------------------
ISTcpConnector::~ISTcpConnector()
{

}
//-----------------------------------------------------------------------------
ISTcpConnector& ISTcpConnector::Instance()
{
    static ISTcpConnector TcpConnector;
    return TcpConnector;
}
//-----------------------------------------------------------------------------
const std::string& ISTcpConnector::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::IsConnected() const
{
    return Socket != 0;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const std::string &Host, unsigned short Port)
{
    if (IsConnected()) //Если подключение поднято - возвращаем true
    {
        return true;
    }

#ifdef WIN32
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
#endif

    ISSocketAddr SocketAddres;
    SocketAddres.sin_family = AF_INET;
    SocketAddres.sin_port = htons(Port);
    int Result = inet_pton(SocketAddres.sin_family, Host.c_str(), &(SocketAddres.sin_addr));
    switch (Result)
    {
    case 0:
        ErrorString = ISAlgorithm::StringF("Invalid host \"%s\"", Host.c_str());
        return false;
        break;

    case -1:
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
        break;
    }

    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == SOCKET_ERROR)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }

    if (connect(Socket, (SOCKADDR *)&SocketAddres, sizeof(SocketAddres)) == SOCKET_ERROR)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
    if (IsConnected()) //Если подключение активно - отключаемся
    {
        //Пытаемся закрыть сокет
#ifdef WIN32
        int Result = closesocket(Socket);
#else
        int Result = close(Socket);
#endif
        if (Result != SOCKET_ERROR)
        {
            Socket = 0;
        }
    }
}
//-----------------------------------------------------------------------------
