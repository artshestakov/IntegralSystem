#include "ISQueryPool.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISQueryPool::ISQueryPool()
	: ErrorString(NO_ERROR_STRING),
	IsRunning(false),
	IsFinished(false)
{
	
}
//-----------------------------------------------------------------------------
ISQueryPool::~ISQueryPool()
{
	
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
void ISQueryPool::Start()
{
	IsRunning = true;
	std::thread(&ISQueryPool::StartWorker, this).detach();
}
//-----------------------------------------------------------------------------
void ISQueryPool::Shutdown()
{
	if (IsRunning)
	{
		Mutex.lock();
		IsRunning = false;
		Mutex.unlock();

		while (!IsFinished)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		int x = 0;
	}
}
//-----------------------------------------------------------------------------
void ISQueryPool::AddQuery(const QString &SqlText)
{
	Mutex.lock();
	Queue.push(ISQueryPoolObject{ SqlText });
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
void ISQueryPool::AddQuery(const QString &SqlText, const ISStringToVariantMap &Parameters)
{
	Mutex.lock();
	Queue.push(ISQueryPoolObject{ SqlText, Parameters });
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
void ISQueryPool::StartWorker()
{
	ISConnectOptionDB ConnectOption = ISDatabase::Instance().GetOption(CONNECTION_DEFAULT);
	if (!ISDatabase::Instance().Connect(CONNECTION_QUERY_POOL, ConnectOption.Host, ConnectOption.Port, ConnectOption.Name, ConnectOption.Login, ConnectOption.Password))
	{
		ISLOGGER_E(QString("Error create connection \"%1\": %2").arg(CONNECTION_QUERY_POOL).arg(ISDatabase::Instance().GetErrorString()));
		return;
	}

	while (true)
	{
		Mutex.lock();
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
				Query.Execute();
			}
		}
		bool is_running = IsRunning;
		Mutex.unlock();

		if (is_running)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(QUERY_POOL_TIMEOUT));
		}
		else
		{
			break;
		}
	}
	ISDatabase::Instance().Disconnect(CONNECTION_QUERY_POOL);
	IsFinished = true;
}
//-----------------------------------------------------------------------------
