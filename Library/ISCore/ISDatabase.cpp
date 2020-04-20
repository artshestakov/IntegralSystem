#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISMetaData.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
static QString QS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) "
                                           "FROM pg_database "
                                           "WHERE datname = :DatabaseName");
//-----------------------------------------------------------------------------
static QString QS_VERSION_POSTGRESQL = PREPARE_QUERY("SELECT version()");
//-----------------------------------------------------------------------------
static QString QS_DATABASE_SIZE = PREPARE_QUERY("SELECT databasesize()");
//-----------------------------------------------------------------------------
static QString QS_TABLE_SIZE = PREPARE_QUERY("SELECT pg_size_pretty(pg_relation_size(:TableName))");
//-----------------------------------------------------------------------------
static QString QS_TABLE_SIZE_WITH_INDICES = PREPARE_QUERY("SELECT pg_size_pretty(pg_total_relation_size(:TableName))");
//-----------------------------------------------------------------------------
static QString QS_START_TIME_SERVER = PREPARE_QUERY("SELECT pg_postmaster_start_time()");
//-----------------------------------------------------------------------------
static QString QS_LOAD_CONFIGURATION_TIME = PREPARE_QUERY("SELECT pg_conf_load_time()");
//-----------------------------------------------------------------------------
static QString QS_INET_CLIENT_ADDRESS = PREPARE_QUERY("SELECT inet_client_addr()");
//-----------------------------------------------------------------------------
static QString QS_INET_SERVER_ADDRESS = PREPARE_QUERY("SELECT inet_server_addr()");
//-----------------------------------------------------------------------------
static QString QS_BACKEND_PID = PREPARE_QUERY("SELECT pg_backend_pid()");
//-----------------------------------------------------------------------------
static QString QS_LC_COLLATE = PREPARE_QUERY("SELECT datcollate FROM pg_database WHERE datname = current_database()");
//-----------------------------------------------------------------------------
static QString QS_C_TYPE = PREPARE_QUERY("SELECT datctype FROM pg_database WHERE datname = current_database()");
//-----------------------------------------------------------------------------
static QString QS_DATA_DIRECTORY = PREPARE_QUERY("SELECT setting FROM pg_settings WHERE name = 'data_directory'");
//-----------------------------------------------------------------------------
static QString QS_USER_ONLINE_FROM_ID = PREPARE_QUERY("SELECT useronline(userlogin(:UserID))");
//-----------------------------------------------------------------------------
static QString QS_USER_ONLINE_FROM_LOGIN = PREPARE_QUERY("SELECT useronline(:UserLogin)");
//-----------------------------------------------------------------------------
ISDatabase::ISDatabase()
    : ErrorString(NO_ERROR_STRING)
{

}
//-----------------------------------------------------------------------------
ISDatabase::~ISDatabase()
{

}
//-----------------------------------------------------------------------------
ISDatabase& ISDatabase::Instance()
{
    static ISDatabase Database;
    return Database;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
QSqlDatabase ISDatabase::GetDB(const QString &ConnectionName)
{
    if (QSqlDatabase::contains(ConnectionName))
    {
        return QSqlDatabase::database(ConnectionName);
    }
    return QSqlDatabase();
}
//-----------------------------------------------------------------------------
ISConnectOptionDB ISDatabase::GetOption(const QString &ConnectionName)
{
    ISConnectOptionDB ConnectOption;
    if (ConnectOptions.count(ConnectionName))
    {
        ConnectOption.Host = ConnectOptions[ConnectionName].Host;
        ConnectOption.Port = ConnectOptions[ConnectionName].Port;
        ConnectOption.Name = ConnectOptions[ConnectionName].Name;
        ConnectOption.Login = ConnectOptions[ConnectionName].Login;
        ConnectOption.Password = ConnectOptions[ConnectionName].Password;
    }
    return ConnectOption;
}
//-----------------------------------------------------------------------------
bool ISDatabase::CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist)
{
    ISQuery qSelectDatabase(GetDB(ConnectionName), QS_DATABASE);
    qSelectDatabase.SetShowLongQuery(false);
    qSelectDatabase.BindValue(":DatabaseName", Database);
    bool Result = qSelectDatabase.ExecuteFirst();
    if (Result)
    {
        Exist = qSelectDatabase.ReadColumn("count").toBool();
    }
    return Result;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetVersionPostgres() const
{
    ISQuery qSelectVersion(QS_VERSION_POSTGRESQL);
    if (qSelectVersion.ExecuteFirst())
    {
        return qSelectVersion.ReadColumn("version").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetCurrentDatabaseSize() const
{
    ISQuery qSelectDatabaseSize(QS_DATABASE_SIZE);
    qSelectDatabaseSize.SetShowLongQuery(false);
    if (qSelectDatabaseSize.ExecuteFirst())
    {
        return qSelectDatabaseSize.ReadColumn("databasesize").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetTableSize(const QString &TableName) const
{
    ISQuery qSelect(QS_TABLE_SIZE);
    qSelect.BindValue(":TableName", TableName);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("pg_size_pretty").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetTableSizeWithIndices(const QString &TableName) const
{
    ISQuery qSelect(QS_TABLE_SIZE_WITH_INDICES);
    qSelect.BindValue(":TableName", TableName);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("pg_size_pretty").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
int ISDatabase::GetTableRowCount(const QString &TableName) const
{
    ISQuery qSelect("SELECT COUNT(*) FROM " + TableName);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("count").toInt();
    }
    return 0;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetStartTimeServer() const
{
    ISQuery qSelect(QS_START_TIME_SERVER);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("pg_postmaster_start_time").toDateTime().toString(FORMAT_DATE_TIME_V3);
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetLoadConfigurationTime() const
{
    ISQuery qSelect(QS_LOAD_CONFIGURATION_TIME);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("pg_conf_load_time").toDateTime().toString(FORMAT_DATE_TIME_V3);
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetClientAddress() const
{
    ISQuery qSelect(QS_INET_CLIENT_ADDRESS);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("inet_client_addr").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetServerAddress() const
{
    ISQuery qSelect(QS_INET_SERVER_ADDRESS);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("inet_server_addr").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
int ISDatabase::GetServerProcessID() const
{
    ISQuery qSelect(QS_BACKEND_PID);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("pg_backend_pid").toInt();
    }
    return 0;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCollate() const
{
    ISQuery qSelect(QS_LC_COLLATE);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("datcollate").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCType() const
{
    ISQuery qSelect(QS_C_TYPE);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("datctype").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseDataDirectory() const
{
    ISQuery qSelect(QS_DATA_DIRECTORY);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn("setting").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetAge(const QDateTime &DateTime) const
{
    ISQuery qAge(QString("SELECT age(to_timestamp('%1', 'DD.MM.YYYY hh24:mi:ss'))").arg(DateTime.toString(FORMAT_DATE_TIME_V4)));
    qAge.SetShowLongQuery(false);
    if (qAge.ExecuteFirst())
    {
        return qAge.ReadColumn("age").toString();
    }
    return QString();
}
//-----------------------------------------------------------------------------
bool ISDatabase::IsUserOnline(int UserID) const
{
    ISQuery qSelect(QS_USER_ONLINE_FROM_ID);
    qSelect.BindValue(":UserID", UserID);
    bool Result = qSelect.ExecuteFirst();
    if (Result)
    {
        Result = qSelect.ReadColumn("useronline").toBool();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISDatabase::IsUserOnline(const QString &UserLogin) const
{
    ISQuery qSelect(QS_USER_ONLINE_FROM_LOGIN);
    qSelect.BindValue(":UserLogin", UserLogin);
    bool Result = qSelect.ExecuteFirst();
    if (Result)
    {
        Result = qSelect.ReadColumn("useronline").toBool();
    }
    return Result;
}
//-----------------------------------------------------------------------------
QVariant ISDatabase::GetValue(const QString &TableName, const QString &FieldName, int ObjectID) const
{
    PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
    PMetaField *MetaField = MetaTable->GetField(FieldName);
    QString SqlText = "SELECT " + MetaTable->Alias + '_' + MetaField->Name.toLower() + " FROM " + MetaTable->Name.toLower() + " WHERE " + MetaTable->Alias + "_id = :ObjectID";
    ISQuery qSelect(SqlText);
    qSelect.BindValue(":ObjectID", ObjectID);
    if (qSelect.ExecuteFirst())
    {
        return qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name.toLower());
    }
    return QVariant();
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password)
{
    //Если объект БД с таким именем соединения уже существует - то забираем
    //его из внутренней памяти QSqlDatabase, иначе - добавляем
    QSqlDatabase SqlDatabase = QSqlDatabase::contains(ConnectionName) ?
                QSqlDatabase::database(ConnectionName) :
                QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, ConnectionName);
    bool Result = SqlDatabase.isValid();
    if (!Result) //Ошибка при добавлении нового подключения в память
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return Result;
    }

    //Заполняем параметры подключения
    SqlDatabase.setHostName(Host);
    SqlDatabase.setPort(Port);
    SqlDatabase.setDatabaseName(Database);
    SqlDatabase.setUserName(Login);
    SqlDatabase.setPassword(Password);

    Result = SqlDatabase.open();
    if (!Result) //Ошибка при подключении
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return Result;
    }

    ConnectOptions.emplace(ConnectionName, ISConnectOptionDB{ Host, Port, Database, Login, Password });

    //Изменяем имя приложения для коннекта
    QSqlError SqlError = SqlDatabase.exec("SET application_name = '" + ISDefines::Core::APPLICATION_NAME + "'").lastError();
    if (SqlError.type() != QSqlError::NoError) //Не удалось изменить имя приложенения
    {
        ErrorString = "Not change application_name: " + SqlError.databaseText();
        return Result;
    }

    return Result;
}
//-----------------------------------------------------------------------------
void ISDatabase::Disconnect(const QString &ConnectionName)
{
    bool Contains = true; //Флаг удаления экземпляра БД из памяти
    {
        Contains = QSqlDatabase::contains(ConnectionName);
        if (Contains) //Если БД существует - проводим процедуру отключения от неё
        {
            QSqlDatabase SqlDatabase = QSqlDatabase::database(ConnectionName);
            if (SqlDatabase.isOpen())
            {
                SqlDatabase.close();
                SqlDatabase.setHostName(QString());
                SqlDatabase.setPort(0);
                SqlDatabase.setDatabaseName(QString());
                SqlDatabase.setUserName(QString());
                SqlDatabase.setPassword(QString());
            }
        }
    }

    if (Contains)
    {
        QSqlDatabase::removeDatabase(ConnectionName);
        ConnectOptions.erase(ConnectionName);
    }
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectAll()
{
    for (const QString &ConnectionName : QSqlDatabase::connectionNames())
    {
        if (GetDB(ConnectionName).isOpen())
        {
            Disconnect(ConnectionName);
        }
    }
}
//-----------------------------------------------------------------------------
