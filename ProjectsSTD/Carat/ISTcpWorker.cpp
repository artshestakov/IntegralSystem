#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISTcpQueue.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISTcpClients.h"
//-----------------------------------------------------------------------------
static std::string QS_USERS_HASH = "SELECT usrs_hash, usrs_salt "
                                   "FROM _users "
                                   "WHERE usrs_hash IS NOT NULL "
                                   "AND usrs_salt IS NOT NULL";
//-----------------------------------------------------------------------------
static std::string QS_USER_AUTH = "SELECT usrs_id, usrs_issystem, usrs_fio, usrs_group, usgp_fullaccess, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, "
                                  "(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_uid = $1) "
                                  "FROM _users "
                                  "LEFT JOIN _usergroup ON usgp_id = usrs_group "
                                  "WHERE usrs_hash = $2";
//-----------------------------------------------------------------------------
static std::string QI_PROTOCOL = "SELECT protocol_user($1, $2, $3, $4, $5, $6)";
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker()
    : ErrorString(STRING_NO_ERROR),
    IsBusy(false),
    IsRunning(false),
    IsFinished(false),
    CurrentMessage(nullptr),
    DBConnection(nullptr),
    qProtocol(nullptr)
{
    MapFunction[API_AUTH] = &ISTcpWorker::Auth;
    MapFunction[API_SLEEP] = &ISTcpWorker::Sleep;

    CRITICAL_SECTION_INIT(&CriticalSection);
    CRITICAL_SECTION_INIT(&CSRunning);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
    CRITICAL_SECTION_DESTROY(&CriticalSection);
    CRITICAL_SECTION_DESTROY(&CSRunning);
    delete qProtocol;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetBusy()
{
    bool is_busy = false;
    CRITICAL_SECTION_LOCK(&CriticalSection);
    is_busy = IsBusy;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return is_busy;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
    CRITICAL_SECTION_LOCK(&CriticalSection);
    CurrentMessage = TcpMessage;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Start()
{
    std::thread(&ISTcpWorker::Process, this).detach();
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Shutdown()
{
    //Останавливаем воркер
    SetRunning(false);

    //Ждём пока воркер не остановится
    while (true)
    {
        ISSleep(1);

        CRITICAL_SECTION_INIT(&CriticalSection);
        bool is_finished = IsFinished;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        if (is_finished) //Воркер остановимся - выходим из цикла
        {
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetRunning(bool is_running)
{
    CRITICAL_SECTION_LOCK(&CSRunning);
    IsRunning = is_running;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRunning()
{
    bool is_running = false;
    CRITICAL_SECTION_LOCK(&CSRunning);
    is_running = IsRunning;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
    return is_running;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
    //Получаем параметры подключения к БД
    std::string DBHost = ISConfig::Instance().GetValueString("Database", "Host"),
        DBName = ISConfig::Instance().GetValueString("Database", "Name"),
        DBUser = ISConfig::Instance().GetValueString("Database", "User"),
        DBPassword = ISConfig::Instance().GetValueString("Database", "Password");
    unsigned short DBPort = ISConfig::Instance().GetValueUShort("Database", "Port");

    //Формируем имя подключения
    std::stringstream StringStream;
    StringStream << CURRENT_THREAD_ID();
    std::string DBConnectionName = StringStream.str();

    //Подключаемся к БД
    if (!ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword))
    {
        ISLOGGER_E(__CLASS__, "Not connected to database: %s", ISDatabase::Instance().GetErrorString().c_str());
        return;
    }

    //Получаем указатель на соединение
    DBConnection = ISDatabase::Instance().GetDB(DBConnectionName);

    //Создаём указатель на объект запроса протокола
    qProtocol = new ISQuery(DBConnection, QI_PROTOCOL);
    
    SetRunning(true);
    while (true)
    {
        ISSleep(1); //Немного поспим

        //Проверяем, не останавливали ли воркер. Если не запущен - выходим из потока
        if (!GetRunning())
        {
            break;
        }

        //Получаем текущее сообщение
        CRITICAL_SECTION_LOCK(&CriticalSection);
        ISTcpMessage *tcp_message = CurrentMessage;
        if (tcp_message) 
        {
            IsBusy = true;
            CurrentMessage = nullptr;
        }
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        
        //Если сообщения нет - переходим на следующую итерацию
        if (!tcp_message)
        {
            continue;
        }

        bool Result = false;
        ISUInt64 PerfomanceMsec = 0;
        ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpClient->Socket);

        if (tcp_message->IsValid()) //Если сообщение валидное - переходим к выполнению
        {
            //Если запрос не авторизационный и клиент ещё не авторизовался - ошибка
            if (tcp_message->Type != API_AUTH && !tcp_message->TcpClient->Authorized)
            {
                ErrorString = "Not authorized";
            }
            else //Клиент авторизовался - продолжаем
            {
                ISTimePoint TimePoint = ISAlgorithm::GetTick(); //Запоминаем текущее время
                Result = Execute(tcp_message, TcpAnswer);
                PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
            }
        }
        else //Сообщение не валидное
        {
            ErrorString = tcp_message->GetErrorString();
        }
        
        //Формируем лог-сообщение
        std::string LogText = ISAlgorithm::StringF("%s message \"%s\"\n"
            "Size: %d, Chunk: %d, Parse msec: %llu, Execute msec: %llu",
            (Result ? "Done" : "Failed"), tcp_message->Type.c_str(),
            tcp_message->Size, tcp_message->ChunkCount, tcp_message->MSecParse, PerfomanceMsec);
        if (!Result) //Запрос выполнен с ошибкой - устанавливаем текст ошибки в ответе и лог-сообщении, а так же очищаем потенциально не пустые параметры ответа
        {
            TcpAnswer->SetError(ErrorString);
            LogText.append("\nError string: " + ErrorString);
            ErrorString.clear();
        }

        //Удаляем сообщение, логируемся, добавляем ответ в очередь и освобождаем воркер
        delete tcp_message;
        Result ? ISLOGGER_I(__CLASS__, LogText.c_str()) : ISLOGGER_E(__CLASS__, LogText.c_str());
        ISTcpQueue::Instance().AddAnswer(TcpAnswer);

        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsBusy = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
    }

    //Установим флаг остановки
    CRITICAL_SECTION_LOCK(&CriticalSection);
    IsFinished = true;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Ищем указатель на функцию и если нашли - выполняем
    auto It = MapFunction.find(TcpMessage->Type);
    if (It != MapFunction.end())
    {
        return (this->*It->second)(TcpMessage, TcpAnswer);
    }
    else //Функция не найдена
    {
        ErrorString = ISAlgorithm::StringF("Not support function \"%s\"", TcpMessage->Type);
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName)
{
    //Получаем константное имя параметра
    if (!TcpMessage->Parameters.HasMember(ParameterName))
    {
        ErrorString = ISAlgorithm::StringF("Parameter \"%s\" not exist", ParameterName);
        return false;
    }

    //Получаем значение
    rapidjson::Value &JsonValue = TcpMessage->Parameters[ParameterName];

    //Проверяем, не пустое ли значение
    if (JsonValue.IsNull())
    {
        ErrorString = ISAlgorithm::StringF("Parameter \"%s\" is null", ParameterName);
        return false;
    }

    switch (JsonValue.GetType()) //Проверяем тип
    {
    case rapidjson::Type::kNullType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kFalseType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kTrueType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kObjectType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kArrayType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kStringType: //Строковый тип
        if (strlen(JsonValue.GetString()) == 0)
        {
            ErrorString = ISAlgorithm::StringF("Parameter \"%s\" is empty", ParameterName);
            return false;
        }
        break;

    default:
        break;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::ErrorQuery(const std::string &LocalError, ISQuery &SqlQuery)
{
    ErrorString = LocalError;
    ISLOGGER_E(__CLASS__, "Sql query error:\n%s\n%s", SqlQuery.GetSqlText().c_str(), SqlQuery.GetErrorString().c_str());
    return false;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Protocol(unsigned int UserID, const char *ActionUID, const std::string &TableName, const std::string &TableLocalName, unsigned int ObjectID, const std::string &Information)
{
    qProtocol->BindValue(UserID);
    TableName.empty() ? qProtocol->BindValue(nullptr) : qProtocol->BindValue(TableName);
    TableLocalName.empty() ? qProtocol->BindValue(nullptr) : qProtocol->BindValue(TableLocalName);
    qProtocol->BindValue(ActionUID, UUIDOID);
    ObjectID == 0 ? qProtocol->BindValue(nullptr) : qProtocol->BindValue(ObjectID);
    Information.empty() ? qProtocol->BindValue(nullptr) : qProtocol->BindValue(Information);
    if (!qProtocol->Execute(true, 6))
    {
        ISLOGGER_E(__CLASS__, "Not insert protocol: %s", qProtocol->GetErrorString().c_str());
    }
}
//-----------------------------------------------------------------------------
unsigned int ISTcpWorker::ExtractVersionFile(const std::string &FileName)
{
    unsigned int Version = 0;
    ISVectorString VectorString = ISAlgorithm::StringSplit(FileName, '_');
    if (VectorString.size() == 4) //Формат вроде валиден
    {
        std::string Temp = VectorString.back();
        size_t Pos = Temp.find('.');
        if (Pos != NPOS) //Нашли точку (разделитель расширения файла)
        {
            Temp.erase(Pos);
            if (!Temp.empty()) //На всякий случай
            {
                Version = std::stoi(Temp);
            }
        }
    }
    return Version;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    //Проверяем, не авторизаван ли уже клиент. Если авторизован - выходим с ошибкой
    if (TcpMessage->TcpClient->Authorized)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
        return false;
    }

    if (!CheckIsNull(TcpMessage, "Hash"))
    {
        return false;
    }

    //Получаем хэш
    std::string Hash = TcpMessage->Parameters["Hash"].GetString();

    //Проверяем размер хэша
    size_t HashSize = Hash.size();
    if (HashSize != CARAT_HASH_SIZE)
    {
        ErrorString = ISAlgorithm::StringF("Invalid hash size %d", HashSize);
        return false;
    }

    //Проверяем валидность хэша
    for (const char Char : Hash)
    {
        int ASCII = (int)Char; //Преобразовываем текущий символ в ASCII-код
        if ((ASCII >= 48 && ASCII <= 57) || (ASCII >= 65 && ASCII <= 70)) //Если текущий символ входит в диапазон [0-9] или [a-z] - все окей
        {
            continue;
        }
        else //Иначе - хэш невалидный
        {
            ErrorString = "Invalid hash";
            return false;
        }
    }

    {
        //Запрашиваем все хэши из БД
        ISQuery qSelectHash(DBConnection, QS_USERS_HASH);
        if (!qSelectHash.Execute()) //Ошибка запроса
        {
            return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectHash"), qSelectHash);
        }

        //Если запрос ничего не вернул, значит в БД нет ни одного пользователя
        if (qSelectHash.GetResultSize() == 0)
        {
            ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
            return false;
        }

        //Ищем пользователя
        bool IsFound = false;
        while (qSelectHash.Next())
        {
            //Получаем хэш и соль
            std::string CurrentHash = qSelectHash.ReadColumn_String(0),
                CurrentSalt = qSelectHash.ReadColumn_String(1);

            //Солим присланный хэш текущей солью
            std::string HashResult = ISAlgorithm::SaltPassword(Hash, CurrentSalt);
            IsFound = HashResult == CurrentHash;
            if (IsFound) //Нашли
            {
                Hash = HashResult;
                break;
            }
        }
    }

    //Проверка пользователя
    ISQuery qSelectAuth(DBConnection, QS_USER_AUTH);
    qSelectAuth.BindValue(CONST_UID_SETTINGS_DATABASE, UUIDOID);
    qSelectAuth.BindValue(Hash);
    if (!qSelectAuth.ExecuteFirst()) //Запрос выполнен с ошибкой
    {
        return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectLogin"), qSelectAuth);
    }

    unsigned int UserID = qSelectAuth.ReadColumn_UInt(0);
    bool UserSystem = qSelectAuth.ReadColumn_Bool(1);
    std::string UserFIO = qSelectAuth.ReadColumn_String(2);
    unsigned int GroupID = qSelectAuth.ReadColumn_UInt(3);
    bool GroupFullAccess = qSelectAuth.ReadColumn_Bool(4);

    //Доступ к БД запрещен
    if (!qSelectAuth.ReadColumn_Bool(9) && !UserSystem)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
        return false;
    }

    //Если у пользователя нет права доступа
    if (!qSelectAuth.ReadColumn_Bool(5))
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
        return false;
    }

    //Проверка наличия привязки не системного пользователя к группе
    if (!UserSystem && GroupID == 0)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
        return false;
    }

    //Если для пользователя настроено ограничение срока действия учётной записи
    if (qSelectAuth.ReadColumn_Bool(6))
    {
        ISDate DateStart = qSelectAuth.ReadColumn_Date(7),
            DateEnd = qSelectAuth.ReadColumn_Date(8);
        if (!DateStart.IsNull() && !DateEnd.IsNull()) //Если дата начала и окончания срока действия установлена
        {
            ISDate CurrentDate = ISDate::CurrentDate();
            if (CurrentDate < DateStart) //Срок действия учётной записи ещё не начался
            {
                ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.Auth.LoginLifetimeNotStarted").c_str(), DateStart.ToString().c_str());
                return false;
            }
            else if (CurrentDate > DateEnd) //Срок действия учётной записи закончился
            {
                ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeEnded");
                return false;
            }
        }
    }

    //Проверяем версию клиента
    bool IsNeedUpdate = false; //По умолчанию флаг обновления должен быть false (вдруг клиент отправил невалидную версию)
    unsigned int VersionLast = 0;
    if (TcpMessage->Parameters.HasMember("Version")) //Если версия указана
    {
        //Получаем версию клиента
        unsigned int VersionClient = TcpMessage->Parameters["Version"].GetUint();
        VersionClient = VersionClient;

        //Получаем директорию с обновлениями
        std::string UpdateClientDir = ISConfig::Instance().GetValueString("Other", "UpdateClientDir");
        if (!UpdateClientDir.empty()) //Если директория настроена - идём дальше
        {
            //Проверяем существование директории с обновлениями
            if (!ISAlgorithm::DirExist(UpdateClientDir))
            {
                ISLOGGER_W(__CLASS__, "Folder \"%s\" not exist. Check config parameter \"Other\\UpdateClientDir\"", UpdateClientDir.c_str());
                ErrorString = LANG("Carat.Error.Query.Auth.UpdatePathNotExist");
                return false;
            }

            //Получаем список файлов и проверяем его на пустоту
            ISVectorString VectorString = ISAlgorithm::DirFiles(UpdateClientDir, ErrorString);
            if (!VectorString.empty()) //Если обновления есть - ищем последнюю версию
            {
                std::string FileNameLast;
                for (const std::string &FileName : VectorString) //Обходим список файлов
                {
                    unsigned int Version = ExtractVersionFile(FileName); //Вытаскиваем версию текущего файла
                    if (Version > VersionLast) //Если версия текущего файла выше последней - запоминаем
                    {
                        VersionLast = Version;
                        FileNameLast = FileName;
                    }
                }

                if (VersionLast > 0) //Если нашли версию - сравниваем её с версией клиента
                {
                    IsNeedUpdate = VersionLast > VersionClient;
                }
            }
            //Обновлений нет - идём дальше
        }
        else //Директория не настроена - логируем предупреждение и идём дальше
        {
            ISLOGGER_W(__CLASS__, "Not setting directory updates");
        }
    }

    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Проверяем, не подключен ли уже это пользователь
    //Если пользователь уже подключен - вытаскиваем информацию о подключении
    if (ISTcpClients::Instance().IsExistUserID(UserID))
    {
        ISTcpClientInfo TcpClientInfo = ISTcpClients::Instance().GetInfo(UserID);
        std::string DTConnected = TcpClientInfo.DTConnected.ToString();

        rapidjson::Document AlreadyConnected(rapidjson::Type::kObjectType);
        AlreadyConnected.AddMember("DateTime", rapidjson::Value().SetString(DTConnected.c_str(), (rapidjson::SizeType)DTConnected.size(), AlreadyConnected.GetAllocator()), AlreadyConnected.GetAllocator());
        AlreadyConnected.AddMember("IPAddress", rapidjson::Value().SetString(TcpClientInfo.IPAddress.c_str(), (rapidjson::SizeType)TcpClientInfo.IPAddress.size(), AlreadyConnected.GetAllocator()), AlreadyConnected.GetAllocator());
        TcpAnswer->Parameters.AddMember("AlreadyConnected", rapidjson::Value(AlreadyConnected, Allocator), Allocator);
    }

    //Устанавливаем флаги клиенту
    TcpMessage->TcpClient->Authorized = true;
    TcpMessage->TcpClient->UserID = UserID;
    TcpMessage->TcpClient->UserSystem = UserSystem;

    //Отдаём информацию о пользователе и конфигурации
    TcpAnswer->Parameters.AddMember("UserID", UserID, Allocator);
    TcpAnswer->Parameters.AddMember("UserIsSystem", UserSystem, Allocator);
    TcpAnswer->Parameters.AddMember("UserFIO", rapidjson::Value(UserFIO.c_str(), (rapidjson::SizeType)UserFIO.size()), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);
    
    //Информация о сервере
    rapidjson::Document Server(rapidjson::Type::kObjectType);
    Server.AddMember("Version", 1, Server.GetAllocator());
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(Server, Allocator), Allocator);

    //Информация об обновлении клиента
    rapidjson::Document UpdateClient(rapidjson::Type::kObjectType);
    UpdateClient.AddMember("IsNeed", IsNeedUpdate, UpdateClient.GetAllocator());
    UpdateClient.AddMember("NewVersion", 1, UpdateClient.GetAllocator());
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClient, Allocator), Allocator);

    //Информация о конфигурации
    rapidjson::Document Configuration(rapidjson::Type::kObjectType);
    Configuration.AddMember("Name", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    Configuration.AddMember("UID", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    Configuration.AddMember("LocalName", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    Configuration.AddMember("DesktopForm", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    Configuration.AddMember("DateExpired", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    Configuration.AddMember("LogoName", rapidjson::Value().SetNull(), Configuration.GetAllocator());
    TcpAnswer->Parameters.AddMember("Configuration", rapidjson::Value(Configuration, Allocator), Allocator);

    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_ENTER_APPLICATION);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    if (!CheckIsNull(TcpMessage, "Timeout"))
    {
        return false;
    }

    rapidjson::Value &ValueTimeout = TcpMessage->Parameters["Timeout"];
    if (!ValueTimeout.IsInt()) //Значение не является числовым
    {
        ErrorString = "The value is not a integer";
        return false;
    }

    int Timeout = ValueTimeout.GetInt();
    if (Timeout < 0) //Значение меньше нуля
    {
        ErrorString = "The value cannot be less than zero";
        return false;
    }

    if (Timeout == 0) //Значение равно нулю
    {
        ErrorString = "The value cannot be zero";
        return false;
    }
    ISSleep(Timeout);
    return true;
}
//-----------------------------------------------------------------------------
