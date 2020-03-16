#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISQuery.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISCountingTime.h"
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
	//DefaultDB = QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, CONNECTION_DEFAULT);
	//SystemDB = QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, CONNECTION_SYSTEM);
}
//-----------------------------------------------------------------------------
ISDatabase::~ISDatabase()
{

}
//-----------------------------------------------------------------------------
ISDatabase& ISDatabase::GetInstance()
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
void ISDatabase::SetErrorString(const QString &error_string)
{
	ErrorString = error_string;
}
//-----------------------------------------------------------------------------
QSqlDatabase& ISDatabase::GetDefaultDB()
{
	return DefaultDB;
}
//-----------------------------------------------------------------------------
QSqlDatabase& ISDatabase::GetSystemDB()
{
	return SystemDB;
}
//-----------------------------------------------------------------------------
QSqlDatabase& ISDatabase::GetDB(const QString &ConnectionName)
{
	if (ConnectionName.isEmpty())
	{
		return Connections[CONNECTION_DEFAULT];
	}
	return Connections[ConnectionName];
}
//-----------------------------------------------------------------------------
bool ISDatabase::CheckExistDatabase(const QString &ConnectionName, const QString &Database, bool &Exist)
{
	ISQuery qSelectDatabase(Connections[ConnectionName], QS_DATABASE);
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
QString ISDatabase::GetVersionPostgres()
{
	ISQuery qSelectVersion(QS_VERSION_POSTGRESQL);
	if (qSelectVersion.ExecuteFirst())
	{
		return qSelectVersion.ReadColumn("version").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetCurrentDatabaseSize()
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
QString ISDatabase::GetLoadConfigurationTime()
{
	ISQuery qSelect(QS_LOAD_CONFIGURATION_TIME);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("pg_conf_load_time").toDateTime().toString(FORMAT_DATE_TIME_V3);
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetClientAddress()
{
	ISQuery qSelect(QS_INET_CLIENT_ADDRESS);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("inet_client_addr").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetServerAddress()
{
	ISQuery qSelect(QS_INET_SERVER_ADDRESS);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("inet_server_addr").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
int ISDatabase::GetServerProcessID()
{
	ISQuery qSelect(QS_BACKEND_PID);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("pg_backend_pid").toInt();
	}
	return 0;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCollate()
{
	ISQuery qSelect(QS_LC_COLLATE);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("datcollate").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCType()
{
	ISQuery qSelect(QS_C_TYPE);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("datctype").toString();
	}
	return QString();
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseDataDirectory()
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
QVariant ISDatabase::GetValue(const QString &TableName, const QString &FieldName, int ObjectID)
{
	PMetaTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	PMetaField *MetaField = MetaTable->GetField(FieldName);
	QString SqlText = "SELECT " + MetaTable->Alias + '_' + MetaField->Name + " FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id = :ObjectID";
	ISQuery qSelect(SqlText);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn(MetaTable->Alias + '_' + MetaField->Name);
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToDefaultDB(const QString &Login, const QString &Password, QString &ErrorConnection)
{
	bool Result = ConnectToDatabase(DefaultDB, Login, Password, CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), ErrorConnection);
	if (Result)
	{
		QSqlQuery SqlQuery = DefaultDB.exec("SET application_name = '" + ISDefines::Core::APPLICATION_NAME + "'");
		Result = SqlQuery.lastError().type() == QSqlError::NoError;
		if (!Result)
		{
			ErrorConnection = SqlQuery.lastError().text();
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToSystemDB(QString &ErrorConnection)
{
	return ConnectToDatabase(SystemDB, CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), SYSTEM_DATABASE_NAME, ErrorConnection);
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection)
{
	bool Result = SqlDatabase.isOpen();
	if (!Result)
	{
		SqlDatabase.setHostName(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER));
		SqlDatabase.setPort(CONFIG_INT(CONST_CONFIG_CONNECTION_PORT));
		SqlDatabase.setDatabaseName(Database);
		SqlDatabase.setUserName(Login);
		SqlDatabase.setPassword(Password);
		Result = SqlDatabase.open();
		if (!Result)
		{
			ErrorConnection = SqlDatabase.lastError().text();
			ISLOGGER_WARNING(QString("Connection to database \"%1\" failed. Error: %2").arg(Database).arg(ErrorConnection));
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const QString &ConnectionName, const QString &Host, int Port, const QString &Database, const QString &Login, const QString &Password)
{
	QSqlDatabase SqlDatabase;
	bool Result = Connections.count(ConnectionName) > 0;
	if (Result)
	{
		SqlDatabase = Connections[ConnectionName];
	}
	else
	{
		SqlDatabase = QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, ConnectionName);
		Result = SqlDatabase.isValid();
		if (Result) //Если при добавлении БД возникла ошибка
		{
			Connections.emplace(ConnectionName, SqlDatabase);
		}
		else
		{
			ErrorString = SqlDatabase.lastError().text();
		}
	}

	if (Result)
	{
		SqlDatabase.setHostName(Host);
		SqlDatabase.setPort(Port);
		SqlDatabase.setDatabaseName(Database);
		SqlDatabase.setUserName(Login);
		SqlDatabase.setPassword(Password);
		Result = SqlDatabase.open();
		if (!Result)
		{
			ErrorString = QString("Error connect to db \"%1\": %2").arg(Database).arg(SqlDatabase.lastError().text());
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISDatabase::Disconnect(const QString &ConnectionName)
{
	Connections[ConnectionName].close();
	Connections[ConnectionName].setHostName(QString());
	Connections[ConnectionName].setPort(-1);
	Connections[ConnectionName].setDatabaseName(QString());
	Connections[ConnectionName].setUserName(QString());
	Connections[ConnectionName].setPassword(QString());
	QSqlDatabase::removeDatabase(ConnectionName);
	Connections.erase(ConnectionName);
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectFromDefaultDB()
{
	DisconnectFromDatabase(DefaultDB);
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectFromSystemDB()
{
	DisconnectFromDatabase(SystemDB);
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectFromDatabase(QSqlDatabase &SqlDatabase)
{
	SqlDatabase.close();
	SqlDatabase.setHostName(QString());
	SqlDatabase.setPort(-1);
	SqlDatabase.setDatabaseName(QString());
	SqlDatabase.setUserName(QString());
	SqlDatabase.setPassword(QString());
}
//-----------------------------------------------------------------------------
