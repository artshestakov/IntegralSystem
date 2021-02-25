#include "ISDatabase.h"
//-----------------------------------------------------------------------------
ISDatabase::ISDatabase()
    : ErrorString(NO_ERROR_STRING)
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
PGconn* ISDatabase::GetDBLibPQ(const std::string &ConnectionName)
{
	PGconn *Connection = NULL;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = ConnectionsLibPQ.find(ConnectionName);
	if (It != ConnectionsLibPQ.end())
	{
		Connection = It->second;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return Connection;
}
//-----------------------------------------------------------------------------
ISConnectOptionDB ISDatabase::GetOption(const QString &ConnectionName)
{
    ISConnectOptionDB ConnectOption;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (ConnectOptions.count(ConnectionName)) //���� ���������� � ����� ������ ���� - ��������� ��������������
	{
		ConnectOption = ConnectOptions[ConnectionName];
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return ConnectOption;
}
//-----------------------------------------------------------------------------
ISConnectOptionDB ISDatabase::GetOptionLibPQ(const std::string &ConnectionName)
{
	ISConnectOptionDB ConnectOption;
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = ConnectionsLibPQ.find(ConnectionName);
	if (It != ConnectionsLibPQ.end())
	{
		ConnectOption.Host = PQhost(It->second);
		ConnectOption.Port = atoi(PQport(It->second));
		ConnectOption.Name = PQdb(It->second);
		ConnectOption.Login = PQuser(It->second);
		ConnectOption.Password = PQpass(It->second);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return ConnectOption;
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectLibPQ(const std::string &ConnectionName, const ISConnectOptionDB &ConnectOptionDB)
{
	return ConnectLibPQ(ConnectionName, ConnectOptionDB.Host, ConnectOptionDB.Port, ConnectOptionDB.Name, ConnectOptionDB.Login, ConnectOptionDB.Password);
}
//-----------------------------------------------------------------------------
bool ISDatabase::ConnectLibPQ(const std::string &ConnectionName, const QString &Host, unsigned short Port, const QString &Database, const QString &Login, const QString &Password)
{
	//��������� ������ �����������
	std::stringstream StringStream;
	StringStream << "host=" << Host.toStdString() << ' ';
	StringStream << "port=" << std::to_string(Port) << ' ';
	StringStream << "dbname=" << Database.toStdString() << ' ';
	StringStream << "user=" << Login.toStdString() << ' ';
	StringStream << "password=" << Password.toStdString() << ' ';
	StringStream << "application_name=" << QCoreApplication::applicationName().toStdString();
	std::string String = StringStream.str(); //����� ������ ����� �����������
	const char *ConnectionInfo = String.c_str();

	//������� ������
	PGPing Ping = PQping(ConnectionInfo);
	if (Ping != PQPING_OK) //���-�� �� ���
	{
		switch (Ping)
		{
		case PGPing::PQPING_REJECT: ErrorString = "The server reject connection"; break;
		case PGPing::PQPING_NO_RESPONSE: ErrorString = "The server no responce"; break;
		case PGPing::PQPING_NO_ATTEMPT: ErrorString = "No contact was made with the server: invalid connection parameters or out of memory"; break;
		default:
			break;
		}
		return false;
	}

	//������������
	PGconn *Connection = PQconnectdb(ConnectionInfo);
	if (PQstatus(Connection) != CONNECTION_OK) //���-�� �� ���
	{
		PQfinish(Connection);
		ErrorString = QString::fromLocal8Bit(PQerrorMessage(Connection));
		return false;
	}

	//��������� ��������� �� ���������� � ������
	CRITICAL_SECTION_LOCK(&CriticalSection);
	ConnectionsLibPQ.emplace(ConnectionName, Connection);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return true;
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectLibPQ(const std::string &ConnectionName)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	auto It = ConnectionsLibPQ.find(ConnectionName);
	if (It != ConnectionsLibPQ.end()) //����� ����������
	{
		PQfinish(It->second);
		It->second = NULL;
		ConnectionsLibPQ.erase(It);
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISDatabase::DisconnectAllLibPQ()
{
	std::vector<std::string> Keys = ISAlgorithm::ConvertMapToKeys(ConnectionsLibPQ);
	for (const std::string &ConnectionName : Keys)
	{
		DisconnectLibPQ(ConnectionName);
	}
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const QString &ConnectionName, const ISConnectOptionDB &ConnectOptionDB)
{
	return Connect(ConnectionName, ConnectOptionDB.Host, ConnectOptionDB.Port, ConnectOptionDB.Name, ConnectOptionDB.Login, ConnectOptionDB.Password);
}
//-----------------------------------------------------------------------------
bool ISDatabase::Connect(const QString &ConnectionName, const QString &Host, unsigned short Port, const QString &Database, const QString &Login, const QString &Password)
{
    //���� ������ �� � ����� ������ ���������� ��� ���������� - �� ��������
    //��� �� ���������� ������ QSqlDatabase, ����� - ���������
    QSqlDatabase SqlDatabase = QSqlDatabase::contains(ConnectionName) ?
                QSqlDatabase::database(ConnectionName) :
                QSqlDatabase::addDatabase(SQL_DRIVER_QPSQL, ConnectionName);
    if (!SqlDatabase.isValid()) //������ ��� ���������� ������ ����������� � ������
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return false;
    }

    //��������� ��������� � ������������
    SqlDatabase.setHostName(Host);
    SqlDatabase.setPort(Port);
    SqlDatabase.setDatabaseName(Database);
    SqlDatabase.setUserName(Login);
    SqlDatabase.setPassword(Password);
    if (!SqlDatabase.open()) //������ ��� �����������
    {
        ErrorString = SqlDatabase.lastError().databaseText().simplified();
        return false;
    }

	CRITICAL_SECTION_LOCK(&CriticalSection);
    ConnectOptions.emplace(ConnectionName, ISConnectOptionDB(Host, Port, Database, Login, Password));
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return true;
}
//-----------------------------------------------------------------------------
void ISDatabase::Disconnect(const QString &ConnectionName)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
    bool Contains = true; //���� �������� ���������� �� �� ������
    {
        Contains = QSqlDatabase::contains(ConnectionName);
        if (Contains) //���� �� ���������� - �������� ��������� ���������� �� ��
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
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
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
