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
    SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY; //Любой IP адресс
#else
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); //Любой IP адресс
#endif
    SocketAddress.sin_port = htons(ISConfig::Instance().GetValueUShort("TcpServer", "Port")); //Задаём порт
    SocketAddress.sin_family = AF_INET; //AF_INET - Cемейство адресов для IPv4

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

    //Создаём воркеры
    WorkerCount = ISConfig::Instance().GetValueInt("TcpServer", "WorkerCount");
    Workers.resize(WorkerCount);
    for (unsigned int i = 0; i < WorkerCount; ++i)
    {
        ISTcpWorker *TcpWorker = new ISTcpWorker();
        TcpWorker->Start();
        Workers[i] = TcpWorker;
    }

    //Запускаем потоки и выходим
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
    if (WSACleanup() != 0) //Не удалось выгрузить WSA
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

        //При подключении произошла ошибка
        if (SocketClient == SOCKET_ERROR)
        {
            ISLOGGER_C(__CLASS__, "Connect client with error: %s", ISAlgorithm::GetLastErrorS().c_str());
            continue;
        }

        //Завершение работы сервера
        if (SocketClient == NPOS && ISAlgorithm::GetLastErrorN() == WSAEINTR)
        {
            ISLOGGER_I(__CLASS__, "Stopped");
            break;
        }

        //Пытаемся получить IP-адрес клиента и если не получилось - отключаем его
        char Char[15] = { 0 }; //Выделяем 15 байт для хранения IP-адреса
        if (!inet_ntop(AF_INET, &SocketInfo.sin_addr, Char, 15))
        {
            CloseSocket(SocketClient);
            continue;
        }

        //Адрес получили. Добавляем клиента в память
        ISTcpClient *TcpClient = new ISTcpClient();
        TcpClient->Socket = SocketClient;
        TcpClient->IPAddress = Char;
        TcpClient->Port = SocketInfo.sin_port;
        ISTcpClients::Instance().Add(TcpClient);
        ISLOGGER_I(__CLASS__, "Connected %s", TcpClient->IPAddress.c_str());

        //Создаём новым поток для этого клиента
        std::thread(&ISTcpServer::WorkerReader, this, std::ref(TcpClient)).detach();
    }
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerReader(ISTcpClient *TcpClient)
{
    int Result = 0;
    size_t MessageSize = 0;
    char Buffer[TCP_PACKET_MAX_SIZE] = { 0 }; //Буфер для сообщения
    while (true)
    {
        Result = recv(TcpClient->Socket, Buffer, TCP_PACKET_MAX_SIZE, 0);
        if (Result == 0 || //Клиент отключился корректно
            (Result == -1 && ISAlgorithm::GetLastErrorN() == WSAECONNRESET)) //Клиент отключился принудительно - не считаем это ошибкой
        {
            ISLOGGER_I(__CLASS__, "Disconnected %s", TcpClient->IPAddress.c_str());
            break;
        }
        else if (Result == SOCKET_ERROR) //Произошла ошибка
        {
            ISLOGGER_E(__CLASS__, "Socket error: %s", ISAlgorithm::GetLastErrorS().c_str());
            break;
        }
        else if(Result > 0) //Пришли данные
        {
            ISLOGGER_I(__CLASS__, "Receive data: %d bytes", Result);
            if (MessageSize == 0) //Если размер сообщения ещё не получен - получаем его
            {
                for (int i = 0; i < Result; ++i)
                {
                    if (Buffer[i] == '.') //Нашли точку
                    {
                        char Char[20] = { 0 };
                        std::memcpy(Char, Buffer, i); //Получаем размер строкой
                        MessageSize = std::atoi(Char); //Приводим строку с размером к целому числу
                        if (MessageSize > 0) //Приведение прошло успешно
                        {
                            --Result -= i; //Реформируем размер данных
                            std::memmove(Buffer, Buffer + i + 1, Result); //Смещаем буфер (вырезаем размер и точку)
                        }
                        break;
                    }
                }
                if (!MessageSize) //Размер не найден - сообщение не валидное - отключаем клиента
                {
                    ISLOGGER_C(__CLASS__, "Not get message size. Client will be disconnected. Invalid message:\n%s", Buffer);
                    break;
                }
            }
            
            //Добавляем очередную порцию данных в буфер клиента
            TcpClient->AddData(Buffer, Result);

            //Если последний символ текущего пакета не является завершающим - идём на следующую итерацию
            if (Buffer[Result - 1] != CHAR_NULL_TERM)
            {
                continue;
            }

            //Получаем целое сообщение
            std::vector<char> Vector;
            TcpClient->GetBuffer(Vector);

            //Размер заявленных данных не соответствует реальному - отключаем клиента
            size_t VectorSize = Vector.size();
            if (VectorSize != MessageSize)
            {
                ISLOGGER_E(__CLASS__, "Invalid size. Client will be disconnected. Declared size %d, read size %d", MessageSize, VectorSize);
                break;
            }

            //Создаём указатель на сообщение и пытаем парсить
            ISTcpMessage *TcpMessage = new ISTcpMessage();
            TcpMessage->TcpClient = TcpClient;
            if (!ParseMessage(Vector.data(), VectorSize, TcpMessage)) //Не удалось спарсить сообщение
            {
                ISLOGGER_E(__CLASS__, "Invalid message. Client will be disconnected. Error: %s", TcpMessage->GetErrorString().c_str());
            }
            ISTcpQueue::Instance().AddMessage(TcpMessage);
        }

        //Чистим буфер и переменные
        std::memset(Buffer, 0, sizeof(Buffer));
        Result = 0;
        MessageSize = 0;
    }

    //Закрываем сокет
    CloseSocket(TcpClient);
}
//-----------------------------------------------------------------------------
void ISTcpServer::WorkerBalancer()
{
    ISTcpMessage *TcpMessage = nullptr;
    size_t QueueSize = 0;

    while (true)
    {
        if (!GetIsRunning()) //Проверим, не остановлен ли сервер
        {
            break;
        }

        ISSleep(1);
        TcpMessage = ISTcpQueue::Instance().GetMessage(QueueSize);
        if (TcpMessage) //Если есть сообщение на очереди - ищем свободный воркер
        {
            unsigned int Index = 0;
            while (Index < WorkerCount)
            {
                ISTcpWorker *TcpWorker = Workers[Index]; //Получаем текущий воркер
                if (!TcpWorker->GetBusy()) //Если он не занят - передаём ему очередное сообщение и выходим из цикла
                {
                    TcpWorker->SetMessage(TcpMessage);
                    TcpMessage = nullptr;
                    break;
                }
                if (++Index == WorkerCount) //Если текущий индекс сравнялся с количеством воркером - обнуляем его и ждём
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
        if (!GetIsRunning()) //Проверим, не остановлен ли сервер
        {
            break;
        }

        ISSleep(1);
        TcpAnswer = ISTcpQueue::Instance().GetAnswer();
        if (TcpAnswer) //Если на очереди есть ответ - отправляем его клиенту
        {
            if (ISTcpClients::Instance().IsExistSocket(TcpAnswer->GetSocketClient())) //Клиент подключен - отправляем ему ответ
            {
                std::string JsonString = TcpAnswer->ToJson();
                size_t Size = JsonString.size();

                ISLOGGER_I(__CLASS__, "Sending %d bytes...", Size);
                int Result = send(TcpAnswer->GetSocketClient(), JsonString.c_str(), (int)Size, MSG_DONTROUTE);
                if (Result == SOCKET_ERROR) //Ошибка отправки
                {
                    ISLOGGER_E(__CLASS__, "not send answer: %s", ISAlgorithm::GetLastErrorS().c_str());
                }
                else //Данные были отправлены успешно
                {
                    ISLOGGER_I(__CLASS__, "Sended %d of %d bytes", Result, Size);
                }
            }
            else //Клиент не подключен - ничего не отправляем
            {
                ISLOGGER_W(__CLASS__, "Client disconnected, no send answer");
            }
            delete TcpAnswer; //Удаляем указатель на ответ
        }
    }
}
//-----------------------------------------------------------------------------
bool ISTcpServer::ParseMessage(const char *Buffer, size_t BufferSize, ISTcpMessage *TcpMessage)
{
    //Заполняем размер сообщения
    TcpMessage->Size = BufferSize;

    //Засекаем время и парсим сообщение
    rapidjson::Document JsonDocument;
    ISTimePoint TimePoint = ISAlgorithm::GetTick();
    JsonDocument.Parse(Buffer, BufferSize);
    TcpMessage->MSecParse = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);

    //Проверяем результат парсинга
    rapidjson::ParseErrorCode ParseError = JsonDocument.GetParseError();
    if (ParseError != rapidjson::ParseErrorCode::kParseErrorNone)
    {
        TcpMessage->SetErrorString(GetParseError_En(ParseError));
        return false;
    }

    //Документ не является объектом - ошибка
    if (!JsonDocument.IsObject())
    {
        TcpMessage->SetErrorString("message is not JSON-Object");
        return false;
    }

    //Проверяем наличие поля Type
    if (!JsonDocument.HasMember("Type"))
    {
        TcpMessage->SetErrorString("not exist field \"Type\"");
        return false;
    }

    //Проверим что Type - строка
    rapidjson::Value &FieldType = JsonDocument["Type"];
    if (!FieldType.IsString())
    {
        TcpMessage->SetErrorString("field \"Type\" is not a string");
        return false;
    }

    //Значение Type пустое - ошибка
    std::string MessageType = FieldType.GetString();
    if (MessageType.empty())
    {
        TcpMessage->SetErrorString("field \"Type\" is empty");
        return false;
    }
    TcpMessage->Type = MessageType;

    //Если параметры в запросе есть - проверим их тип
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
    //Запоминаем удаляемый сокет
    ISSocket TcpSocket = TcpClient->Socket;

    if (!ISTcpClients::Instance().Remove(TcpClient)) //Клиент не был удалён
    {
        ISLOGGER_C(__CLASS__, "Not remove client with memory");
    }
    CloseSocket(TcpSocket);
}
//-----------------------------------------------------------------------------
void ISTcpServer::CloseSocket(ISSocket Socket)
{
#ifdef WIN32
    //Пытаемся закрыть сокет
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
    //Проверяем, запущен ли сервер
    CRITICAL_SECTION_LOCK(&CSRunning);
    bool is_running = IsRunning;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
    return is_running;
}
//-----------------------------------------------------------------------------
