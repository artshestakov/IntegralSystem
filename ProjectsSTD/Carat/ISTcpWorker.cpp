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
static std::string QS_SYSTEM = "SELECT stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint "
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
    //������������� ������
    SetRunning(false);

    //��� ���� ������ �� �����������
    while (true)
    {
        ISSleep(1);

        CRITICAL_SECTION_INIT(&CriticalSection);
        bool is_finished = IsFinished;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        if (is_finished) //������ ����������� - ������� �� �����
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
    //�������� ��������� ����������� � ��
    std::string DBHost = ISConfig::Instance().GetValueString("Database", "Host"),
        DBName = ISConfig::Instance().GetValueString("Database", "Name"),
        DBUser = ISConfig::Instance().GetValueString("Database", "User"),
        DBPassword = ISConfig::Instance().GetValueString("Database", "Password");
    unsigned short DBPort = ISConfig::Instance().GetValueUShort("Database", "Port");

    //��������� ��� �����������
    std::stringstream StringStream;
    StringStream << CURRENT_THREAD_ID();
    std::string DBConnectionName = StringStream.str();

    //������������ � ��
    if (!ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword))
    {
        ISLOGGER_E(__CLASS__, "Not connected to database: %s", ISDatabase::Instance().GetErrorString().c_str());
        return;
    }

    //�������� ��������� �� ����������
    DBConnection = ISDatabase::Instance().GetDB(DBConnectionName);

    //������ ��������� �� ������ ������� ���������
    qProtocol = new ISQuery(DBConnection, QI_PROTOCOL);
    
    SetRunning(true);
    while (true)
    {
        ISSleep(1); //������� ������

        //���������, �� ������������� �� ������. ���� �� ������� - ������� �� ������
        if (!GetRunning())
        {
            break;
        }

        //�������� ������� ���������
        CRITICAL_SECTION_LOCK(&CriticalSection);
        ISTcpMessage *tcp_message = CurrentMessage;
        if (tcp_message) 
        {
            IsBusy = true;
            CurrentMessage = nullptr;
        }
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        
        //���� ��������� ��� - ��������� �� ��������� ��������
        if (!tcp_message)
        {
            continue;
        }

        bool Result = false;
        ISUInt64 PerfomanceMsec = 0;
        ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpClient->Socket);

        if (tcp_message->IsValid()) //���� ��������� �������� - ��������� � ����������
        {
            //���� ������ �� ��������������� � ������ ��� �� ������������� - ������
            if (tcp_message->Type != API_AUTH && !tcp_message->TcpClient->Authorized)
            {
                ErrorString = "Not authorized";
            }
            else //������ ������������� - ����������
            {
                ISTimePoint TimePoint = ISAlgorithm::GetTick(); //���������� ������� �����
                Result = Execute(tcp_message, TcpAnswer);
                PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
            }
        }
        else //��������� �� ��������
        {
            ErrorString = tcp_message->GetErrorString();
        }
        
        //��������� ���-���������
        std::string LogText = ISAlgorithm::StringF("%s message \"%s\"\n"
            "Size: %d, Chunk: %d, Parse msec: %llu, Execute msec: %llu",
            (Result ? "Done" : "Failed"), tcp_message->Type.c_str(),
            tcp_message->Size, tcp_message->ChunkCount, tcp_message->MSecParse, PerfomanceMsec);
        if (!Result) //������ �������� � ������� - ������������� ����� ������ � ������ � ���-���������, � ��� �� ������� ������������ �� ������ ��������� ������
        {
            TcpAnswer->SetError(ErrorString);
            LogText.append("\nError string: " + ErrorString);
            ErrorString.clear();
        }

        //������� ���������, ����������, ��������� ����� � ������� � ����������� ������
        delete tcp_message;
        Result ? ISLOGGER_I(__CLASS__, LogText.c_str()) : ISLOGGER_E(__CLASS__, LogText.c_str());
        ISTcpQueue::Instance().AddAnswer(TcpAnswer);

        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsBusy = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
    }

    //��������� ���� ���������
    CRITICAL_SECTION_LOCK(&CriticalSection);
    IsFinished = true;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //���� ��������� �� ������� � ���� ����� - ���������
    auto It = MapFunction.find(TcpMessage->Type);
    if (It != MapFunction.end())
    {
        return (this->*It->second)(TcpMessage, TcpAnswer);
    }
    else //������� �� �������
    {
        ErrorString = ISAlgorithm::StringF("Not support function \"%s\"", TcpMessage->Type);
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName)
{
    //�������� ����������� ��� ���������
    if (!TcpMessage->Parameters.HasMember(ParameterName))
    {
        ErrorString = ISAlgorithm::StringF("Parameter \"%s\" not exist", ParameterName);
        return false;
    }

    //�������� ��������
    rapidjson::Value &JsonValue = TcpMessage->Parameters[ParameterName];

    //���������, �� ������ �� ��������
    if (JsonValue.IsNull())
    {
        ErrorString = ISAlgorithm::StringF("Parameter \"%s\" is null", ParameterName);
        return false;
    }

    switch (JsonValue.GetType()) //��������� ���
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

    case rapidjson::Type::kStringType: //��������� ���
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
    if (VectorString.size() == 4) //������ ����� �������
    {
        std::string Temp = VectorString.back();
        size_t Pos = Temp.find('.');
        if (Pos != NPOS) //����� ����� (����������� ���������� �����)
        {
            Temp.erase(Pos);
            if (!Temp.empty()) //�� ������ ������
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

    //���������, �� ����������� �� ��� ������. ���� ����������� - ������� � �������
    if (TcpMessage->TcpClient->Authorized)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
        return false;
    }

    if (!CheckIsNull(TcpMessage, "Hash"))
    {
        return false;
    }

    //�������� ���
    std::string Hash = TcpMessage->Parameters["Hash"].GetString();

    //��������� ������ ����
    size_t HashSize = Hash.size();
    if (HashSize != CARAT_HASH_SIZE)
    {
        ErrorString = ISAlgorithm::StringF("Invalid hash size %d", HashSize);
        return false;
    }

    //��������� ���������� ����
    for (const char Char : Hash)
    {
        int ASCII = (int)Char; //��������������� ������� ������ � ASCII-���
        if ((ASCII >= 48 && ASCII <= 57) || (ASCII >= 65 && ASCII <= 70)) //���� ������� ������ ������ � �������� [0-9] ��� [a-z] - ��� ����
        {
            continue;
        }
        else //����� - ��� ����������
        {
            ErrorString = "Invalid hash";
            return false;
        }
    }

    {
        //����������� ��� ���� �� ��
        ISQuery qSelectHash(DBConnection, QS_USERS_HASH);
        if (!qSelectHash.Execute()) //������ �������
        {
            return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectHash"), qSelectHash);
        }

        //���� ������ ������ �� ������, ������ � �� ��� �� ������ ������������
        if (qSelectHash.GetResultRowCount() == 0)
        {
            ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
            return false;
        }

        //���� ������������
        bool IsFound = false;
        while (qSelectHash.Next())
        {
            //�������� ��� � ����
            std::string CurrentHash = qSelectHash.ReadColumn_String(0),
                CurrentSalt = qSelectHash.ReadColumn_String(1);

            //����� ���������� ��� ������� �����
            std::string HashResult = ISAlgorithm::SaltPassword(Hash, CurrentSalt);
            IsFound = HashResult == CurrentHash;
            if (IsFound) //�����
            {
                Hash = HashResult;
                break;
            }
        }
    }

    //�������� ������������
    ISQuery qSelectAuth(DBConnection, QS_USER_AUTH);
    qSelectAuth.BindValue(CONST_UID_SETTINGS_DATABASE, UUIDOID);
    qSelectAuth.BindValue(Hash);
    if (!qSelectAuth.ExecuteFirst()) //������ �������� � �������
    {
        return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectLogin"), qSelectAuth);
    }

    unsigned int UserID = qSelectAuth.ReadColumn_UInt(0);
    bool UserSystem = qSelectAuth.ReadColumn_Bool(1);
    std::string UserFIO = qSelectAuth.ReadColumn_String(2);
    unsigned int GroupID = qSelectAuth.ReadColumn_UInt(3);
    bool GroupFullAccess = qSelectAuth.ReadColumn_Bool(4);

    //������ � �� ��������
    if (!qSelectAuth.ReadColumn_Bool(9) && !UserSystem)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
        return false;
    }

    //���� � ������������ ��� ����� �������
    if (!qSelectAuth.ReadColumn_Bool(5))
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
        return false;
    }

    //�������� ������� �������� �� ���������� ������������ � ������
    if (!UserSystem && GroupID == 0)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
        return false;
    }

    //���� ��� ������������ ��������� ����������� ����� �������� ������� ������
    if (qSelectAuth.ReadColumn_Bool(6))
    {
        ISDate DateStart = qSelectAuth.ReadColumn_Date(7),
            DateEnd = qSelectAuth.ReadColumn_Date(8);
        if (!DateStart.IsNull() && !DateEnd.IsNull()) //���� ���� ������ � ��������� ����� �������� �����������
        {
            ISDate CurrentDate = ISDate::CurrentDate();
            if (CurrentDate < DateStart) //���� �������� ������� ������ ��� �� �������
            {
                ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.Auth.LoginLifetimeNotStarted").c_str(), DateStart.ToString().c_str());
                return false;
            }
            else if (CurrentDate > DateEnd) //���� �������� ������� ������ ����������
            {
                ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeEnded");
                return false;
            }
        }
    }

    //��������� ������ �������
    bool IsNeedUpdate = false; //�� ��������� ���� ���������� ������ ���� false (����� ������ �������� ���������� ������)
    unsigned int VersionLast = 0;
    if (TcpMessage->Parameters.HasMember("Version")) //���� ������ �������
    {
        //�������� ������ �������
        unsigned int VersionClient = TcpMessage->Parameters["Version"].GetUint();
        VersionClient = VersionClient;

        //�������� ���������� � ������������
        std::string UpdateClientDir = ISConfig::Instance().GetValueString("Other", "UpdateClientDir");
        if (!UpdateClientDir.empty()) //���� ���������� ��������� - ��� ������
        {
            //��������� ������������� ���������� � ������������
            if (!ISAlgorithm::DirExist(UpdateClientDir))
            {
                ISLOGGER_W(__CLASS__, "Folder \"%s\" not exist. Check config parameter \"Other\\UpdateClientDir\"", UpdateClientDir.c_str());
                ErrorString = LANG("Carat.Error.Query.Auth.UpdatePathNotExist");
                return false;
            }

            //�������� ������ ������ � ��������� ��� �� �������
            ISVectorString VectorString = ISAlgorithm::DirFiles(UpdateClientDir, ErrorString);
            if (!VectorString.empty()) //���� ���������� ���� - ���� ��������� ������
            {
                std::string FileNameLast;
                for (const std::string &FileName : VectorString) //������� ������ ������
                {
                    unsigned int Version = ExtractVersionFile(FileName); //����������� ������ �������� �����
                    if (Version > VersionLast) //���� ������ �������� ����� ���� ��������� - ����������
                    {
                        VersionLast = Version;
                        FileNameLast = FileName;
                    }
                }

                if (VersionLast > 0) //���� ����� ������ - ���������� � � ������� �������
                {
                    IsNeedUpdate = VersionLast > VersionClient;
                }
            }
            //���������� ��� - ��� ������
        }
        else //���������� �� ��������� - �������� �������������� � ��� ������
        {
            ISLOGGER_W(__CLASS__, "Not setting directory updates");
        }
    }

    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &Allocator = TcpAnswer->Parameters.GetAllocator();

    //���������, �� ��������� �� ��� ��� ������������
    //���� ������������ ��� ��������� - ����������� ���������� � �����������
    if (ISTcpClients::Instance().IsExistUserID(UserID))
    {
        ISTcpClientInfo TcpClientInfo = ISTcpClients::Instance().GetInfo(UserID);
        std::string DTConnected = TcpClientInfo.DTConnected.ToString();

        rapidjson::Value AlreadyConnectedObject(rapidjson::Type::kObjectType);
        AlreadyConnectedObject.AddMember("DateTime", rapidjson::Value(DTConnected.c_str(), (rapidjson::SizeType)DTConnected.size()), Allocator);
        AlreadyConnectedObject.AddMember("IPAddress", rapidjson::Value(TcpClientInfo.IPAddress.c_str(), (rapidjson::SizeType)TcpClientInfo.IPAddress.size()), Allocator);
        TcpAnswer->Parameters.AddMember("AlreadyConnected", rapidjson::Value(AlreadyConnectedObject, Allocator), Allocator);
    }

    //������������� ����� �������
    TcpMessage->TcpClient->Authorized = true;
    TcpMessage->TcpClient->UserID = UserID;
    TcpMessage->TcpClient->UserGroupID = GroupID;
    TcpMessage->TcpClient->UserSystem = UserSystem;

    //����� ���������� � ������������ � ������������
    TcpAnswer->Parameters.AddMember("UserID", UserID, Allocator);
    TcpAnswer->Parameters.AddMember("UserIsSystem", UserSystem, Allocator);
    TcpAnswer->Parameters.AddMember("UserFIO", rapidjson::Value(UserFIO.c_str(), (rapidjson::SizeType)UserFIO.size()), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);
    
    //���������� � �������
    rapidjson::Value ServerObject(rapidjson::Type::kObjectType);
    ServerObject.AddMember("Version", 1, Allocator);
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(ServerObject, Allocator), Allocator);

    //���������� �� ���������� �������
    rapidjson::Value UpdateClientObject(rapidjson::Type::kObjectType);
    UpdateClientObject.AddMember("IsNeed", IsNeedUpdate, Allocator);
    UpdateClientObject.AddMember("NewVersion", 1, Allocator);
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClientObject, Allocator), Allocator);

    //���������� � ������������
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
    if (!ValueTimeout.IsInt()) //�������� �� �������� ��������
    {
        ErrorString = "The value is not a integer";
        return false;
    }

    int Timeout = ValueTimeout.GetInt();
    if (Timeout < 0) //�������� ������ ����
    {
        ErrorString = "The value cannot be less than zero";
        return false;
    }

    if (Timeout == 0) //�������� ����� ����
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

    //�������� ��������� ��
    rapidjson::Value SettingsDBObject(rapidjson::Type::kObjectType);
    ISQuery qSelectSettingsDB(DBConnection, QS_SETTINGS_DATABASE);
    qSelectSettingsDB.BindValue(CONST_UID_SETTINGS_DATABASE, UUIDOID);
    if (qSelectSettingsDB.ExecuteFirst())
    {
        if (qSelectSettingsDB.GetResultRowCount() == 0) //���� �������� � �� ��� - ������
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

    //�������� ����� �� �������
    rapidjson::Value AccessTablesObject(rapidjson::Type::kObjectType);
    ISQuery qSelectAccessTables(DBConnection, QS_GROUP_ACCESS_TABLE);
    qSelectAccessTables.BindValue(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessTables.Execute())
    {
        while (qSelectAccessTables.Next())
        {
            const char *TableName = qSelectAccessTables.ReadColumn(0),
                *AccessUID = qSelectAccessTables.ReadColumn(1);
            rapidjson::Value AccessUIDValue(AccessUID, strlen(AccessUID), Allocator);
            if (AccessTablesObject.HasMember(TableName))
            {
                AccessTablesObject[TableName].PushBack(AccessUIDValue, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(AccessUIDValue, Allocator);
                AccessTablesObject.AddMember(rapidjson::Value(TableName, strlen(TableName), Allocator), TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable"), qSelectAccessTables);
    }

    //�������� ����������� �����
    rapidjson::Value AccessSpecialObject(rapidjson::Type::kArrayType);
    ISQuery qSelectAccessSpecial(DBConnection, QS_GROUP_ACCESS_SPECIAL);
    qSelectAccessSpecial.BindValue(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessSpecial.Execute())
    {
        while (qSelectAccessSpecial.Next())
        {
            const char *AccessSpecialUID = qSelectAccessSpecial.ReadColumn(0);
            AccessSpecialObject.PushBack(rapidjson::Value(AccessSpecialUID, strlen(AccessSpecialUID), Allocator), Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial"), qSelectAccessSpecial);
    }

    //�������� ������� � ����������
    rapidjson::Value SystemSubSystemArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSystem(DBConnection, QS_SYSTEM),
        qSelectSubSystem(DBConnection, QS_SUBSYSTEM);
    if (qSelectSystem.Execute(true)) //����������� �������
    {
        while (qSelectSystem.Next())
        {
            const char *SystemUID = qSelectSystem.ReadColumn(1);

            rapidjson::Value SubSystemsArray(rapidjson::Type::kArrayType);
            qSelectSubSystem.BindValue(SystemUID, UUIDOID);
            qSelectSubSystem.BindValue(TcpMessage->TcpClient->UserID);
            qSelectSubSystem.BindValue(TcpMessage->TcpClient->UserSystem);
            if (qSelectSubSystem.Execute(true, 3)) //����������� ���������� ������� �������
            {
                while (qSelectSubSystem.Next())
                {
                    const char *SubSystemUID = qSelectSubSystem.ReadColumn(0),
                        *LocalName = qSelectSubSystem.ReadColumn(1),
                        *IconName = qSelectSubSystem.ReadColumn(2),
                        *ClassName = qSelectSubSystem.ReadColumn(3),
                        *TableName = qSelectSubSystem.ReadColumn(4),
                        *Hint = qSelectSubSystem.ReadColumn(5);
                    rapidjson::Value SubSystemObject(rapidjson::Type::kObjectType);
                    SubSystemObject.AddMember("UID", rapidjson::Value(SubSystemUID, strlen(SubSystemUID)), Allocator);
                    SubSystemObject.AddMember("LocalName", rapidjson::Value(LocalName, strlen(LocalName)), Allocator);
                    SubSystemObject.AddMember("IconName", rapidjson::Value(IconName, strlen(IconName)), Allocator);
                    SubSystemObject.AddMember("ClassName", rapidjson::Value(ClassName, strlen(ClassName)), Allocator);
                    SubSystemObject.AddMember("TableName", rapidjson::Value(TableName, strlen(TableName)), Allocator);
                    SubSystemObject.AddMember("Hint", rapidjson::Value(Hint, strlen(Hint)), Allocator);
                    SubSystemsArray.PushBack(SubSystemObject, Allocator);
                }
            }
            else
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SubSystems"), qSelectSubSystem);
            }

            //��������� ������� ������ ���� �� ��� ��������� �����-������ ����������
            if (qSelectSubSystem.GetResultRowCount() > 0)
            {
                const char *LocalName = qSelectSystem.ReadColumn(1),
                    *IconName = qSelectSystem.ReadColumn(2),
                    *Hint = qSelectSystem.ReadColumn(3);
                rapidjson::Value SystemObject(rapidjson::Type::kObjectType);
                SystemObject.AddMember("IsSystem", qSelectSystem.ReadColumn_Bool(0), Allocator);
                SystemObject.AddMember("UID", rapidjson::Value(SystemUID, strlen(SystemUID)), Allocator);
                SystemObject.AddMember("LocalName", rapidjson::Value(LocalName, strlen(LocalName)), Allocator);
                SystemObject.AddMember("IconName", rapidjson::Value(IconName, strlen(IconName)), Allocator);
                SystemObject.AddMember("Hint", rapidjson::Value(Hint, strlen(Hint)), Allocator);
                SystemObject.AddMember("SubSystems", SubSystemsArray, Allocator);
                SystemSubSystemArray.PushBack(SystemObject, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Systems"), qSelectSystem);
    }

    //�������� ��������� �������
    rapidjson::Value FavoriteObject(rapidjson::Type::kObjectType);
    ISQuery qSelectFavorite(DBConnection, QS_FAVORITE);
    qSelectFavorite.BindValue(TcpMessage->TcpClient->UserID);
    if (qSelectFavorite.Execute())
    {
        while (qSelectFavorite.Next())
        {
            const char *TableName = qSelectFavorite.ReadColumn(0);
            unsigned int ObjectID = qSelectFavorite.ReadColumn_UInt(1);
            rapidjson::Value TableNameV(TableName, strlen(TableName));

            if (FavoriteObject.HasMember(TableName))
            {
                FavoriteObject[TableNameV].PushBack(ObjectID, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(ObjectID, Allocator);
                FavoriteObject.AddMember(TableNameV, TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Favorite"), qSelectFavorite);
    }

    //�������� ������� �����
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

            rapidjson::Value TableNameV(TableName, strlen(TableName)),
                FieldNameV(FieldName, strlen(FieldName));

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

    //�������� ���������������� ���������
    /*QVariantList Settings;
    ISQuery qSelectSettingGroup(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING_GROUP),
        qSelectSettingUser(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING);
    if (qSelectSettingGroup.Execute())
    {
        while (qSelectSettingGroup.Next())
        {
            QString GroupUID = qSelectSettingGroup.ReadColumn("stgp_uid").toString();
            QVariantList SettingsList;
            qSelectSettingUser.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
            qSelectSettingUser.BindValue(":GroupUID", GroupUID);
            if (qSelectSettingUser.Execute())
            {
                while (qSelectSettingUser.Next())
                {
                    QString SettingUID = qSelectSettingUser.ReadColumn("stgs_uid").toString();
                    QVariant SettingDefault = qSelectSettingUser.ReadColumn("stgs_defaultvalue");
                    QVariantMap SettingMap =
                    {
                        { "UID", SettingUID },
                        { "Name", qSelectSettingUser.ReadColumn("stgs_name") },
                        { "Type", qSelectSettingUser.ReadColumn("stgs_type") },
                        { "WidgetEditName", qSelectSettingUser.ReadColumn("stgs_widgeteditname") },
                        { "Local", qSelectSettingUser.ReadColumn("stgs_localname") },
                        { "Hint", qSelectSettingUser.ReadColumn("stgs_hint") },
                        { "Default", SettingDefault }
                    };

                    if (qSelectSettingUser.ReadColumn("count").toInt())  //���� ����� ��������� � ������������ ��� ���� - �������� � ��������
                    {
                        SettingMap["Value"] = qSelectSettingUser.ReadColumn("usst_value");
                    }
                    else //����� ��������� � ������������ ��� - ��������� �� ��������� �� ���������
                    {
                        ISQuery qInsertSetting(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_SETTING);
                        qInsertSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
                        qInsertSetting.BindValue(":SettingUID", SettingUID);
                        qInsertSetting.BindValue(":Value", SettingDefault);
                        if (!qInsertSetting.Execute())
                        {
                            //???
                            ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
                            return false;
                        }
                        SettingMap["Value"] = SettingDefault;
                    }
                    SettingsList.append(SettingMap);
                }
            }
            else
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsUser"), qSelectSettingUser);
            }
            Settings.append(QVariantMap
            {
                { "UID", GroupUID },
                { "Name", qSelectSettingGroup.ReadColumn("stgp_name").toString() },
                { "Local", qSelectSettingGroup.ReadColumn("stgp_localname").toString() },
                { "Icon", qSelectSettingGroup.ReadColumn("stgp_iconname").toString() },
                { "Hint", qSelectSettingGroup.ReadColumn("stgp_hint").toString() },
                { "Settings", SettingsList }
            });
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsGroup"), qSelectSettingGroup);
    }*/

    //�������� ���������
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
            ParagraphObject.AddMember("UID", rapidjson::Value(UID, strlen(UID)), Allocator);
            ParagraphObject.AddMember("Name", rapidjson::Value(Name, strlen(Name)), Allocator);
            ParagraphObject.AddMember("Local", rapidjson::Value(LocalName, strlen(LocalName)), Allocator);
            ParagraphObject.AddMember("ToolTip", rapidjson::Value(ToolTip, strlen(ToolTip)), Allocator);
            ParagraphObject.AddMember("Icon", rapidjson::Value(IconName, strlen(IconName)), Allocator);
            ParagraphObject.AddMember("Class", rapidjson::Value(ClassName, strlen(ClassName)), Allocator);
            ParagraphArray.PushBack(ParagraphObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Paragraph"), qSelectParagraph);
    }

    //�������� ���������� �����
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
            TaskPriorityObject.AddMember("LocalName", rapidjson::Value(LocalName, strlen(LocalName)), Allocator);
            TaskPriorityObject.AddMember("ToolTip", rapidjson::Value(ToolTip, strlen(ToolTip)), Allocator);
            TaskPriorityObject.AddMember("StyleSheet", rapidjson::Value(StyleSheet, strlen(StyleSheet)), Allocator);
            TaskPriorityObject.AddMember("IconName", rapidjson::Value(IconName, strlen(IconName)), Allocator);
            TaskPriorityArray.PushBack(TaskPriorityObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.TaskPriority"), qSelectTaskPriority);
    }

    //������ ����-������
    /*QVariantList MetaDataList;
    QStringList Filter("*.xsn"); //������
    QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //�������� ����-������ ������
    FileInfoList.append(QDir(":_" + ISConfigurations::Instance().Get().Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //�������� ����-������ ������������
    for (const QFileInfo &FileInfo : FileInfoList) //����� ���� XSN ������
    {
        QFile FileXSN(FileInfo.filePath());
        if (!FileXSN.open(QIODevice::ReadOnly)) //�� ������� ������� ���� �� ������ - ������� � �������
        {
            ErrorString = LANG("Carat.Error.Query.GetMetaData.FileXSN").arg(FileInfo.fileName()).arg(FileXSN.errorString());
            return false;
        }
        MetaDataList.append(FileXSN.readAll());
        FileXSN.close();
    }*/

    TcpAnswer->Parameters.AddMember("SettingsDB", rapidjson::Value(SettingsDBObject, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("AccessTables", rapidjson::Value(AccessTablesObject, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("AccessSpecial", rapidjson::Value(AccessSpecialObject, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("SystemSubSystem", rapidjson::Value(SystemSubSystemArray, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("Favorite", rapidjson::Value(FavoriteObject, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("ColumnSize", rapidjson::Value(ColumnSizeObject, Allocator), Allocator);
    //TcpAnswer->Parameters["Settings"] = Settings;
    TcpAnswer->Parameters.AddMember("Paragraphs", rapidjson::Value(ParagraphArray, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("TaskPriority", rapidjson::Value(TaskPriorityArray, Allocator), Allocator);
    //TcpAnswer->Parameters["MetaData"] = MetaDataList;
    return true;
}
//-----------------------------------------------------------------------------
