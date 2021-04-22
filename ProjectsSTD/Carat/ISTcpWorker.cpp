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
#include "ISConfigurations.h"
#include "ISTcpWorkerHelper.h"
//-----------------------------------------------------------------------------
static std::string QS_USERS_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
                                                 "FROM _users "
                                                 "WHERE usrs_hash IS NOT NULL "
                                                 "AND usrs_salt IS NOT NULL");
//-----------------------------------------------------------------------------
static std::string QS_USER_AUTH = PREPARE_QUERYN("SELECT usrs_id, usrs_issystem, usrs_fio, usrs_group, usgp_fullaccess, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, "
                                                "(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_uid = $1) "
                                                "FROM _users "
                                                "LEFT JOIN _usergroup ON usgp_id = usrs_group "
                                                "WHERE usrs_hash = $2", 2);
//-----------------------------------------------------------------------------
static std::string QI_PROTOCOL = PREPARE_QUERYN("SELECT protocol_user($1, $2, $3, $4, $5, $6)", 6);
//-----------------------------------------------------------------------------
static std::string QS_SETTINGS_DATABASE = PREPARE_QUERYN("SELECT sgdb_id, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize, sgdb_tcpmessageid "
                                                        "FROM _settingsdatabase "
                                                        "WHERE sgdb_uid = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_TABLE = PREPARE_QUERYN("SELECT gatb_table, gatt_uid "
                                                          "FROM _groupaccesstable "
                                                          "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
                                                          "WHERE gatb_group = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERYN("SELECT gast_uid "
                                                            "FROM _groupaccessspecial "
                                                            "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
                                                            "WHERE gasp_group = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_SYSTEM_SUBSYSTEM = PREPARE_QUERYN("SELECT check_access_user_subsystem($1, $2, sbsm_uid), "
                                                        "stms_uid, stms_issystem, stms_localname, stms_icon, stms_hint, "
                                                        "sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
                                                        "FROM _systems "
                                                        "LEFT JOIN _subsystems ON stms_uid = sbsm_system "
                                                        "ORDER BY stms_orderid, sbsm_orderid", 2);
//-----------------------------------------------------------------------------
static std::string QS_FAVORITE = PREPARE_QUERYN("SELECT fvts_tablename, fvts_objectid "
                                                "FROM _favorites "
                                                "WHERE fvts_user = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_COLUMN_SIZE = PREPARE_QUERYN("SELECT clsz_tablename, clsz_fieldname, clsz_size "
                                                   "FROM _columnsize "
                                                   "WHERE clsz_user = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_SETTINGS = PREPARE_QUERYN("SELECT stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint, "
                                                "stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
                                                "usst_value, "
                                                "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = $1 AND usst_setting = stgs_id) "
                                                "FROM _settingsgroup "
                                                "LEFT JOIN _settings ON stgs_group = stgp_uid "
                                                "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_user = $1 "
                                                "ORDER BY stgp_order, stgs_order", 1);
//-----------------------------------------------------------------------------
static std::string QI_USER_SETTING = "INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
                                     "VALUES($1, (SELECT stgs_id FROM _settings WHERE stgs_uid = $2), $3)";
//-----------------------------------------------------------------------------
static std::string QS_PARAGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
                                                "FROM _paragraphs "
                                                "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
static std::string QS_TASK_PRIORITY = PREPARE_QUERY("SELECT tspr_id, tspr_name, tspr_tooltip, tspr_stylesheet, tspr_icon "
                                                    "FROM _taskpriority "
                                                    "ORDER BY tspr_order");
//-----------------------------------------------------------------------------
static std::string QS_USER_PASSWORD_IS_NULL = PREPARE_QUERYN("SELECT "
                                                             "( "
                                                             "SELECT (COUNT(*) > 0)::BOOLEAN is_exist "
                                                             "FROM _users "
                                                             "WHERE usrs_id = $1 "
                                                             "AND usrs_hash IS NOT NULL "
                                                             "AND usrs_salt IS NOT NULL "
                                                             ") "
                                                             "FROM _users "
                                                             "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_USER_PASSWORD = PREPARE_QUERYN("SELECT usrs_hash, usrs_salt "
                                                     "FROM _users "
                                                     "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QU_USER_HASH_RESET = PREPARE_QUERYN("UPDATE _users SET "
                                                       "usrs_hash = NULL, "
                                                       "usrs_salt = NULL "
                                                       "WHERE usrs_id = $1 "
                                                       "RETURNING usrs_fio", 1);
//-----------------------------------------------------------------------------
static std::string QS_USER_IS_SYSTEM = PREPARE_QUERYN("SELECT usrs_issystem "
                                                      "FROM _users "
                                                      "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QU_USER_HASH = PREPARE_QUERYN("UPDATE _users SET "
                                                 "usrs_hash = $1, "
                                                 "usrs_salt = $2 "
                                                 "WHERE usrs_id = $3 "
                                                 "RETURNING usrs_fio", 3);
//-----------------------------------------------------------------------------
static std::string QU_USER_SETTINGS_RESET = PREPARE_QUERYN("UPDATE _usersettings SET "
                                                           "usst_value = (SELECT stgs_defaultvalue FROM _settings WHERE stgs_id = usst_setting) "
                                                           "WHERE usst_user = $1 "
                                                           "RETURNING (SELECT stgs_uid FROM _settings WHERE stgs_id = usst_setting), usst_value", 1);
