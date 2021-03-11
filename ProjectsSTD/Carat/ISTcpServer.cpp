#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer()
	: ErrorString(STRING_NO_ERROR),
	SocketServer(0)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
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
	SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY; //����� IP ������
	SocketAddress.sin_port = htons(50000); //����� ����
	SocketAddress.sin_family = AF_INET; //AF_INET - C�������� ������� ��� IPv4

	SocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (SocketServer == INVALID_SOCKET)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	if (bind(SocketServer, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR)
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}

	if (listen(SocketServer, SOMAXCONN) == SOCKET_ERROR)
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
		SOCKADDR_IN SocketInfo = { 0 };
		int AddressLen = sizeof(SocketInfo);
		SOCKET SocketClient = accept(SocketServer, (struct sockaddr*)&SocketInfo, &AddressLen);
		if (SocketClient != SOCKET_ERROR) //������ ������� �����������
		{
			//�������� �������� IP-����� ������� � ���� �� ���������� - ��������� ���
			char Char[15] = { 0 }; //�������� 15 ���� ��� �������� IP-������
			if (!inet_ntop(AF_INET, &SocketInfo.sin_addr, Char, 32))
			{
				CloseSocket(SocketClient);
				continue;
			}

			//����� ��������. ��������� ������� � ������
			ISTcpClient TcpClient = { SocketClient, Char, SocketInfo.sin_port };
			ClientAdd(TcpClient);
			ISLOGGER_I(__CLASS__, "Connected " + TcpClient.IPAddress);

			//������ ����� ����� ��� ����� �������
			std::thread(&ISTcpServer::ReadData, this, std::ref(TcpClient)).detach();
		}
		else //��� ����������� ��������� ������
		{
			ISLOGGER_C(__CLASS__, "Connect client with error: " + ISAlgorithm::GetLastErrorS());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::ReadData(const ISTcpClient &TcpClient)
{
	while (true)
	{
		char Buffer[1024] = { 0 };
		int Result = recv(TcpClient.Socket, Buffer, 1024, 0);
		if (Result == SOCKET_ERROR)
		{
			int x = 0;
			x = x;
		}
		if (Result == 0) //������ ���������� - ������� ������� �� ������ � ������� �� �����
		{
			CloseSocket(TcpClient.Socket);
			ISLOGGER_I(__CLASS__, "Disconnected " + TcpClient.IPAddress);
			break;
		}
		else if (Result == SOCKET_ERROR) //������
		{
			ISLOGGER_E(__CLASS__, "Not recv() from " + TcpClient.IPAddress + ": " + ISAlgorithm::GetLastErrorS());
			break;
		}
		else //������ ������
		{
			ISLOGGER_I(__CLASS__, "Recive bytes: " + std::to_string(Result));
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::CloseSocket(SOCKET Socket)
{
	if (closesocket(Socket) != SOCKET_ERROR) //����� ������ - ������� ��� �� ������
	{
		bool IsDeleted = false; //���� �������� �������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		for (size_t i = 0, c = Clients.size(); i < c; ++i)
		{
			if (Clients[i].Socket == Socket) //����� ������� �������
			{
				Clients.erase(Clients.begin() + i);
				IsDeleted = true;
				break;
			}
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		if (!IsDeleted) //������ �� ��� �����
		{
			ISLOGGER_C(__CLASS__, "Not remove client with memory");
		}
	}
	else //�� ������� ������� �����
	{
		ISLOGGER_E(__CLASS__, "Not close socket: " + ISAlgorithm::GetLastErrorS());
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::ClientAdd(const ISTcpClient &TcpClient)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Clients.emplace_back(TcpClient);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
