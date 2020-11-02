#include "ISQueryPool.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
ISQueryPool::ISQueryPool()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	Port(0),
	IsRunning(false),
	IsFinished(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISQueryPool::~ISQueryPool()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISQueryPool& ISQueryPool::Instance()
{
	static ISQueryPool QueryPool;
	return QueryPool;
}
//-----------------------------------------------------------------------------
QString ISQueryPool::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISQueryPool::Start(const QString &host, unsigned short port, const QString &database, const QString &login, const QString &password)
{
	Host = host;
	Port = port;
	Database = database;
	Login = login;
	Password = password;

	QEventLoop EventLoop;
	connect(this, &ISQueryPool::Runned, &EventLoop, &QEventLoop::quit);

	std::thread(&ISQueryPool::StartWorker, this).detach();
	EventLoop.exec();
	return IsRunning;
}
//-----------------------------------------------------------------------------
void ISQueryPool::Shutdown()
{
	if (IsRunning)
	{
		CRITICAL_SECTION_LOCK(&CriticalSection);
		IsRunning = false;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		while (!IsFinished)
		{
			ISSleep(1);
		}
	}
}
//-----------------------------------------------------------------------------
void ISQueryPool::AddQuery(const QString &SqlText, const ISStringToVariantMap &Parameters = ISStringToVariantMap())
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Queue.push(ISQueryPoolObject{ SqlText, Parameters });
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISQueryPool::StartWorker()
{
	IsRunning = ISDatabase::Instance().Connect(CONNECTION_QUERY_POOL, Host, Port, Database, Login, Password);
	if (!IsRunning)
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
	}
	emit Runned();

	if (IsRunning)
	{
		while (true)
		{
			CRITICAL_SECTION_LOCK(&CriticalSection);
			while (!Queue.empty())
			{
				ISQueryPoolObject QueryPoolObject = Queue.front();
				Queue.pop();
				{
					ISQuery Query(ISDatabase::Instance().GetDB(CONNECTION_QUERY_POOL), QueryPoolObject.SqlText);
					for (const auto &MapItem : QueryPoolObject.Parameters)
					{
						Query.BindValue(MapItem.first, MapItem.second);
					}
					if (!Query.Execute())
					{
						ISLOGGER_E(__CLASS__, QString("Query %1: %2").arg(QueryPoolObject.SqlText.simplified()).arg(Query.GetErrorString()));
					}
				}
			}
			bool is_running = IsRunning;
			CRITICAL_SECTION_UNLOCK(&CriticalSection);
			if (!is_running)
			{
				ISDatabase::Instance().Disconnect(CONNECTION_QUERY_POOL);
				break;
			}
			ISSleep(10);
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