//-----------------------------------------------------------------------------
static std::string QS_RIGHT_SHOW_TABLE = PREPARE_QUERYN("SELECT check_access_user_table($1, $2, $3)", 3);
//-----------------------------------------------------------------------------
static std::string QS_SORTING = PREPARE_QUERYN("SELECT sgts_fieldname, sgts_sorting "
                                               "FROM _sortingtables "
                                               "WHERE sgts_user = $1 "
                                               "AND sgts_tablename = $2", 2);
//-----------------------------------------------------------------------------
static std::string QU_SORTING = PREPARE_QUERYN("UPDATE _sortingtables SET "
                                               "sgts_fieldname = $1, "
                                               "sgts_sorting = $2 "
                                               "WHERE sgts_user = $3 "
                                               "AND sgts_tablename = $4", 4);
//-----------------------------------------------------------------------------
static std::string QI_SORTING = PREPARE_QUERYN("INSERT INTO _sortingtables(sgts_user, sgts_tablename, sgts_fieldname, sgts_sorting)"
                                               "VALUES($1, $2, $3, $4)", 4);
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
    MapFunction[API_GET_LAST_CLIENT] = &ISTcpWorker::GetLastClient;
    MapFunction[API_USER_PASSWORD_EXIST] = &ISTcpWorker::UserPasswordExist;
    MapFunction[API_USER_PASSWORD_CREATE] = &ISTcpWorker::UserPasswordCreate;
    MapFunction[API_USER_PASSWORD_EDIT] = &ISTcpWorker::UserPasswordEdit;
    MapFunction[API_USER_PASSWORD_RESET] = &ISTcpWorker::UserPasswordReset;
    MapFunction[API_USER_SETTINGS_RESET] = &ISTcpWorker::UserSettingsReset;
    MapFunction[API_GET_TABLE_DATA] = &ISTcpWorker::GetTableData;

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
                ErrorString = LANG("Carat.Error.NotAuthorized");
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
        ErrorString = ISAlgorithm::StringF("Not support function \"%s\"", TcpMessage->Type.c_str());
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
bool ISTcpWorker::GetParameterUInt(ISTcpMessage *TcpMessage, const char *ParameterName, unsigned int &Value)
{
    rapidjson::Value &ParameterValue = TcpMessage->Parameters[ParameterName];
    if (!ParameterValue.IsInt()) //Значение не является числовым
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotInt").c_str(), ParameterName);
        return false;
    }
    Value = ParameterValue.GetUint();
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetParameterString(ISTcpMessage *TcpMessage, const char *ParameterName, std::string &Value)
{
    rapidjson::Value &ParameterValue = TcpMessage->Parameters[ParameterName];
    if (!ParameterValue.IsString())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotString").c_str(), ParameterName);
        return false;
    }
    Value = ParameterValue.GetString();
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
    if (!qProtocol->Execute())
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
bool ISTcpWorker::UserPasswordExist(unsigned int UserID, bool &Exist)
{
    ISQuery qSelectHashIsNull(DBConnection, QS_USER_PASSWORD_IS_NULL);
    qSelectHashIsNull.BindValue(UserID);
    if (!qSelectHashIsNull.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.CheckExistUserPassword"), qSelectHashIsNull);
    }

    if (!qSelectHashIsNull.First()) //Не удалось перейти на первую строку, т.к. пользователя с таким UserID не существует
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.UserNotExist").c_str(), UserID);
        return false;
    }
    Exist = qSelectHashIsNull.ReadColumn_Bool(0);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserIsSystem(unsigned int UserID, bool &IsSystem)
{
    ISQuery qSelectIsSystem(DBConnection, QS_USER_IS_SYSTEM);
    qSelectIsSystem.BindValue(UserID);
    if (!qSelectIsSystem.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.CheckUserIsSystem"), qSelectIsSystem);
    }

    if (!qSelectIsSystem.First())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.UserNotExist").c_str(), UserID);
        return false;
    }
    IsSystem = qSelectIsSystem.ReadColumn_Bool(0);
    return true;
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
    std::string Hash;
    if (!GetParameterString(TcpMessage, "Hash", Hash))
    {
        return false;
    }

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
            ErrorString = LANG("Carat.Error.Query.Auth.InvalidHash");
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
        unsigned int VersionClient = 0;
        if (!GetParameterUInt(TcpMessage, "Version", VersionClient))
        {
            return false;
        }

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
            std::vector<ISFileInfo> VectorFiles = ISAlgorithm::DirFiles(UpdateClientDir, ErrorString,
                ISNamespace::DirFileSorting::EditDate, ISNamespace::SortingOrder::Descending);
            if (!VectorFiles.empty()) //Если обновления есть - ищем последнюю версию
            {
                std::string FilePath = VectorFiles.front().Path;
                VersionLast = ExtractVersionFile(FilePath);
                if (VersionLast > 0)
                {
                    IsNeedUpdate = VersionLast > VersionClient;
                }
                else
                {
                    ISLOGGER_W(__CLASS__, "Invalid format update file name: %s", + FilePath.c_str());
                }
            }
            //Обновлений нет - идём дальше
        }
        else //Директория не настроена - логируем предупреждение и идём дальше
        {
            ISLOGGER_W(__CLASS__, "Not setting directory updates");
        }
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Проверяем, не подключен ли уже это пользователь
    //Если пользователь уже подключен - вытаскиваем информацию о подключении
    if (ISTcpClients::Instance().IsExistUserID(UserID))
    {
        ISTcpClientInfo TcpClientInfo = ISTcpClients::Instance().GetInfo(UserID);
        const char *DTConnected = TcpClientInfo.DTConnected.ToString().c_str(),
            *IPAddress = TcpClientInfo.IPAddress.c_str();

        rapidjson::Value AlreadyConnectedObject(rapidjson::Type::kObjectType);
        AlreadyConnectedObject.AddMember("DateTime", JSON_STRING(DTConnected), Allocator);
        AlreadyConnectedObject.AddMember("IPAddress", JSON_STRING(IPAddress), Allocator);
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
    TcpAnswer->Parameters.AddMember("UserFIO", JSON_STRING(UserFIO), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);
    
    //Информация о сервере
    rapidjson::Value ServerObject(rapidjson::Type::kObjectType);
    ServerObject.AddMember("Version", JSON_NULL, Allocator);
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(ServerObject, Allocator), Allocator);

    //Информация об обновлении клиента
    rapidjson::Value UpdateClientObject(rapidjson::Type::kObjectType);
    UpdateClientObject.AddMember("IsNeed", IsNeedUpdate, Allocator);
    UpdateClientObject.AddMember("NewVersion", VersionLast > 0 ? rapidjson::Value(VersionLast) : JSON_NULL, Allocator);
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClientObject, Allocator), Allocator);

    //Информация о конфигурации
    rapidjson::Value ConfigurationObject(rapidjson::Type::kObjectType);
    ConfigurationObject.AddMember("Name", JSON_STRING(ISConfigurations::Instance().Get().Name.c_str()), Allocator);
    ConfigurationObject.AddMember("UID", JSON_STRING(ISConfigurations::Instance().Get().UID.c_str()), Allocator);
    ConfigurationObject.AddMember("LocalName", JSON_STRING(ISConfigurations::Instance().Get().LocalName.c_str()), Allocator);
    ConfigurationObject.AddMember("DesktopForm", JSON_STRING(ISConfigurations::Instance().Get().DesktopForm.c_str()), Allocator);
    ConfigurationObject.AddMember("DateExpired", JSON_STRING(ISConfigurations::Instance().Get().DateExpired.ToString().c_str()), Allocator);
    ConfigurationObject.AddMember("LogoName", JSON_STRING(ISConfigurations::Instance().Get().LogoName.c_str()), Allocator);
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

    unsigned int Timeout = 0;
    if (!GetParameterUInt(TcpMessage, "Timeout", Timeout))
    {
        return false;
    }

    if (Timeout == 0) //Значение равно нулю
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull").c_str());
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
        SettingsDBObject.AddMember("ID", qSelectSettingsDB.ReadColumn_UInt(0), Allocator);
        SettingsDBObject.AddMember("UserAccessDatabase", qSelectSettingsDB.ReadColumn_Bool(1), Allocator);
        SettingsDBObject.AddMember("NumberSimbolsAfterComma", qSelectSettingsDB.ReadColumn_Int(2), Allocator);
        SettingsDBObject.AddMember("StorageFileMaxSize", qSelectSettingsDB.ReadColumn_Int(3), Allocator);
        SettingsDBObject.AddMember("TCPMessageID", (uint64_t)qSelectSettingsDB.ReadColumn_UInt64(4), Allocator);
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
            if (AccessTablesObject.HasMember(TableName))
            {
                AccessTablesObject[JSON_STRING(TableName)].PushBack(JSON_STRING(AccessUID), Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(JSON_STRING(AccessUID), Allocator);
                AccessTablesObject.AddMember(JSON_STRING(TableName), TempArray, Allocator);
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
            const char *UID = qSelectAccessSpecial.ReadColumn(0);
            AccessSpecialObject.PushBack(JSON_STRING(UID), Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial"), qSelectAccessSpecial);
    }

    //Получаем системы и подсистемы
    rapidjson::Value SystemSubSystemArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSystems(DBConnection, QS_SYSTEM_SUBSYSTEM);
    qSelectSystems.BindValue(TcpMessage->TcpClient->UserID);
    qSelectSystems.BindValue(TcpMessage->TcpClient->UserSystem);
    if (qSelectSystems.Execute())
    {
        const char *CurrentSystemUID = nullptr;
        while (qSelectSystems.Next())
        {
            //Если доступа к подсистеме нет, переходим к следующей
            if (!qSelectSystems.ReadColumn_Bool(0))
            {
                continue;
            }

            const char *SystemUID = qSelectSystems.ReadColumn(1);
            if (!CurrentSystemUID || strcmp(SystemUID, CurrentSystemUID) != 0)
            {
                CurrentSystemUID = SystemUID;
                const char *SystemLocalName = qSelectSystems.ReadColumn(3),
                    *SystemIcon = qSelectSystems.ReadColumn(4),
                    *SystemHint = qSelectSystems.ReadColumn(5);

                rapidjson::Value SystemObject(rapidjson::Type::kObjectType);
                SystemObject.AddMember("IsSystem", qSelectSystems.ReadColumn_Bool(2), Allocator);
                SystemObject.AddMember("UID", JSON_STRINGA(SystemUID, Allocator), Allocator);
                SystemObject.AddMember("Local", JSON_STRINGA(SystemLocalName, Allocator), Allocator);
                SystemObject.AddMember("Icon", strlen(SystemIcon) > 0 ? JSON_STRINGA(SystemIcon, Allocator) : JSON_NULL, Allocator);
                SystemObject.AddMember("Hint", strlen(SystemHint) > 0 ? JSON_STRINGA(SystemHint, Allocator) : JSON_NULL, Allocator);
                SystemObject.AddMember("SubSystems", rapidjson::Value(rapidjson::Type::kArrayType), Allocator);
                SystemSubSystemArray.PushBack(SystemObject, Allocator);
            }

            const char *SubSystemUID = qSelectSystems.ReadColumn(6),
                *SubSystemLocalName = qSelectSystems.ReadColumn(7),
                *SubSystemIcon = qSelectSystems.ReadColumn(8),
                *SubSystemClassName = qSelectSystems.ReadColumn(9),
                *SubSystemTableName = qSelectSystems.ReadColumn(10),
                *SubSystemHint = qSelectSystems.ReadColumn(11);

            rapidjson::Value SubSystemObject(rapidjson::Type::kObjectType);
            SubSystemObject.AddMember("UID", JSON_STRINGA(SubSystemUID, Allocator), Allocator);
            SubSystemObject.AddMember("Local", JSON_STRINGA(SubSystemLocalName, Allocator), Allocator);
            SubSystemObject.AddMember("Icon", strlen(SubSystemIcon) > 0 ? JSON_STRINGA(SubSystemIcon, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Class", strlen(SubSystemClassName) > 0 ? JSON_STRINGA(SubSystemClassName, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Table", strlen(SubSystemTableName) > 0 ? JSON_STRINGA(SubSystemTableName, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Hint", strlen(SubSystemHint) > 0 ? JSON_STRINGA(SubSystemHint, Allocator) : JSON_NULL, Allocator);
            SystemSubSystemArray[SystemSubSystemArray.Size() - 1][JSON_STRING("SubSystems")].PushBack(SubSystemObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Systems"), qSelectSystems);
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
                FavoriteObject[rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName), Allocator)].PushBack(ObjectID, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(ObjectID, Allocator);
                FavoriteObject.AddMember(rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName), Allocator), TempArray, Allocator);
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

            rapidjson::Value VTableName(TableName, (rapidjson::SizeType)strlen(TableName), Allocator),
                VFieldName(FieldName, (rapidjson::SizeType)strlen(FieldName), Allocator);

            if (ColumnSizeObject.HasMember(TableName))
            {
                ColumnSizeObject[VTableName].AddMember(VFieldName, Size, Allocator);
            }
            else
            {
                rapidjson::Value TempObject(rapidjson::Type::kObjectType);
                TempObject.AddMember(VFieldName, Size, Allocator);
                ColumnSizeObject.AddMember(VTableName, TempObject, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.ColumnSize"), qSelectColumnSize);
    }

    //Получаем пользовательские настройки
    rapidjson::Value SettingGroupsArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSettings(DBConnection, QS_SETTINGS);
    qSelectSettings.BindValue(TcpMessage->TcpClient->UserID);
    if (qSelectSettings.Execute())
    {
        const char *CurrentGroupUID = nullptr;
        while (qSelectSettings.Next())
        {
            const char *GroupUID = qSelectSettings.ReadColumn(0),
                *SettingUID = qSelectSettings.ReadColumn(5);
            if (strlen(SettingUID) == 0) //Если идентификатор настройки пустой - значит по этой группе настроек не числится
            {
                continue;
            }

            if (!CurrentGroupUID || strcmp(CurrentGroupUID, GroupUID) != 0)
            {
                CurrentGroupUID = GroupUID;
                const char *GroupName = qSelectSettings.ReadColumn(1),
                    *GroupLocalName = qSelectSettings.ReadColumn(2),
                    *GroupIconName = qSelectSettings.ReadColumn(3),
                    *GroupHint = qSelectSettings.ReadColumn(4);

                rapidjson::Value SettingGroupObject(rapidjson::Type::kObjectType);
                SettingGroupObject.AddMember("UID", JSON_STRINGA(GroupUID, Allocator), Allocator);
                SettingGroupObject.AddMember("Name", JSON_STRINGA(GroupName, Allocator), Allocator);
                SettingGroupObject.AddMember("Local", JSON_STRINGA(GroupLocalName, Allocator), Allocator);
                SettingGroupObject.AddMember("Icon", JSON_STRINGA(GroupIconName, Allocator), Allocator);
                SettingGroupObject.AddMember("Hint", strlen(GroupHint) > 0 ? JSON_STRINGA(GroupHint, Allocator) : JSON_NULL, Allocator);
                SettingGroupObject.AddMember("Settings", rapidjson::Value(rapidjson::Type::kArrayType), Allocator);
                SettingGroupsArray.PushBack(SettingGroupObject, Allocator);
            }

            const char *SettingName = qSelectSettings.ReadColumn(6),
                *SettingType = qSelectSettings.ReadColumn(7),
                *SettingWidget = qSelectSettings.ReadColumn(8),
                *SettingLocalName = qSelectSettings.ReadColumn(9),
                *SettingHint = qSelectSettings.ReadColumn(10),
                *SettingDefault = qSelectSettings.ReadColumn(11),
                *SettingValue = nullptr;

            rapidjson::Value SettingObject(rapidjson::Type::kObjectType);
            SettingObject.AddMember("UID", JSON_STRINGA(SettingUID, Allocator), Allocator);
            SettingObject.AddMember("Name", JSON_STRINGA(SettingName, Allocator), Allocator);
            SettingObject.AddMember("Type", JSON_STRINGA(SettingType, Allocator), Allocator);
            SettingObject.AddMember("Widget", strlen(SettingWidget) > 0 ? JSON_STRINGA(SettingWidget, Allocator) : JSON_NULL, Allocator);
            SettingObject.AddMember("Local", JSON_STRINGA(SettingLocalName, Allocator), Allocator);
            SettingObject.AddMember("Hint", strlen(SettingHint) > 0 ? JSON_STRINGA(SettingHint, Allocator) : JSON_NULL, Allocator);
            SettingObject.AddMember("Default", strlen(SettingDefault) > 0 ? JSON_STRINGA(SettingDefault, Allocator) : JSON_NULL, Allocator);

            if (qSelectSettings.ReadColumn_Int(13) != 0)  //Если такая настройка у пользователя уже есть - получаем её значение
            {
                SettingValue = qSelectSettings.ReadColumn(12);
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
                SettingValue = SettingDefault;
            }
            SettingObject.AddMember("Value", JSON_STRINGA(SettingValue, Allocator), Allocator);
            SettingGroupsArray[SettingGroupsArray.Size() - 1][JSON_STRING("Settings")].PushBack(SettingObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsGroup"), qSelectSettings);
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
            ParagraphObject.AddMember("UID", JSON_STRING(UID), Allocator);
            ParagraphObject.AddMember("Name", JSON_STRING(Name), Allocator);
            ParagraphObject.AddMember("Local", JSON_STRING(LocalName), Allocator);
            ParagraphObject.AddMember("ToolTip", JSON_STRING(ToolTip), Allocator);
            ParagraphObject.AddMember("Icon", JSON_STRING(IconName), Allocator);
            ParagraphObject.AddMember("Class", JSON_STRING(ClassName), Allocator);
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
            TaskPriorityObject.AddMember("Local", JSON_STRING(LocalName), Allocator);
            TaskPriorityObject.AddMember("ToolTip", JSON_STRING(ToolTip), Allocator);
            TaskPriorityObject.AddMember("Style", JSON_STRING(StyleSheet), Allocator);
            TaskPriorityObject.AddMember("Icon", JSON_STRING(IconName), Allocator);
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
        std::string ContentBase64 = ISAlgorithm::Base64Encode(ISResourcer::Instance().GetFile(FileName), ErrorString);
        if (ContentBase64.empty())
        {
            return false;
        }
        MetaDataArray.PushBack(rapidjson::Value(ContentBase64.c_str(), (rapidjson::SizeType)ContentBase64.size(), Allocator), Allocator);
    }

    TcpAnswer->Parameters.AddMember("SettingsDB", SettingsDBObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessTables", AccessTablesObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessSpecial", AccessSpecialObject, Allocator);
    TcpAnswer->Parameters.AddMember("Systems", SystemSubSystemArray, Allocator);
    TcpAnswer->Parameters.AddMember("Favorite", FavoriteObject, Allocator);
    TcpAnswer->Parameters.AddMember("ColumnSize", ColumnSizeObject, Allocator);
    TcpAnswer->Parameters.AddMember("SettingGroups", SettingGroupsArray, Allocator);
    TcpAnswer->Parameters.AddMember("Paragraphs", ParagraphArray, Allocator);
    TcpAnswer->Parameters.AddMember("TaskPriority", TaskPriorityArray, Allocator);
    TcpAnswer->Parameters.AddMember("MetaData", MetaDataArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Получаем отсортированный (по дате) список файлов
    //в настроенной директории и проверяем его на пустоту
    std::vector<ISFileInfo> VectorFiles = ISAlgorithm::DirFiles(ISConfig::Instance().GetValueString("Other", "UpdateClientDir"),
        ISNamespace::DirFileSorting::EditDate, ISNamespace::SortingOrder::Descending);
    if (VectorFiles.empty()) //Если обновлений нет - выходим
    {
        TcpAnswer->Parameters.AddMember("Found", false, Allocator);
        return true;
    }
    const ISFileInfo &FileInfo = VectorFiles.front();

    //Открываем файл
    const char *FilePath = FileInfo.Path.c_str();
    long FileSize = 0;
    unsigned char *FileData = ISAlgorithm::ReadFile(FilePath, "rb", FileSize, ErrorString);
    if (!FileData)
    {
        return false;
    }
    
    //Кодируем в base64, очищаем память и проверяем результат кодирования
    std::string Base64 = ISAlgorithm::Base64Encode(FileData, (size_t)FileSize, ErrorString);
    free(FileData);
    if (Base64.empty())
    {
        return false;
    }
    
    //Протоколируем, конвертируем в base64 и отдаём
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_GET_UPDATE_CLIENT);
    TcpAnswer->Parameters.AddMember("FileName", JSON_STRINGA(FileInfo.Name.c_str(), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("Data", JSON_STRINGA(Base64.c_str(), Allocator), Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    if (!CheckIsNull(TcpMessage, "UserID"))
    {
        return false;
    }

    //Получаем идентификатор пользователя
    unsigned int UserID = 0;
    if (!GetParameterUInt(TcpMessage, "UserID", UserID))
    {
        return false;
    }

    //Проверяем наличие пароля
    bool Exist = true;
    if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
    {
        return false;
    }
    TcpAnswer->Parameters.AddMember("IsExist", Exist, TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    if (!CheckIsNull(TcpMessage, "UserID") || !CheckIsNull(TcpMessage, "Hash"))
    {
        return false;
    }

    //Получаем идентификатор пользователя
    unsigned int UserID = 0;
    if (!GetParameterUInt(TcpMessage, "UserID", UserID))
    {
        return false;
    }

    //Получаем хэш
    std::string Hash;
    if (!GetParameterString(TcpMessage, "Hash", Hash))
    {
        return false;
    }

    //Проверяем пользователя на системность
    bool IsSystem = true;
    if (!UserIsSystem(UserID, IsSystem))
    {
        return false;
    }

    if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UserIsSystem");
        return false;
    }

    //Проверяем наличие пароля
    bool Exist = true;
    if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
    {
        return false;
    }

    if (Exist) //Пароль уже существует - считаем за ошибку
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.AlreadyExist");
        return false;
    }

    //Генерируем соль и солим пароль
    std::string Salt;
    if (!ISAlgorithm::GenerateSalt(Salt, ErrorString)) //Ошибка генерации
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.GenerateSalt");
        return false;
    }
    std::string HashResult = ISAlgorithm::SaltPassword(Hash, Salt);

    //Устанавливаем пароль
    ISQuery qUpdateHash(DBConnection, QU_USER_HASH);
    qUpdateHash.BindValue(HashResult);
    qUpdateHash.BindValue(Salt);
    qUpdateHash.BindValue(UserID);
    if (!qUpdateHash.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash"), qUpdateHash);
    }
    qUpdateHash.First(); //Проверка не требуется

    //Фиксируем создание пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    if (!CheckIsNull(TcpMessage, "UserID") || !CheckIsNull(TcpMessage, "HashOld") || !CheckIsNull(TcpMessage, "Hash"))
    {
        return false;
    }

    //Получаем идентификатор пользователя
    unsigned int UserID = 0;
    if (!GetParameterUInt(TcpMessage, "UserID", UserID))
    {
        return false;
    }

    //Получаем старый хэш
    std::string HashOld;
    if (!GetParameterString(TcpMessage, "HashOld", HashOld))
    {
        return false;
    }

    //Получаем хэш
    std::string Hash;
    if (!GetParameterString(TcpMessage, "Hash", Hash))
    {
        return false;
    }

    //Получаем текущий хэш и соль пользователя
    ISQuery qSelectHash(DBConnection, QS_USER_PASSWORD);
    qSelectHash.BindValue(UserID);
    if (!qSelectHash.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.SelectHash"), qSelectHash);
    }

    if (!qSelectHash.First()) //Пользователя с таким UserID нет в БД
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.UserPasswordEdit.UserNotExist").c_str(), UserID);
        return false;
    }

    //Текущие хэш и соль
    std::string CurrentHash = qSelectHash.ReadColumn_String(0),
        CurrentSalt = qSelectHash.ReadColumn_String(1);

    //Солим присланный хэш и проверяем. Если подсоленый хэш не соответствует тому что в БД - значит вводили неправильный текущий пароль (или логин) - ошибка
    if (ISAlgorithm::SaltPassword(HashOld, CurrentSalt) != CurrentHash)
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.InvalidCurrentLoginOrPassword");
        return false;
    }

    //Генерируем новую соль и солим новый пароль
    if (!ISAlgorithm::GenerateSalt(CurrentSalt, ErrorString)) //Ошибка генерации
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.GenerateSalt");
        return false;
    }
    std::string HashResult = ISAlgorithm::SaltPassword(Hash, CurrentSalt);

    //Обновляем пароль
    ISQuery qUpdateHash(DBConnection, QU_USER_HASH);
    qUpdateHash.BindValue(HashResult);
    qUpdateHash.BindValue(CurrentSalt);
    qUpdateHash.BindValue(UserID);
    if (!qUpdateHash.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.UpdateHash"), qUpdateHash);
    }
    qUpdateHash.First(); //Проверка не требуется

    //Фиксируем изменение пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    if (!CheckIsNull(TcpMessage, "UserID"))
    {
        return false;
    }

    //Получаем идентификатор пользователя
    unsigned int UserID = 0;
    if (!GetParameterUInt(TcpMessage, "UserID", UserID))
    {
        return false;
    }

    //Проверяем пользователя на системность
    bool IsSystem = true;
    if (!UserIsSystem(UserID, IsSystem))
    {
        return false;
    }

    if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordReset.UserIsSystem");
        return false;
    }

    //Проверяем наличие пароля
    bool Exist = true;
    if (!UserPasswordExist(UserID, Exist))
    {
        return false;
    }

    if (!Exist) //Если пароля нет - считаем ошибкой
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordReset.PasswordIsNull");
        return false;
    }

    //Сбрасываем пароль
    ISQuery qUpdateHashReset(DBConnection, QU_USER_HASH_RESET);
    qUpdateHashReset.BindValue(UserID);
    if (!qUpdateHashReset.Execute()) //Не удалось сбросить пароль
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordReset.Reset"), qUpdateHashReset);
    }
    qUpdateHashReset.First(); //Проверка не требуется

    //Фиксируем сброс пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHashReset.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Сбрасываем значения настроек в дефолтные
    ISQuery qUpdate(DBConnection, QU_USER_SETTINGS_RESET);
    qUpdate.BindValue(TcpMessage->TcpClient->UserID);
    if (!qUpdate.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserSettingsReset.Update"), qUpdate);
    }

    //Обходим результат
    rapidjson::Value SettingsObject(rapidjson::Type::kObjectType);
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    while (qUpdate.Next())
    {
        const char *SettingUID = qUpdate.ReadColumn(0), *SettingValue = qUpdate.ReadColumn(1);
        SettingsObject.AddMember(JSON_STRING(SettingUID), JSON_STRINGA(SettingValue, Allocator), Allocator);
    }
    TcpAnswer->Parameters.AddMember("Result", SettingsObject, Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_SETTINGS_RESET);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    if (!CheckIsNull(TcpMessage, "TableName"))
    {
        return false;
    }

    //Получаем имя таблицы
    std::string TableName;
    if (!GetParameterString(TcpMessage, "TableName", TableName))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }
    unsigned int UserID = TcpMessage->TcpClient->UserID;

    //Проверяем наличие права на просмотр данных
    ISQuery qSelectRight(DBConnection, QS_RIGHT_SHOW_TABLE);
    qSelectRight.BindValue(UserID);
    qSelectRight.BindValue(MetaTable->Name);
    qSelectRight.BindValue(CONST_UID_GROUP_ACCESS_TYPE_SHOW, UUIDOID);
    if (!qSelectRight.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectRightShow"), qSelectRight);
    }
    if (!qSelectRight.First())
    {
        ErrorString = qSelectRight.GetErrorString();
        return false;
    }

    //Прав доступа на просмотр данных нет - ошибка
    if (!qSelectRight.ReadColumn_Bool(0))
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableData.NoRightShow").c_str(), MetaTable->LocalListName.c_str());
        return false;
    }

    //Получаем сортировку для этой таблицы
    std::string SortingField;
    ISNamespace::SortingOrder SortingOrder = ISNamespace::SortingOrder::Ascending;
    ISQuery qSelectSorting(DBConnection, QS_SORTING);
    qSelectSorting.BindValue(UserID);
    qSelectSorting.BindValue(MetaTable->Name);
    if (!qSelectSorting.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectSorting"), qSelectSorting);
    }

    if (qSelectSorting.First()) //Сортирока есть - получаем её
    {
        SortingField = qSelectSorting.ReadColumn(0);
        SortingOrder = static_cast<ISNamespace::SortingOrder>(qSelectSorting.ReadColumn_Int(1));

        //Проверяем не указана ли сортировка в запросе
        //Если указана - проверяем - не нужно ли обновить её в БД
        //QVariantMap SortingFieldQuery = TcpMessage->Parameters["Sorting"].toMap();
        if (/*!SortingFieldQuery.isEmpty()*/TcpMessage->Parameters.HasMember("Sorting"))
        {
            rapidjson::Value &SortingObject = TcpMessage->Parameters["Sorting"];
            std::string s = SortingObject["Field"].GetString();
            ISNamespace::SortingOrder srt = static_cast<ISNamespace::SortingOrder>(SortingObject["Order"].GetInt());

            //Если новая сортировка отличается от текущей - сохраняем её в БД
            if (SortingField != /*SortingFieldQuery["Field"].toString()*/s ||
                SortingOrder != /*static_cast<Qt::SortOrder>(SortingFieldQuery["Order"].toUInt())*/srt)
            {
                SortingField = /*SortingFieldQuery["Field"].toString()*/s;
                SortingOrder = /*static_cast<Qt::SortOrder>(SortingFieldQuery["Order"].toUInt())*/srt;

                ISQuery qUpdateSorting(DBConnection, QU_SORTING);
                qUpdateSorting.BindValue(SortingField);
                qUpdateSorting.BindValue((int)SortingOrder);
                qUpdateSorting.BindValue(UserID);
                qUpdateSorting.BindValue(MetaTable->Name);
                if (!qUpdateSorting.Execute())
                {
                    return ErrorQuery(LANG("Carat.Error.Query.GetTableData.UpdateSorting"), qUpdateSorting);
                }
            }
        }
    }
    else //Сортировки нет - добавляем дефолтную
    {
        SortingField = "ID";

        ISQuery qInsertSorting(DBConnection, QI_SORTING);
        qInsertSorting.BindValue(UserID);
        qInsertSorting.BindValue(MetaTable->Name);
        qInsertSorting.BindValue(SortingField);
        qInsertSorting.BindValue((int)SortingOrder);
        if (!qInsertSorting.Execute())
        {
            return ErrorQuery(LANG("Carat.Error.Query.GetTableData.InsertSorting"), qInsertSorting);
        }
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Заполняем служебную информацию
    rapidjson::Value ServiceInfoObject(rapidjson::Type::kObjectType);
    ServiceInfoObject.AddMember("SortingField", JSON_STRING(SortingField.c_str()), Allocator);
    ServiceInfoObject.AddMember("SortingOrder", (int)SortingOrder, Allocator);
    
    //Получаем фильтры
    rapidjson::Value FilterObject(rapidjson::Type::kObjectType);
    if (TcpMessage->Parameters.FindMember("Filter") != TcpMessage->Parameters.MemberEnd())
    {
        FilterObject = TcpMessage->Parameters["Filter"];
    }

    //Получаем поисковые параметры
    rapidjson::Value SearchArray(rapidjson::Type::kArrayType);
    if (TcpMessage->Parameters.FindMember("Search") != TcpMessage->Parameters.MemberEnd())
    {
        SearchArray = TcpMessage->Parameters["Search"];
    }
    bool IsSearch = !SearchArray.Empty();

    //Формируем запрос на выборку
    std::string SqlText = ISTcpWorkerHelper::CreateSqlFromTable(MetaTable, FilterObject, SearchArray, SortingField, SortingOrder);

    ISQuery qSelect(DBConnection, SqlText);
    qSelect.SetShowLongQuery(false);

    //Заполняем параметры запроса
    /*for (const auto &MapItem : FilterMap.toStdMap())
    {
        qSelect.BindValue(":" + MapItem.first, MapItem.second);
    }*/

    if (!qSelect.Execute()) //Запрос не отработал
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableData.Select"), qSelect);
    }

    int FieldCount = qSelect.GetResultColumnCount();
    rapidjson::Value FieldArray(rapidjson::Type::kArrayType),
        RecordArray(rapidjson::Type::kArrayType);
    std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FieldType::Unknown);
    std::vector<bool> VectorForeign(FieldCount, false);

    //Заполняем описание полей
    for (int i = 0; i < FieldCount; ++i)
    {
        PMetaField *MetaField = MetaTable->GetField(qSelect.GetResultFieldName(i));
        VectorType[i] = MetaField->Type; //Заполняем типы сейчас, чтобы использовать их ниже
        VectorForeign[i] = MetaField->Foreign ? true : false;

        rapidjson::Value FieldObject(rapidjson::Type::kObjectType);
        FieldObject.AddMember("Index", i, Allocator);
        FieldObject.AddMember("Name", JSON_STRING(MetaField->Name.c_str()), Allocator);
        FieldObject.AddMember("LocalName", JSON_STRING(MetaField->LocalListName.c_str()), Allocator);
        FieldObject.AddMember("Type", static_cast<int>(MetaField->Type), Allocator);
        FieldObject.AddMember("IsForeign", MetaField->Foreign ? true : false, Allocator);
        FieldObject.AddMember("IsSystem", MetaField->IsSystem, Allocator);
        FieldArray.PushBack(FieldObject, Allocator);
    }

    if (qSelect.First()) //Данные есть
    {
        //Получаем необходимые настройки БД
        //???
        unsigned int Precision = std::atoi(ISTcpWorkerHelper::GetSettingDB(DBConnection, CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA));
        IS_UNUSED(Precision);

        do
        {
            rapidjson::Value Values(rapidjson::Type::kArrayType); //Список значений
            for (int i = 0; i < FieldCount; ++i) //Обходим поля и вытаскиваем значения
            {
                //Получаем тип поля и наличие внешнего ключа
                ISNamespace::FieldType Type = VectorType[i];
                bool IsForeign = VectorForeign[i];

                //Если значение содержит NULL - добавляем пустое и переходим к следующему
                if (qSelect.IsNull(i))
                {
                    Values.PushBack(JSON_NULL, Allocator);
                    continue;
                }
                //Значение не содержит NULL - анализируем

                if (Type == ISNamespace::FieldType::Int || Type == ISNamespace::FieldType::BigInt)
                {
                    if (IsForeign)
                    {
                        Values.PushBack(JSON_STRINGA(qSelect.ReadColumn(i), Allocator), Allocator);
                    }
                    else
                    {
                        Values.PushBack(JSON_STRING(ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(i)).c_str()), Allocator);
                    }
                }
                else if (Type == ISNamespace::FieldType::ID)
                {
                    Values.PushBack(qSelect.ReadColumn_UInt64(i), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Bool)
                {
                    Values.PushBack(qSelect.ReadColumn_Bool(i), Allocator);
                }
                else if (Type == ISNamespace::FieldType::String || Type == ISNamespace::FieldType::Text)
                {
                    Values.PushBack(JSON_STRINGA(qSelect.ReadColumn(i), Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Date || Type == ISNamespace::FieldType::Birthday)
                {
                    std::string String = ISTcpWorkerHelper::ConvertDateToString(qSelect.ReadColumn_Date(i));
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Time)
                {
                    std::string String = qSelect.ReadColumn_Time(i).ToString();
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::DateTime)
                {
                    std::string String = ISTcpWorkerHelper::ConvertDateTimeToString(qSelect.ReadColumn_DateTime(i));
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Phone)
                {
                    const char *String = qSelect.ReadColumn(i);
                    char Temp[19] = { 0 };
                    Temp[0] = '+';
                    Temp[1] = '7';
                    Temp[2] = ' ';
                    Temp[3] = '(';
                    Temp[4] = String[0];
                    Temp[5] = String[1];
                    Temp[6] = String[2];
                    Temp[7] = ')';
                    Temp[8] = ' ';
                    Temp[9] = String[3];
                    Temp[10] = String[4];
                    Temp[11] = String[5];
                    Temp[12] = '-';
                    Temp[13] = String[6];
                    Temp[14] = String[7];
                    Temp[15] = '-';
                    Temp[16] = String[8];
                    Temp[17] = String[9];
                    Temp[18] = CHAR_NULL_TERM;
                    Values.PushBack(JSON_STRINGA(Temp, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Seconds)
                {
                    //Value = QTime(0, 0).addSecs(Value.toInt()).toString(FORMAT_TIME_V3);
                }
                else if (Type == ISNamespace::FieldType::Double)
                {
                    std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Double(i), ' ', Precision);
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Money)
                {
                    std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Double(i), ' ', 2);
                    const char *SString = String.c_str();
                    Values.PushBack(JSON_STRINGA(SString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::UID)
                {
                    const char *String = qSelect.ReadColumn(i);
                    Values.PushBack(JSON_STRINGA(String, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::ProtocolDT)
                {
                    //Values.PushBack(JSON_STRING(ISTcpWorkerHelper::ConvertDateTimeToString(qSelect.ReadColumn_DateTime(i)).c_str()), Allocator);
                }
            }
            RecordArray.PushBack(Values, Allocator); //Добавляем список значений в список записей
        } while (qSelect.Next()); //Обходим выборку	
    }
    TcpAnswer->Parameters.AddMember("ServiceInfo", ServiceInfoObject, Allocator);
    TcpAnswer->Parameters.AddMember("FieldList", FieldArray, Allocator);
    TcpAnswer->Parameters.AddMember("RecordList", RecordArray, Allocator);
    Protocol(TcpMessage->TcpClient->UserID, IsSearch ? CONST_UID_PROTOCOL_SEARCH : CONST_UID_PROTOCOL_GET_TABLE_DATA, MetaTable->Name, MetaTable->LocalListName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::NoteRecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::NoteRecordSet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchTaskText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchTaskID(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::LogGetStructure(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::LogGetContent(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::TaskCommentAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OrganizationFormINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::StatementsQueryGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::StatementsQueryReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetMonitor(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
