#include "ISDatabase.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISDatabase::ISDatabase()
    : ErrorString(STRING_NO_ERROR)
{
    CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISDatabase::~ISDatabase()
{
    CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISDatabase& ISDatabase::Instance()
{
    static ISDatabase Database;
    return Database;
}
//-----------------------------------------------------------------------------
std::string ISDatabase::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
PGconn* ISDatabase::GetDB(const std::string &ConnectionName)
{
    PGconn *Connection = NULL;
    CRITICAL_SECTION_LOCK(&CriticalSection);
    auto It = ConnectionsMap.find(ConnectionName);
    if (It != ConnectionsMap.end())
    {
        Connection = It->second;
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return Connection;
}
//-----------------------------------------------------------------------------
ISConnectOptionDB ISDatabase::GetOption(const std::string &ConnectionName)
{
    ISConnectOptionDB ConnectOption;
    CRITICAL_SECTION_LOCK(&CriticalSection);
    auto It = ConnectionsMap.find(ConnectionName);
    if (It != ConnectionsMap.end())
    {
        ConnectOption.Host = PQhost(It->second);
        ConnectOption.Port = static_cast<unsigned short>(std::atoi(PQport(It->second)));
        ConnectOption.Name = PQdb(It->second);
        ConnectOption.Login = PQuser(It->second);
        ConnectOption.Password = PQpass(It->second);
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return ConnectOption;
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const std::string &ConnectionName, const ISConnectOptionDB &ConnectOptionDB)
{
    return Connect(ConnectionName, ConnectOptionDB.Host, ConnectOptionDB.Port, ConnectOptionDB.Name, ConnectOptionDB.Login, ConnectOptionDB.Password);
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const std::string &ConnectionName, const std::string &Host, unsigned short Port, const std::string &Database, const std::string &Login, const std::string &Password)
{
    //Формируем строку подключения
    std::string String = ISAlgorithm::StringF("host=%s port=%d dbname=%s user=%s password=%s connect_timeout=3 application_name=Carat",
        Host.c_str(), Port, Database.c_str(), Login.c_str(), Password.c_str());
    const char *ConnectionString = String.c_str();

    //Пингуем сервер
    PGPing Ping = PQping(ConnectionString);
    if (Ping != PQPING_OK) //Что-то не так
    {
        switch (Ping)
        {
        case PGPing::PQPING_REJECT: ErrorString = "The server reject connection"; break;
        case PGPing::PQPING_NO_RESPONSE: ErrorString = "The server no responce"; break;
        case PGPing::PQPING_NO_ATTEMPT: ErrorString = "No contact was made with the server: invalid connection parameters or out of memory"; break;
        default:
            ErrorString = "Unknown error";
            break;
        }
        return false;
    }

    //Подключаемся и проверяем объект на NULL
    PGconn *Connection = PQconnectdb(ConnectionString);
    if (!Connection)
    {
        ErrorString = "Error PQconnectdb";
        return false;
    }

    if (PQstatus(Connection) != CONNECTION_OK) //Что-то не так
    {
        ErrorString = PQerrorMessage(Connection);
        PQfinish(Connection);
        return false;
    }

    //Добавляем указатель на соединение в список
    CRITICAL_SECTION_LOCK(&CriticalSection);
    ConnectionsMap.emplace(ConnectionName, Connection);
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return true;
}
//-----------------------------------------------------------------------------
void ISDatabase::Disconnect(const std::string &ConnectionName)
{
    CRITICAL_SECTION_LOCK(&CriticalSection);
    auto It = ConnectionsMap.find(ConnectionName);
    if (It != ConnectionsMap.end()) //Нашли соединение
    {
        PQfinish(It->second);
        It->second = NULL;
        ConnectionsMap.erase(It);
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectAll()
{
    std::vector<std::string> Keys = ISAlgorithm::ConvertMapToKeys(ConnectionsMap);
    for (const std::string &ConnectionName : Keys)
    {
        Disconnect(ConnectionName);
    }
}
//-----------------------------------------------------------------------------
