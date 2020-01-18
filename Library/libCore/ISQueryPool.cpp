#include "ISQueryPool.h"
#include "ISDefines.h"
#include "ISDatabase.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
ISQueryPool::ISQueryPool(QObject *parent) : QObject(parent)
{
	DB = QSqlDatabase::cloneDatabase(ISDatabase::GetInstance().GetDefaultDB(), DATABASE_CONNECTON_THREAD_QUERY);
	FutureWatcher = new QFutureWatcher<void>(this);

	QTimer *Timer = new QTimer(this);
	Timer->setInterval(2000);
	connect(Timer, &QTimer::timeout, this, &ISQueryPool::StartExecuting);
	Timer->start();
}
//-----------------------------------------------------------------------------
ISQueryPool::~ISQueryPool()
{

}
//-----------------------------------------------------------------------------
ISQueryPool& ISQueryPool::GetInstance()
{
	static ISQueryPool QueryThreader;
	return QueryThreader;
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
		QueryPoolObject.AddBindValue(MapItem.first, MapItem.second);
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
		if (DB.open())
		{
			ISQueryPoolObject QueryPoolObject = Queue.dequeue();

			{
				ISQuery Query(DB, QueryPoolObject.GetSqlText());

				if (QueryPoolObject.GetParameters().count())
				{
					for (const auto &MapItem : QueryPoolObject.GetParameters().toStdMap())
					{
						Query.BindValue(MapItem.first, MapItem.second);
					}
				}

				Query.Execute();
			}

			DB.close();
		}
	}
}
//-----------------------------------------------------------------------------
