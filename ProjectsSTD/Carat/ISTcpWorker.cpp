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
#include "ISResourcer.h"
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
static std::string QS_SETTINGS_DATABASE = "SELECT sgdb_id, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize, sgdb_tcpmessageid "
                                          "FROM _settingsdatabase "
                                          "WHERE sgdb_uid = $1";
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_TABLE = "SELECT gatb_table, gatt_uid "
                                           "FROM _groupaccesstable "
                                           "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
                                           "WHERE gatb_group = $1";
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_SPECIAL = "SELECT gast_uid "
                                             "FROM _groupaccessspecial "
                                             "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
                                             "WHERE gasp_group = $1";
//-----------------------------------------------------------------------------
static std::string QS_SYSTEM = "SELECT stms_uid, stms_localname, stms_icon, stms_hint "
                               "FROM _systems "
                               "ORDER BY stms_orderid";
//-----------------------------------------------------------------------------
static std::string QS_SUBSYSTEM = "SELECT sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
                                  "FROM _subsystems "
                                  "WHERE sbsm_system = $1 "
                                  "AND check_access_user_subsystem($2, $3, sbsm_uid) "
                                  "ORDER BY sbsm_orderid";
//-----------------------------------------------------------------------------
static std::string QS_FAVORITE = "SELECT fvts_tablename, fvts_objectid "
                                 "FROM _favorites "
                                 "WHERE fvts_user = $1";
//-----------------------------------------------------------------------------
static std::string QS_COLUMN_SIZE = "SELECT clsz_tablename, clsz_fieldname, clsz_size "
                                    "FROM _columnsize "
                                    "WHERE clsz_user = $1";
//-----------------------------------------------------------------------------
static std::string QS_SETTING_GROUP = "SELECT stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint "
                                      "FROM _settingsgroup "
                                      "ORDER BY stgp_order";
//-----------------------------------------------------------------------------
static std::string QS_SETTING = "SELECT stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
                                "usst_value, "
                                "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = $1 AND usst_setting = stgs_id) "
                                "FROM _settings "
                                "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_user = $1 "
                                "WHERE stgs_group = $2 "
                                "ORDER BY stgs_order";
//-----------------------------------------------------------------------------
static std::string QI_USER_SETTING = "INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
                                     "VALUES($1, (SELECT stgs_id FROM _settings WHERE stgs_uid = $2), $3)";
//-----------------------------------------------------------------------------
static std::string QS_PARAGRAPH = "SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
                                  "FROM _paragraphs "
                                  "ORDER BY prhs_orderid";
