#include "ISTcpServer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "ISTcpQueue.h"
#include "ISConfig.h"
#include "ISTcpClients.h"
//-----------------------------------------------------------------------------
ISTcpServer::ISTcpServer()
    : ErrorString(STRING_NO_ERROR),
    SocketServer(0),
    IsRunning(false),
    WorkerCount(0)
{
    CRITICAL_SECTION_INIT(&CSRunning);
}
//-----------------------------------------------------------------------------
ISTcpServer::~ISTcpServer()
{
    CRITICAL_SECTION_DESTROY(&CSRunning);
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

#ifdef WIN32
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
#endif

    ISSocketAddr SocketAddress;
#ifdef WIN32
    SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY; //����� IP ������
#else
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); //����� IP ������
#endif
    SocketAddress.sin_port = htons(ISConfig::Instance().GetValueUShort("TcpServer", "Port")); //����� ����
    SocketAddress.sin_family = AF_INET; //AF_INET - C�������� ������� ��� IPv4

    SocketServer = socket(AF_INET, SOCK_STREAM, 0);
#ifdef WIN32
    if (SocketServer == INVALID_SOCKET)
#else
    if (SocketServer == SOCKET_ERROR)
#endif
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

    //������ �������
    WorkerCount = ISConfig::Instance().GetValueInt("TcpServer", "WorkerCount");
    Workers.resize(WorkerCount);
    for (unsigned int i = 0; i < WorkerCount; ++i)
    {
        ISTcpWorker *TcpWorker = new ISTcpWorker();
        TcpWorker->Start();
        Workers[i] = TcpWorker;
    }

    //��������� ������ � �������
    IsRunning = true;
    std::thread(&ISTcpServer::WorkerAcceptor, this).detach();
    std::thread(&ISTcpServer::WorkerBalancer, this).detach();
    std::thread(&ISTcpServer::WorkerAnswer, this).detach();
    ISLOGGER_I(__CLASS__, "Started");
    return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::Stop()
{
#ifdef WIN32
    if (WSACleanup() != 0) //�� ������� ��������� WSA
    {
        ISLOGGER_E(__CLASS__, "not clean WSA: %s", ISAlgorithm::GetLastErrorS().c_str());
    }
#endif

    CRITICAL_SECTION_LOCK(&CSRunning);
    IsRunning = false;
    CRITICAL_SECTION_UNLOCK(&CSRunning);

    while (!Workers.empty())
    {
        ISTcpWorker *TcpWorker = ISAlgorithm::VectorTakeBack(Workers);
        TcpWorker->Shutdown();
        delete TcpWorker;
    }
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerAcceptor()
{
    while (true)
    {
        ISSocketAddr SocketInfo = { 0 };
        ISSocketLen AddressLen = sizeof(SocketInfo);
        ISSocket SocketClient = accept(SocketServer, (struct sockaddr*)&SocketInfo, &AddressLen);

        //��� ����������� ��������� ������
        if (SocketClient == SOCKET_ERROR)
        {
            ISLOGGER_C(__CLASS__, "Connect client with error: %s", ISAlgorithm::GetLastErrorS().c_str());
            continue;
        }

        //���������� ������ �������
        if (SocketClient == NPOS && ISAlgorithm::GetLastErrorN() == WSAEINTR)
        {
            ISLOGGER_I(__CLASS__, "Stopped");
            break;
        }

        //�������� �������� IP-����� ������� � ���� �� ���������� - ��������� ���
        char Char[15] = { 0 }; //�������� 15 ���� ��� �������� IP-������
        if (!inet_ntop(AF_INET, &SocketInfo.sin_addr, Char, 15))
        {
            CloseSocket(SocketClient);
            continue;
        }

        //����� ��������. ��������� ������� � ������
        ISTcpClient *TcpClient = new ISTcpClient();
        TcpClient->Socket = SocketClient;
        TcpClient->IPAddress = Char;
        TcpClient->Port = SocketInfo.sin_port;
        ISTcpClients::Instance().Add(TcpClient);
        ISLOGGER_I(__CLASS__, "Connected %s", TcpClient->IPAddress.c_str());

        //������ ����� ����� ��� ����� �������
        std::thread(&ISTcpServer::WorkerReader, this, std::ref(TcpClient)).detach();
    }
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerReader(ISTcpClient *TcpClient)
{
    int Result = 0;
    size_t MessageSize = 0;
    char Buffer[TCP_PACKET_MAX_SIZE] = { 0 }; //����� ��� ���������
    while (true)
    {
        Result = recv(TcpClient->Socket, Buffer, TCP_PACKET_MAX_SIZE, 0);
        if (Result == 0 || //������ ���������� ���������
            (Result == -1 && ISAlgorithm::GetLastErrorN() == WSAECONNRESET)) //������ ���������� ������������� - �� ������� ��� �������
        {
            ISLOGGER_I(__CLASS__, "Disconnected %s", TcpClient->IPAddress.c_str());
            break;
        }
        else if (Result == SOCKET_ERROR) //��������� ������
        {
            ISLOGGER_E(__CLASS__, "Socket error: %s", ISAlgorithm::GetLastErrorS().c_str());
            break;
        }
        else if(Result > 0) //������ ������
        {
            ISLOGGER_I(__CLASS__, "Receive data: %d bytes", Result);
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
                    ISLOGGER_C(__CLASS__, "Not get message size. Client will be disconnected. Invalid message:\n%s", Buffer);
                    break;
                }
            }
            
            //��������� ��������� ������ ������ � ����� �������
            TcpClient->AddData(Buffer, Result);

            //���� ��������� ������ �������� ������ �� �������� ����������� - ��� �� ��������� ��������
            if (Buffer[Result - 1] != CHAR_NULL_TERM)
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
                ISLOGGER_E(__CLASS__, "Invalid size. Client will be disconnected. Declared size %d, read size %d", MessageSize, VectorSize);
                break;
            }

            //������ ��������� �� ��������� � ������ �������
            ISTcpMessage *TcpMessage = new ISTcpMessage();
            TcpMessage->TcpClient = TcpClient;
            if (!ParseMessage(Vector.data(), VectorSize, TcpMessage)) //�� ������� �������� ���������
            {
                ISLOGGER_E(__CLASS__, "Invalid message. Client will be disconnected. Error: %s", TcpMessage->GetErrorString().c_str());
            }
            ISTcpQueue::Instance().AddMessage(TcpMessage);
        }

        //������ ����� � ����������
        std::memset(Buffer, 0, sizeof(Buffer));
        Result = 0;
        MessageSize = 0;
    }

    //��������� �����
    CloseSocket(TcpClient);
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerBalancer()
{
    ISTcpMessage *TcpMessage = nullptr;
    size_t QueueSize = 0;

    while (true)
    {
        if (!GetIsRunning()) //��������, �� ���������� �� ������
        {
            break;
        }

        ISSleep(1);
        TcpMessage = ISTcpQueue::Instance().GetMessage(QueueSize);
        if (TcpMessage) //���� ���� ��������� �� ������� - ���� ��������� ������
        {
            unsigned int Index = 0;
            while (Index < WorkerCount)
            {
                ISTcpWorker *TcpWorker = Workers[Index]; //�������� ������� ������
                if (!TcpWorker->GetBusy()) //���� �� �� ����� - ������� ��� ��������� ��������� � ������� �� �����
                {
                    TcpWorker->SetMessage(TcpMessage);
                    TcpMessage = nullptr;
                    break;
                }
                if (++Index == WorkerCount) //���� ������� ������ ��������� � ����������� �������� - �������� ��� � ���
                {
                    Index = 0;
                    ISLOGGER_W(__CLASS__, "All workers is busy. Queue size %d. Wait %d msec...", QueueSize, CARAT_WAIT_WORKERS_MSEC);
                    ISSleep(CARAT_WAIT_WORKERS_MSEC);
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerAnswer()
{
    ISTcpAnswer *TcpAnswer = nullptr;
    while (true)
    {
        if (!GetIsRunning()) //��������, �� ���������� �� ������
        {
            break;
        }

        ISSleep(1);
        TcpAnswer = ISTcpQueue::Instance().GetAnswer();
        if (TcpAnswer) //���� �� ������� ���� ����� - ���������� ��� �������
        {
            if (ISTcpClients::Instance().IsExistSocket(TcpAnswer->GetSocketClient())) //������ ��������� - ���������� ��� �����
            {
                std::string JsonString = TcpAnswer->ToJson();
                size_t Size = JsonString.size();

                ISLOGGER_I(__CLASS__, "Sending %d bytes...", Size);
                int Result = send(TcpAnswer->GetSocketClient(), JsonString.c_str(), (int)Size, MSG_DONTROUTE);
                if (Result == SOCKET_ERROR) //������ ��������
                {
                    ISLOGGER_E(__CLASS__, "not send answer: %s", ISAlgorithm::GetLastErrorS().c_str());
                }
                else //������ ���� ���������� �������
                {
                    ISLOGGER_I(__CLASS__, "Sended %d of %d bytes", Result, Size);
                }
            }
            else //������ �� ��������� - ������ �� ����������
            {
                ISLOGGER_W(__CLASS__, "Client disconnected, no send answer");
            }
            delete TcpAnswer; //������� ��������� �� �����
        }
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

    //���� ��������� � ������� ���� - �������� �� ���
    if (JsonDocument.HasMember("Parameters"))
    {
        rapidjson::Value &FieldParameters = JsonDocument["Parameters"];
        if (!FieldParameters.IsObject())
        {
            TcpMessage->SetErrorString("field \"Parameters\" is not a JSON-objects");
            return false;
        }
        TcpMessage->Parameters.CopyFrom(FieldParameters, TcpMessage->Parameters.GetAllocator());
    }
    return true;
}
//-----------------------------------------------------------------------------
void ISTcpServer::CloseSocket(ISTcpClient *TcpClient)
{
    //���������� ��������� �����
    ISSocket TcpSocket = TcpClient->Socket;

    if (!ISTcpClients::Instance().Remove(TcpClient)) //������ �� ��� �����
    {
        ISLOGGER_C(__CLASS__, "Not remove client with memory");
    }
    CloseSocket(TcpSocket);
}
//-----------------------------------------------------------------------------
void ISTcpServer::CloseSocket(ISSocket Socket)
{
#ifdef WIN32
    //�������� ������� �����
    if (closesocket(Socket) == SOCKET_ERROR)
    {
        ISLOGGER_E(__CLASS__, "Not close socket %d: %s", Socket, ISAlgorithm::GetLastErrorS().c_str());
    }
#else
    IS_UNUSED(Socket);
#endif
}
//-----------------------------------------------------------------------------
bool ISTcpServer::GetIsRunning()
{
    //���������, ������� �� ������
    CRITICAL_SECTION_LOCK(&CSRunning);
    bool is_running = IsRunning;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
    return is_running;
}
//-----------------------------------------------------------------------------
