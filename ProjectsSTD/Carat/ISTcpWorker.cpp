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
        if (qSelectHash.GetResultSize() == 0)
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

        rapidjson::Document AlreadyConnected(rapidjson::Type::kObjectType);
        AlreadyConnected.AddMember("DateTime", rapidjson::Value().SetString(DTConnected.c_str(), (rapidjson::SizeType)DTConnected.size(), AlreadyConnected.GetAllocator()), AlreadyConnected.GetAllocator());
        AlreadyConnected.AddMember("IPAddress", rapidjson::Value().SetString(TcpClientInfo.IPAddress.c_str(), (rapidjson::SizeType)TcpClientInfo.IPAddress.size(), AlreadyConnected.GetAllocator()), AlreadyConnected.GetAllocator());
        TcpAnswer->Parameters.AddMember("AlreadyConnected", rapidjson::Value(AlreadyConnected, Allocator), Allocator);
    }

    //������������� ����� �������
    TcpMessage->TcpClient->Authorized = true;
    TcpMessage->TcpClient->UserID = UserID;
    TcpMessage->TcpClient->UserSystem = UserSystem;

    //����� ���������� � ������������ � ������������
    TcpAnswer->Parameters.AddMember("UserID", UserID, Allocator);
    TcpAnswer->Parameters.AddMember("UserIsSystem", UserSystem, Allocator);
    TcpAnswer->Parameters.AddMember("UserFIO", rapidjson::Value(UserFIO.c_str(), (rapidjson::SizeType)UserFIO.size()), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);
    
    //���������� � �������
    rapidjson::Document Server(rapidjson::Type::kObjectType);
    Server.AddMember("Version", 1, Server.GetAllocator());
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(Server, Allocator), Allocator);

    //���������� �� ���������� �������
    rapidjson::Document UpdateClient(rapidjson::Type::kObjectType);
    UpdateClient.AddMember("IsNeed", IsNeedUpdate, UpdateClient.GetAllocator());
    UpdateClient.AddMember("NewVersion", 1, UpdateClient.GetAllocator());
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClient, Allocator), Allocator);

    //���������� � ������������
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