//-----------------------------------------------------------------------------
static std::string QS_TASK_PRIORITY = "SELECT tspr_id, tspr_name, tspr_tooltip, tspr_stylesheet, tspr_icon "
                                      "FROM _taskpriority "
                                      "ORDER BY tspr_order";
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
    MapFunction[API_GET_META_DATA] = &ISTcpWorker::GetMetaData;

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
PMetaTable* ISTcpWorker::GetMetaTable(const std::string &TableName)
{
    PMetaTable *MetaTable = ISMetaData::Instance().GetTable(TableName);
    if (!MetaTable)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.GetMetaTable").c_str(), TableName.c_str());
    }
    return MetaTable;
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
        if (qSelectHash.GetResultRowCount() == 0)
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
    const char *UserFIO = qSelectAuth.ReadColumn(2);
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

        rapidjson::Value AlreadyConnectedObject(rapidjson::Type::kObjectType);
        AlreadyConnectedObject.AddMember("DateTime", rapidjson::Value(DTConnected.c_str(), (rapidjson::SizeType)DTConnected.size()), Allocator);
        AlreadyConnectedObject.AddMember("IPAddress", rapidjson::Value(TcpClientInfo.IPAddress.c_str(), (rapidjson::SizeType)TcpClientInfo.IPAddress.size()), Allocator);
        TcpAnswer->Parameters.AddMember("AlreadyConnected", rapidjson::Value(AlreadyConnectedObject, Allocator), Allocator);
    }

    //Устанавливаем флаги клиенту
    TcpMessage->TcpClient->Authorized = true;
    TcpMessage->TcpClient->UserID = UserID;
    TcpMessage->TcpClient->UserGroupID = GroupID;
    TcpMessage->TcpClient->UserSystem = UserSystem;

    //Отдаём информацию о пользователе и конфигурации
    TcpAnswer->Parameters.AddMember("UserID", UserID, Allocator);
    TcpAnswer->Parameters.AddMember("UserIsSystem", UserSystem, Allocator);
    TcpAnswer->Parameters.AddMember("UserFIO", rapidjson::Value(UserFIO, (rapidjson::SizeType)strlen(UserFIO)), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);
    
    //Информация о сервере
    rapidjson::Value ServerObject(rapidjson::Type::kObjectType);
    ServerObject.AddMember("Version", 1, Allocator);
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(ServerObject, Allocator), Allocator);

    //Информация об обновлении клиента
    rapidjson::Value UpdateClientObject(rapidjson::Type::kObjectType);
    UpdateClientObject.AddMember("IsNeed", IsNeedUpdate, Allocator);
    UpdateClientObject.AddMember("NewVersion", 1, Allocator);
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClientObject, Allocator), Allocator);

    //Информация о конфигурации
    rapidjson::Value ConfigurationObject(rapidjson::Type::kObjectType);
    ConfigurationObject.AddMember("Name", rapidjson::Value().SetNull(), Allocator);
    ConfigurationObject.AddMember("UID", rapidjson::Value().SetNull(), Allocator);
    ConfigurationObject.AddMember("LocalName", rapidjson::Value().SetNull(), Allocator);
    ConfigurationObject.AddMember("DesktopForm", rapidjson::Value().SetNull(), Allocator);
    ConfigurationObject.AddMember("DateExpired", rapidjson::Value().SetNull(), Allocator);
    ConfigurationObject.AddMember("LogoName", rapidjson::Value().SetNull(), Allocator);
    TcpAnswer->Parameters.AddMember("Configuration", rapidjson::Value(ConfigurationObject, Allocator), Allocator);

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
bool ISTcpWorker::GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Получаем настройки БД
    rapidjson::Value SettingsDBObject(rapidjson::Type::kObjectType);
    ISQuery qSelectSettingsDB(DBConnection, QS_SETTINGS_DATABASE);
    qSelectSettingsDB.BindValue(CONST_UID_SETTINGS_DATABASE, UUIDOID);
    if (qSelectSettingsDB.ExecuteFirst())
    {
        if (qSelectSettingsDB.GetResultRowCount() == 0) //Если настроек в БД нет - ошибка
        {
            ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDBNotActive");
            return false;
        }
        SettingsDBObject.AddMember("ID", rapidjson::Value(qSelectSettingsDB.ReadColumn_UInt(0)), Allocator);
        SettingsDBObject.AddMember("UserAccessDatabase", rapidjson::Value(qSelectSettingsDB.ReadColumn_Bool(1)), Allocator);
        SettingsDBObject.AddMember("NumberSimbolsAfterComma", rapidjson::Value(qSelectSettingsDB.ReadColumn_Int(2)), Allocator);
        SettingsDBObject.AddMember("StorageFileMaxSize", rapidjson::Value(qSelectSettingsDB.ReadColumn_Int(3)), Allocator);
        SettingsDBObject.AddMember("TCPMessageID", rapidjson::Value(qSelectSettingsDB.ReadColumn_UInt64(4)), Allocator);
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsDB"), qSelectSettingsDB);
    }

    //Получаем права на таблицы
    rapidjson::Value AccessTablesObject(rapidjson::Type::kObjectType);
    ISQuery qSelectAccessTables(DBConnection, QS_GROUP_ACCESS_TABLE);
    qSelectAccessTables.BindValue(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessTables.Execute())
    {
        while (qSelectAccessTables.Next())
        {
            const char *TableName = qSelectAccessTables.ReadColumn(0),
                *AccessUID = qSelectAccessTables.ReadColumn(1);
            rapidjson::Value AccessUIDValue(AccessUID, (rapidjson::SizeType)strlen(AccessUID), Allocator);
            if (AccessTablesObject.HasMember(TableName))
            {
                AccessTablesObject[TableName].PushBack(AccessUIDValue, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(AccessUIDValue, Allocator);
                AccessTablesObject.AddMember(rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName), Allocator), TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable"), qSelectAccessTables);
    }

    //Получаем специальные права
    rapidjson::Value AccessSpecialObject(rapidjson::Type::kArrayType);
    ISQuery qSelectAccessSpecial(DBConnection, QS_GROUP_ACCESS_SPECIAL);
    qSelectAccessSpecial.BindValue(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessSpecial.Execute())
    {
        while (qSelectAccessSpecial.Next())
        {
            const char *AccessSpecialUID = qSelectAccessSpecial.ReadColumn(0);
            AccessSpecialObject.PushBack(rapidjson::Value(AccessSpecialUID, (rapidjson::SizeType)strlen(AccessSpecialUID), Allocator), Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial"), qSelectAccessSpecial);
    }

    //Получаем системы и подсистемы
    rapidjson::Value SystemSubSystemArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSystem(DBConnection, QS_SYSTEM),
        qSelectSubSystem(DBConnection, QS_SUBSYSTEM);
    if (qSelectSystem.Execute(true)) //Запрашиваем системы
    {
        while (qSelectSystem.Next())
        {
            const char *SystemUID = qSelectSystem.ReadColumn(0);

            rapidjson::Value SubSystemsArray(rapidjson::Type::kArrayType);
            qSelectSubSystem.BindValue(SystemUID, UUIDOID);
            qSelectSubSystem.BindValue(TcpMessage->TcpClient->UserID);
            qSelectSubSystem.BindValue(TcpMessage->TcpClient->UserSystem);
            if (qSelectSubSystem.Execute(true, 3)) //Запрашиваем подсистемы текущей системы
            {
                while (qSelectSubSystem.Next())
                {
                    const char *SubSystemUID = qSelectSubSystem.ReadColumn(0),
                        *LocalName = qSelectSubSystem.ReadColumn(1),
                        *IconName = qSelectSubSystem.ReadColumn(2),
                        *ClassName = qSelectSubSystem.ReadColumn(3),
                        *TableName = qSelectSubSystem.ReadColumn(4),
                        *Hint = qSelectSubSystem.ReadColumn(5);

                    size_t IconNameSize = strlen(IconName),
                        ClassNameSize = strlen(ClassName),
                        TableNameSize = strlen(TableName),
                        HintSize = strlen(Hint);

                    rapidjson::Value SubSystemObject(rapidjson::Type::kObjectType);
                    SubSystemObject.AddMember("UID", rapidjson::Value(SubSystemUID, (rapidjson::SizeType)strlen(SubSystemUID)), Allocator);
                    SubSystemObject.AddMember("Local", rapidjson::Value(LocalName, (rapidjson::SizeType)strlen(LocalName)), Allocator);
                    SubSystemObject.AddMember("Icon", IconNameSize > 0 ? rapidjson::Value(IconName, (rapidjson::SizeType)IconNameSize) : rapidjson::Value(), Allocator);
                    SubSystemObject.AddMember("Class", ClassNameSize > 0 ? rapidjson::Value(ClassName, (rapidjson::SizeType)ClassNameSize) : rapidjson::Value(), Allocator);
                    SubSystemObject.AddMember("Table", TableNameSize > 0 ? rapidjson::Value(TableName, (rapidjson::SizeType)TableNameSize) : rapidjson::Value(), Allocator);
                    SubSystemObject.AddMember("Hint", HintSize > 0 ? rapidjson::Value(Hint, (rapidjson::SizeType)HintSize) : rapidjson::Value(), Allocator);
                    SubSystemsArray.PushBack(SubSystemObject, Allocator);
                }
            }
            else
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SubSystems"), qSelectSubSystem);
            }

            //Добавляем систему только если по ней разрешены какие-нибудь подсистемы
            if (qSelectSubSystem.GetResultRowCount() > 0)
            {
                const char *LocalName = qSelectSystem.ReadColumn(1),
                    *IconName = qSelectSystem.ReadColumn(2),
                    *Hint = qSelectSystem.ReadColumn(3);
                
                size_t IconNameSize = strlen(IconName),
                    HintSize = strlen(Hint);

                rapidjson::Value SystemObject(rapidjson::Type::kObjectType);
                SystemObject.AddMember("IsSystem", qSelectSystem.ReadColumn_Bool(0), Allocator);
                SystemObject.AddMember("UID", rapidjson::Value(SystemUID, (rapidjson::SizeType)strlen(SystemUID)), Allocator);
                SystemObject.AddMember("Local", rapidjson::Value(LocalName, (rapidjson::SizeType)strlen(LocalName)), Allocator);
                SystemObject.AddMember("Icon", IconNameSize > 0 ? rapidjson::Value(IconName, (rapidjson::SizeType)IconNameSize) : rapidjson::Value(), Allocator);
                SystemObject.AddMember("Hint", HintSize > 0 ? rapidjson::Value(Hint, (rapidjson::SizeType)HintSize) : rapidjson::Value(), Allocator);
                SystemObject.AddMember("SubSystems", SubSystemsArray, Allocator);
                SystemSubSystemArray.PushBack(SystemObject, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Systems"), qSelectSystem);
    }

    //Получаем избранные объекты
    rapidjson::Value FavoriteObject(rapidjson::Type::kObjectType);
    ISQuery qSelectFavorite(DBConnection, QS_FAVORITE);
    qSelectFavorite.BindValue(TcpMessage->TcpClient->UserID);
    if (qSelectFavorite.Execute())
    {
        while (qSelectFavorite.Next())
        {
            const char *TableName = qSelectFavorite.ReadColumn(0);
            unsigned int ObjectID = qSelectFavorite.ReadColumn_UInt(1);

            if (FavoriteObject.HasMember(TableName))
            {
                FavoriteObject[rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName))].PushBack(ObjectID, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(ObjectID, Allocator);
                FavoriteObject.AddMember(rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName)), TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Favorite"), qSelectFavorite);
    }

    //Получаем размеры полей
    rapidjson::Value ColumnSizeObject(rapidjson::Type::kObjectType);
    ISQuery qSelectColumnSize(DBConnection, QS_COLUMN_SIZE);
    qSelectColumnSize.BindValue(TcpMessage->TcpClient->UserID);
    if (qSelectColumnSize.Execute())
    {
        while (qSelectColumnSize.Next())
        {
            const char *TableName = qSelectColumnSize.ReadColumn(0),
                *FieldName = qSelectColumnSize.ReadColumn(1);
            unsigned int Size = qSelectColumnSize.ReadColumn_UInt(2);

            rapidjson::Value TableNameV(TableName, (rapidjson::SizeType)strlen(TableName)),
                FieldNameV(FieldName, (rapidjson::SizeType)strlen(FieldName));

            if (ColumnSizeObject.HasMember(TableName))
            {
                ColumnSizeObject[TableName].AddMember(FieldNameV, Size, Allocator);
            }
            else
            {
                rapidjson::Value TempObject(rapidjson::Type::kObjectType);
                TempObject.AddMember(FieldNameV, Size, Allocator);
                ColumnSizeObject.AddMember(TableNameV, TempObject, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.ColumnSize"), qSelectColumnSize);
    }

    //Получаем пользовательские настройки
    rapidjson::Value SettingGroupArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSettingGroup(DBConnection, QS_SETTING_GROUP),
        qSelectSettingUser(DBConnection, QS_SETTING);
    if (qSelectSettingGroup.Execute())
    {
        while (qSelectSettingGroup.Next())
        {
            const char *GroupUID = qSelectSettingGroup.ReadColumn(0),
                *GroupName = qSelectSettingGroup.ReadColumn(1),
                *GroupLocalName = qSelectSettingGroup.ReadColumn(2),
                *GroupIconName = qSelectSettingGroup.ReadColumn(3),
                *GroupHint = qSelectSettingGroup.ReadColumn(4);
            
            rapidjson::Value SettingsArray(rapidjson::Type::kArrayType);
            qSelectSettingUser.BindValue(TcpMessage->TcpClient->UserID);
            qSelectSettingUser.BindValue(GroupUID, UUIDOID);
            if (qSelectSettingUser.Execute())
            {
                while (qSelectSettingUser.Next())
                {
                    const char *SettingUID = qSelectSettingUser.ReadColumn(0),
                        *SettingName = qSelectSettingUser.ReadColumn(1),
                        *SettingType = qSelectSettingUser.ReadColumn(2),
                        *SettingWidgetEditName = qSelectSettingUser.ReadColumn(3),
                        *SettingLocalName = qSelectSettingUser.ReadColumn(4),
                        *SettingHint = qSelectSettingUser.ReadColumn(5),
                        *SettingDefault = qSelectSettingUser.ReadColumn(6);
                    
                    rapidjson::Value SettingObject(rapidjson::Type::kObjectType);
                    SettingObject.AddMember("UID", rapidjson::Value(SettingUID, (rapidjson::SizeType)strlen(SettingUID)), Allocator);
                    SettingObject.AddMember("Name", rapidjson::Value(SettingName, (rapidjson::SizeType)strlen(SettingName)), Allocator);
                    SettingObject.AddMember("Type", rapidjson::Value(SettingType, (rapidjson::SizeType)strlen(SettingType)), Allocator);
                    SettingObject.AddMember("WidgetEdit", rapidjson::Value(SettingWidgetEditName, (rapidjson::SizeType)strlen(SettingWidgetEditName)), Allocator);
                    SettingObject.AddMember("Local", rapidjson::Value(SettingLocalName, (rapidjson::SizeType)strlen(SettingLocalName)), Allocator);
                    SettingObject.AddMember("Hint", rapidjson::Value(SettingHint, (rapidjson::SizeType)strlen(SettingHint)), Allocator);
                    SettingObject.AddMember("Default", rapidjson::Value(SettingDefault, (rapidjson::SizeType)strlen(SettingDefault)), Allocator);

                    if (qSelectSettingUser.ReadColumn_Int(8))  //Если такая настройка у пользователя уже есть - получаем её значение
                    {
                        const char *SettingValue = qSelectSettingUser.ReadColumn(7);
                        SettingObject.AddMember("Value", rapidjson::Value(SettingValue, (rapidjson::SizeType)strlen(SettingValue)), Allocator);
                    }
                    else //Такой настройки у пользователя нет - добавляем со значением по умолчанию
                    {
                        ISQuery qInsertSetting(DBConnection, QI_USER_SETTING);
                        qInsertSetting.BindValue(TcpMessage->TcpClient->UserID);
                        qInsertSetting.BindValue(SettingUID, UUIDOID);
                        qInsertSetting.BindValue(SettingDefault);
                        if (!qInsertSetting.Execute())
                        {
                            ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
                            return false;
                        }
                        SettingObject.AddMember("Value", rapidjson::Value(SettingDefault, (rapidjson::SizeType)strlen(SettingDefault)), Allocator);
                    }
                    SettingsArray.PushBack(SettingObject, Allocator);
                }
            }
            else
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsUser"), qSelectSettingUser);
            }

            rapidjson::Value SettingGroupObject(rapidjson::Type::kObjectType);
            SettingGroupObject.AddMember("UID", rapidjson::Value(GroupUID, (rapidjson::SizeType)strlen(GroupUID)), Allocator);
            SettingGroupObject.AddMember("Name", rapidjson::Value(GroupName, (rapidjson::SizeType)strlen(GroupName)), Allocator);
            SettingGroupObject.AddMember("Local", rapidjson::Value(GroupLocalName, (rapidjson::SizeType)strlen(GroupLocalName)), Allocator);
            SettingGroupObject.AddMember("Icon", rapidjson::Value(GroupIconName, (rapidjson::SizeType)strlen(GroupIconName)), Allocator);
            SettingGroupObject.AddMember("Hint", rapidjson::Value(GroupHint, (rapidjson::SizeType)strlen(GroupHint)), Allocator);
            SettingGroupObject.AddMember("Settings", SettingsArray, Allocator);
            SettingGroupArray.PushBack(SettingGroupObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsGroup"), qSelectSettingGroup);
    }

    //Получаем параграфы
    rapidjson::Value ParagraphArray(rapidjson::Type::kArrayType);
    ISQuery qSelectParagraph(DBConnection, QS_PARAGRAPH);
    if (qSelectParagraph.Execute())
    {
        while (qSelectParagraph.Next())
        {
            const char *UID = qSelectParagraph.ReadColumn(0),
                *Name = qSelectParagraph.ReadColumn(1),
                *LocalName = qSelectParagraph.ReadColumn(2),
                *ToolTip = qSelectParagraph.ReadColumn(3),
                *IconName = qSelectParagraph.ReadColumn(4),
                *ClassName = qSelectParagraph.ReadColumn(5);

            rapidjson::Value ParagraphObject(rapidjson::Type::kObjectType);
            ParagraphObject.AddMember("UID", rapidjson::Value(UID, (rapidjson::SizeType)strlen(UID)), Allocator);
            ParagraphObject.AddMember("Name", rapidjson::Value(Name, (rapidjson::SizeType)strlen(Name)), Allocator);
            ParagraphObject.AddMember("Local", rapidjson::Value(LocalName, (rapidjson::SizeType)strlen(LocalName)), Allocator);
            ParagraphObject.AddMember("ToolTip", rapidjson::Value(ToolTip, (rapidjson::SizeType)strlen(ToolTip)), Allocator);
            ParagraphObject.AddMember("Icon", rapidjson::Value(IconName, (rapidjson::SizeType)strlen(IconName)), Allocator);
            ParagraphObject.AddMember("Class", rapidjson::Value(ClassName, (rapidjson::SizeType)strlen(ClassName)), Allocator);
            ParagraphArray.PushBack(ParagraphObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Paragraph"), qSelectParagraph);
    }

    //Получаем приоритеты задач
    rapidjson::Value TaskPriorityArray(rapidjson::Type::kArrayType);
    ISQuery qSelectTaskPriority(DBConnection, QS_TASK_PRIORITY);
    if (qSelectTaskPriority.Execute())
    {
        while (qSelectTaskPriority.Next())
        {
            unsigned int ID = qSelectTaskPriority.ReadColumn_UInt(0);
            const char *LocalName = qSelectTaskPriority.ReadColumn(1),
                *ToolTip = qSelectTaskPriority.ReadColumn(2),
                *StyleSheet = qSelectTaskPriority.ReadColumn(3),
                *IconName = qSelectTaskPriority.ReadColumn(4);
            rapidjson::Value TaskPriorityObject(rapidjson::Type::kObjectType);
            TaskPriorityObject.AddMember("ID", ID, Allocator);
            TaskPriorityObject.AddMember("Local", rapidjson::Value(LocalName, (rapidjson::SizeType)strlen(LocalName)), Allocator);
            TaskPriorityObject.AddMember("ToolTip", rapidjson::Value(ToolTip, (rapidjson::SizeType)strlen(ToolTip)), Allocator);
            TaskPriorityObject.AddMember("Style", rapidjson::Value(StyleSheet, (rapidjson::SizeType)strlen(StyleSheet)), Allocator);
            TaskPriorityObject.AddMember("Icon", rapidjson::Value(IconName, (rapidjson::SizeType)strlen(IconName)), Allocator);
            TaskPriorityArray.PushBack(TaskPriorityObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.TaskPriority"), qSelectTaskPriority);
    }

    //Читаем мета-данные
    rapidjson::Value MetaDataArray(rapidjson::Type::kArrayType);
    const ISVectorString &VectorString = ISMetaData::Instance().GetVectorXSN();
    for (const std::string &FileName : VectorString)
    {
        const char *Content = ISResourcer::Instance().GetFile(FileName);
        std::string ContentBase64 = ISAlgorithm::Base64Encode(Content);
        MetaDataArray.PushBack(rapidjson::Value(ContentBase64.c_str(), (rapidjson::SizeType)ContentBase64.size()), Allocator);
    }

    TcpAnswer->Parameters.AddMember("SettingsDB", SettingsDBObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessTables", AccessTablesObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessSpecial", AccessSpecialObject, Allocator);
    TcpAnswer->Parameters.AddMember("SystemSubSystem", SystemSubSystemArray, Allocator);
    TcpAnswer->Parameters.AddMember("Favorite", FavoriteObject, Allocator);
    TcpAnswer->Parameters.AddMember("ColumnSize", ColumnSizeObject, Allocator);
    TcpAnswer->Parameters.AddMember("Settings", SettingGroupArray, Allocator);
    TcpAnswer->Parameters.AddMember("Paragraphs", ParagraphArray, Allocator);
    TcpAnswer->Parameters.AddMember("TaskPriority", TaskPriorityArray, Allocator);
    TcpAnswer->Parameters.AddMember("MetaData", MetaDataArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
