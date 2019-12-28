#include "StdAfx.h"
#include "ISDatabase.h"
#include "ISDefines.h"
#include "ISConfig.h"
#include "ISDebug.h"
#include "ISQuery.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISCountingTime.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
static QString QS_DATABASE = PREPARE_QUERY("SELECT COUNT(*) FROM pg_database WHERE datname = :DatabaseName");
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
ISDatabase::ISDatabase() : QObject()
{
	DefaultDB = QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, "DefaultDB");
	SystemDB = QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, "SystemDB");
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
bool ISDatabase::CheckExistDatabase(const QString &Database)
{
	bool Result = false;
	ISQuery qSelectDatabase(SystemDB, QS_DATABASE);
	qSelectDatabase.SetShowLongQuery(false);
	qSelectDatabase.BindValue(":DatabaseName", Database);
	if (qSelectDatabase.ExecuteFirst())
	{
		int Count = qSelectDatabase.ReadColumn("count").toInt();
		if (Count)
		{
			Result = true;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetVersionPostgres()
{
	QString Version;
	ISQuery qSelectVersion(QS_VERSION_POSTGRESQL);
	if (qSelectVersion.ExecuteFirst())
	{
		Version = qSelectVersion.ReadColumn("version").toString();
	}
	return Version;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetCurrentDatabaseSize()
{
	QString DatabaseSize;
	ISQuery qSelectDatabaseSize(QS_DATABASE_SIZE);
	qSelectDatabaseSize.SetShowLongQuery(false);
	if (qSelectDatabaseSize.ExecuteFirst())
	{
		DatabaseSize = qSelectDatabaseSize.ReadColumn("databasesize").toString();
	}
	return DatabaseSize;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetTableSize(const QString &TableName) const
{
	QString TableSize;
	ISQuery qSelect(QS_TABLE_SIZE);
	qSelect.BindValue(":TableName", TableName);
	if (qSelect.ExecuteFirst())
	{
		TableSize = qSelect.ReadColumn("pg_size_pretty").toString();
	}
	return TableSize;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetTableSizeWithIndices(const QString &TableName) const
{
	QString TableSize;
	ISQuery qSelect(QS_TABLE_SIZE_WITH_INDICES);
	qSelect.BindValue(":TableName", TableName);
	if (qSelect.ExecuteFirst())
	{
		TableSize = qSelect.ReadColumn("pg_size_pretty").toString();
	}
	return TableSize;
}
//-----------------------------------------------------------------------------
int ISDatabase::GetTableRowCount(const QString &TableName) const
{
	int RowCount = 0;
	ISQuery qSelect("SELECT COUNT(*) FROM " + TableName);
	if (qSelect.ExecuteFirst())
	{
		RowCount = qSelect.ReadColumn("count").toInt();
	}
	return RowCount;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetStartTimeServer() const
{
	QString StartTime;
	ISQuery qSelect(QS_START_TIME_SERVER);
	if (qSelect.ExecuteFirst())
	{
		StartTime = qSelect.ReadColumn("pg_postmaster_start_time").toDateTime().toString(DATE_TIME_FORMAT_V3);
	}
	return StartTime;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetLoadConfigurationTime()
{
	QString LoadConfiguration;
	ISQuery qSelect(QS_LOAD_CONFIGURATION_TIME);
	if (qSelect.ExecuteFirst())
	{
		LoadConfiguration = qSelect.ReadColumn("pg_conf_load_time").toDateTime().toString(DATE_TIME_FORMAT_V3);
	}
	return LoadConfiguration;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetClientAddress()
{
	QString Address;
	ISQuery qSelect(QS_INET_CLIENT_ADDRESS);
	if (qSelect.ExecuteFirst())
	{
		Address = qSelect.ReadColumn("inet_client_addr").toString();
	}
	return Address;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetInetServerAddress()
{
	QString Address;
	ISQuery qSelect(QS_INET_SERVER_ADDRESS);
	if (qSelect.ExecuteFirst())
	{
		Address = qSelect.ReadColumn("inet_server_addr").toString();
	}
	return Address;
}
//-----------------------------------------------------------------------------
int ISDatabase::GetServerProcessID()
{
	int ID = 0;
	ISQuery qSelect(QS_BACKEND_PID);
	if (qSelect.ExecuteFirst())
	{
		ID = qSelect.ReadColumn("pg_backend_pid").toInt();
	}
	return ID;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCollate()
{
	QString LCCollate;
	ISQuery qSelect(QS_LC_COLLATE);
	if (qSelect.ExecuteFirst())
	{
		LCCollate = qSelect.ReadColumn("datcollate").toString();
	}
	return LCCollate;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseCType()
{
	QString CType;
	ISQuery qSelect(QS_C_TYPE);
	if (qSelect.ExecuteFirst())
	{
		CType = qSelect.ReadColumn("datctype").toString();
	}
	return CType;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetDatabaseDataDirectory()
{
	QString DataDirectory;
	ISQuery qSelect(QS_DATA_DIRECTORY);
	if (qSelect.ExecuteFirst())
	{
		DataDirectory = qSelect.ReadColumn("setting").toString();
	}
	return DataDirectory;
}
//-----------------------------------------------------------------------------
QString ISDatabase::GetAge(const QDateTime &DateTime) const
{
	QString Age;
	ISQuery qAge(QString("SELECT age(to_timestamp('%1', 'DD.MM.YYYY hh24:mi:ss'))").arg(DateTime.toString(DATE_TIME_FORMAT_V4)));
	qAge.SetShowLongQuery(false);
	if (qAge.ExecuteFirst())
	{
		Age = qAge.ReadColumn("age").toString();
	}
	return Age;
}
//-----------------------------------------------------------------------------
bool ISDatabase::IsUserOnline(int UserID) const
{
	bool Result = false;
	ISQuery qSelect(QS_USER_ONLINE_FROM_ID);
	qSelect.BindValue(":UserID", UserID);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("useronline").toBool();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISDatabase::IsUserOnline(const QString &UserLogin) const
{
	bool Result = false;
	ISQuery qSelect(QS_USER_ONLINE_FROM_LOGIN);
	qSelect.BindValue(":UserLogin", UserLogin);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("useronline").toBool();
	}
	return Result;
}
//-----------------------------------------------------------------------------
QVariant ISDatabase::GetValue(const QString &TableName, const QString &FieldName, int ObjectID)
{
	QVariant Value;
	PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetMetaTable(TableName);
	PMetaClassField *MetaField = MetaTable->GetField(FieldName);
	QString SqlText = "SELECT " + MetaTable->GetAlias() + "_" + MetaField->GetName() + " FROM " + MetaTable->GetName() + " WHERE " + MetaTable->GetAlias() + "_id = :ObjectID";
	ISQuery qSelect(SqlText);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (qSelect.ExecuteFirst())
	{
		Value = qSelect.ReadColumn(MetaTable->GetAlias() + "_" + MetaField->GetName());
	}
	return Value;
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToDefaultDB(const QString &Login, const QString &Password, QString &ErrorConnection)
{
	return ConnectToDatabase(DefaultDB, Login, Password, CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE), ErrorConnection);
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToSystemDB(QString &ErrorConnection)
{
	return ConnectToDatabase(SystemDB, CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD), SYSTEM_DATABASE_NAME, ErrorConnection);
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
	QString DatabaseName = SqlDatabase.databaseName();
	ISDebug::ShowDebugString(QString("Disconnect from database %1...").arg(DatabaseName));

	ISCountingTime CountingTime;
	SqlDatabase.close();

	SqlDatabase.setHostName(QString());
	SqlDatabase.setPort(-1);
	SqlDatabase.setDatabaseName(QString());
	SqlDatabase.setUserName(QString());
	SqlDatabase.setPassword(QString());

	ISDebug::ShowInfoString(QString("Disconnect from database \"%1\" done. %2 msec").arg(DatabaseName).arg(CountingTime.GetElapsed()));
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectToDatabase(QSqlDatabase &SqlDatabase, const QString &Login, const QString &Password, const QString &Database, QString &ErrorConnection)
{
	bool Result = SqlDatabase.isOpen();

	if (!Result)
	{
		QString Server = ISConfig::GetInstance().GetValue(CONST_CONFIG_CONNECTION_SERVER).toString();
		int Port = ISConfig::GetInstance().GetValue(CONST_CONFIG_CONNECTION_PORT).toInt();

		SqlDatabase.setHostName(Server);
		SqlDatabase.setPort(Port);
		SqlDatabase.setDatabaseName(Database);
		SqlDatabase.setUserName(Login);
		SqlDatabase.setPassword(Password);

		ISCountingTime CountingTime;
		ISDebug::ShowInfoString(QString("Connecting to database %1...").arg(Database));

		Result = SqlDatabase.open();
		if (Result)
		{
			ISDebug::ShowInfoString(QString("Connection to database \"%1\" done. %2 msec").arg(Database).arg(CountingTime.GetElapsed()));
		}
		else
		{
			ErrorConnection = SqlDatabase.lastError().text();
			ISDebug::ShowWarningString(QString("Connection to database \"%1\" failed. Error: %2").arg(Database).arg(ErrorConnection));
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
