#include "ISQueryPool.h"
#include "ISConstants.h"
#include "ISDatabase.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
ISQueryPool::ISQueryPool()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	FutureWatcher(new QFutureWatcher<void>(this))
{
	QTimer *Timer = new QTimer(this);
	Timer->setInterval(QUERY_POOL_TIMEOUT);
	connect(Timer, &QTimer::timeout, this, &ISQueryPool::StartExecuting);
	Timer->start();
}
//-----------------------------------------------------------------------------
ISQueryPool::~ISQueryPool()
{
	ISDatabase::Instance().Disconnect(CONNECTION_QUERY_POOL);
}
//-----------------------------------------------------------------------------
ISQueryPool& ISQueryPool::Instance()
{
	static ISQueryPool QueryThreader;
	return QueryThreader;
}
//-----------------------------------------------------------------------------
QString ISQueryPool::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISQueryPool::Initialize()
{
	ISConnectOptionDB ConnectOption = ISDatabase::Instance().GetOption(CONNECTION_DEFAULT);
	bool Result = ISDatabase::Instance().Connect(CONNECTION_QUERY_POOL, ConnectOption.Host, ConnectOption.Port, ConnectOption.Name, ConnectOption.Login, ConnectOption.Password);
	if (!Result)
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISQueryPool::AddQuery(const QString &SqlText)
{
	Queue.enqueue(ISQueryPoolObject(SqlText));
}
//-----------------------------------------------------------------------------
void ISQueryPool::AddQuery(const QString &SqlText, const QVariantMap &Parameters)
{
	ISQueryPoolObject QueryPoolObject(SqlText);
	for (const auto &MapItem : Parameters.toStdMap())
	{
		QueryPoolObject.Parameters.emplace(MapItem.first, MapItem.second);
	}
	Queue.push_front(QueryPoolObject);
}
//-----------------------------------------------------------------------------
void ISQueryPool::StartExecuting()
{
	if (Queue.count()) //Если в очереди есть запросы
	{
		if (FutureWatcher->isRunning()) //Если в данный момент времени происходит отработка запроса, выходить из функции
		{
			return;
		}

		QFuture<void> Future = QtConcurrent::run(this, &ISQueryPool::ExecuteQuery);
		FutureWatcher->setFuture(Future);
	}
}
//-----------------------------------------------------------------------------
void ISQueryPool::ExecuteQuery()
{
	if (Queue.count())
	{
		ISQueryPoolObject QueryPoolObject = Queue.dequeue();
		{
			ISQuery Query(ISDatabase::Instance().GetDB(CONNECTION_QUERY_POOL), QueryPoolObject.SqlText);
			for (const auto &MapItem : QueryPoolObject.Parameters)
			{
				Query.BindValue(MapItem.first, MapItem.second);
			}
			Query.Execute();
		}
	}
}
//-----------------------------------------------------------------------------
