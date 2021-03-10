#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer()
	: ErrorString(STRING_NO_ERROR),
	Socket(0)
{

}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{

}
//-----------------------------------------------------------------------------
ISTcpServer& ISTcpServer::Instance()
{
	static ISTcpServer TcpServer;
	return TcpServer;
}
//-----------------------------------------------------------------------------
std::string ISTcpServer::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISTcpServer::Start()
{
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	SOCKADDR_IN SocketAddress;
	SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY; //Любой IP адресс
	SocketAddress.sin_port = htons(50000); //Задаём порт
	SocketAddress.sin_family = AF_INET; //AF_INET - Cемейство адресов для IPv4

	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	if (bind(Socket, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	if (listen(Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	std::thread(&ISTcpServer::WorkerAcceptor, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerAcceptor()
{
	while (true)
	{
		SOCKADDR_IN ClientAddress = { 0 };
		int AddressLen = sizeof(ClientAddress);
		SOCKET Client = accept(Socket, (struct sockaddr*)&ClientAddress, &AddressLen);
		if (Client != SOCKET_ERROR)
		{
			Clients.emplace_back(Client);
			std::thread(&ISTcpServer::ReadData, this).detach();
		}
		else
		{
			std::cout << ISAlgorithm::GetLastErrorS() << std::endl;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::ReadData()
{
	while (true)
	{
		char Buffer[1024] = { 0 };
		int Result = recv(Clients.front(), Buffer, 1024, 0);
		Result = Result;
		if (Result == 0) //Клиент отключился
		{
			std::cout << "disconnected" << std::endl;
			break;
		}
		else if (Result == SOCKET_ERROR) //Ошибка
		{
			ErrorString = ISAlgorithm::GetLastErrorS();
		}
		else //Пришли данные
		{
			
		}
	}
}
//-----------------------------------------------------------------------------
