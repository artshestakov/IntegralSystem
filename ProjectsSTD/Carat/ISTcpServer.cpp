#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "rapidjson.h"
#include "document.h"
#include "error\en.h"
#include "ISTcpQueue.h"
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
	ISLOGGER_I(__CLASS__, "Starting");

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
	ISLOGGER_I(__CLASS__, "Started");
	return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::Stop()
{
	if (WSACleanup() != 0) //�� ������� ��������� WSA
	{
		ISLOGGER_E(__CLASS__, "not clean WSA: " + ISAlgorithm::GetLastErrorS());
	}
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
			ISTcpClient *TcpClient = new ISTcpClient();
			TcpClient->Socket = SocketClient;
			TcpClient->IPAddress = Char;
			TcpClient->Port = SocketInfo.sin_port;
			ClientAdd(TcpClient);
			ISLOGGER_I(__CLASS__, "Connected " + TcpClient->IPAddress);

			//������ ����� ����� ��� ����� �������
			std::thread(&ISTcpServer::ReadData, this, std::ref(TcpClient)).detach();
		}
		else if (SocketClient == NPOS && GetLastError() == WSAEINTR) //���������� ������ �������
		{
			ISLOGGER_I(__CLASS__, "Stopped");
			break;
		}
		else //��� ����������� ��������� ������
		{
			ISLOGGER_C(__CLASS__, "Connect client with error: " + ISAlgorithm::GetLastErrorS());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpServer::ReadData(ISTcpClient *TcpClient)
{
	int Result = 0;
	size_t MessageSize = 0;
	char Buffer[TCP_PACKET_MAX_SIZE] = { 0 }; //����� ��� ���������
	while (true)
	{
		Result = recv(TcpClient->Socket, Buffer, TCP_PACKET_MAX_SIZE, 0);
		if (Result == 0 || Result == SOCKET_ERROR) //������ ���� ����������, ���� ��������� ������ - � ����� ������ ��������� ��� �������������
		{
			Result == 0 ? ISLOGGER_I(__CLASS__, "Disconnected " + TcpClient->IPAddress) :
				ISLOGGER_E(__CLASS__, "Socket error: " + ISAlgorithm::GetLastErrorS());
			CloseSocket(TcpClient->Socket);
			break;
		}
		else if(Result > 0) //������ ������
		{
			ISLOGGER_I(__CLASS__, "Receive data: " + std::to_string(Result));
			if (MessageSize == 0) //���� ������ ��������� ��� �� ������� - �������� ���
			{
				for (int i = 0; i < Result; ++i)
				{
					if (Buffer[i] == '.') //����� �����
					{
						char Char[20] = { 0 };
						std::memcpy(Char, Buffer, i); //�������� ������ �������
						MessageSize = std::atoi(Char); //�������� ������ � �������� � ������ �����
						if (MessageSize > 0) //���������� ������ �������
						{
							--Result -= i; //����������� ������ ������
							std::memmove(Buffer, Buffer + i + 1, Result); //������� ����� (�������� ������ � �����)
						}
						break;
					}
				}
				if (!MessageSize) //������ �� ������ - ��������� �� �������� - ��������� �������
				{
					ISLOGGER_C(__CLASS__, "Not get message size. Client will be disconnected. Invalid message:\n" + std::string(Buffer, Result));
					CloseSocket(TcpClient->Socket);
					break;
				}
			}
			
			//��������� ��������� ������ ������ � ����� �������
			TcpClient->AddData(Buffer, Result);

			//���� ��������� ������ �������� ������ �� �������� ����������� - ��� �� ��������� ��������
			if (Buffer[Result - 1] != TCP_PAKCET_END_CHAR)
			{
				continue;
			}

			//�������� ����� ���������
			std::vector<char> Vector;
			TcpClient->GetBuffer(Vector);

			//������ ���������� ������ �� ������������� ��������� - ��������� �������
			size_t VectorSize = Vector.size();
			if (VectorSize != MessageSize)
			{
				ISLOGGER_E(__CLASS__, "Invalid size. Client will be disconnected. Declared size " + std::to_string(MessageSize) + ", read size " + std::to_string(VectorSize));
				CloseSocket(TcpClient->Socket);
				break;
			}

			//������ ��������� �� ��������� � ������ �������
			ISTcpMessage *TcpMessage = new ISTcpMessage();
			if (!ParseMessage(Vector.data(), VectorSize, TcpMessage)) //�� ������� �������� ���������
			{
				ISLOGGER_E(__CLASS__, "Invalid message. Client will be disconnected. Error: " + TcpMessage->GetErrorString());
			}
			ISTcpQueue::Instance().AddMessage(TcpMessage);
		}

		//������ ����� � ����������
		std::memset(Buffer, 0, sizeof(Buffer));
		Result = 0;
		MessageSize = 0;
	}
}
//-----------------------------------------------------------------------------
bool ISTcpServer::ParseMessage(const char *Buffer, size_t BufferSize, ISTcpMessage *TcpMessage)
{
	//��������� ������ ���������
	TcpMessage->Size = BufferSize;

	//�������� ����� � ������ ���������
	rapidjson::Document JsonDocument;
	ISTimePoint TimePoint = ISAlgorithm::GetTick();
	JsonDocument.Parse(Buffer, BufferSize);
	TcpMessage->MSecParse = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

	//��������� ��������� ��������
	rapidjson::ParseErrorCode ParseError = JsonDocument.GetParseError();
	if (ParseError != rapidjson::ParseErrorCode::kParseErrorNone)
	{
		TcpMessage->SetErrorString(GetParseError_En(ParseError));
		return false;
	}

	//�������� �� �������� �������� - ������
	if (!JsonDocument.IsObject())
	{
		TcpMessage->SetErrorString("message is not JSON-Object");
		return false;
	}

	//��������� ������� ���� Type
	if (!JsonDocument.HasMember("Type"))
	{
		TcpMessage->SetErrorString("not exist field \"Type\"");
		return false;
	}

	//�������� ��� Type - ������
	rapidjson::Value &FieldType = JsonDocument["Type"];
	if (!FieldType.IsString())
	{
		TcpMessage->SetErrorString("field \"Type\" is not a string");
		return false;
	}

	//�������� Type ������ - ������
	std::string MessageType = FieldType.GetString();
	if (MessageType.empty())
	{
		TcpMessage->SetErrorString("field \"Type\" is empty");
		return false;
	}
	TcpMessage->Type = MessageType;

	rapidjson::Value &FieldParameters = JsonDocument["Parameters"];
	if (!FieldParameters.IsObject())
	{
		TcpMessage->SetErrorString("field \"Parameters\" is not a JSON-objects");
		return false;
	}
	TcpMessage->Parameters = FieldParameters;
	return true;
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
			if (Clients[i]->Socket == Socket) //����� ������� �������
			{
				ISTcpClient *TcpClient = Clients[i];
				Clients.erase(Clients.begin() + i);
				delete TcpClient;
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
void ISTcpServer::ClientAdd(ISTcpClient *TcpClient)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Clients.emplace_back(TcpClient);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
