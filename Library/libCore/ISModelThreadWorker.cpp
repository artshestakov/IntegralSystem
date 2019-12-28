#include "StdAfx.h"
#include "ISModelThreadWorker.h"
#include "ISDefines.h"
#include "ISDatabase.h"
#include "ISCountingTime.h"
#include "ISDebug.h"
#include "ISAssert.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISModelThreadWorker::ISModelThreadWorker(QObject *parent) : QObject(parent)
{
	ConnectionName = ISSystem::GenerateUuid();
	DB = QSqlDatabase::database(ConnectionName);

	if (!DB.isValid())
	{
		DB = QSqlDatabase::cloneDatabase(ISDatabase::GetInstance().GetDefaultDB(), ConnectionName);
	}
}
//-----------------------------------------------------------------------------
ISModelThreadWorker::~ISModelThreadWorker()
{
	
}
//-----------------------------------------------------------------------------
void ISModelThreadWorker::Execute(const QString &SqlQueryText, const QVariantMap &Conditions)
{
	QList<QSqlRecord> Records;
	int Step = 0;
	ISCountingTime Time;

	ISDebug::ShowDebugString("Connecting to DB...");
	if (DB.open())
	{
		ISDebug::ShowDebugString(QString("Connecting to DB %1 msec.").arg(Time.GetElapsed()));
		Time.Restart();

		QSqlQuery *SqlQuery = new QSqlQuery(DB);
		ISDebug::ShowDebugString("Preparing query...");
		if (SqlQuery->prepare(SqlQueryText))
		{
			ISDebug::ShowDebugString(QString("Prepared query %1 msec.").arg(Time.GetElapsed()));
			Time.Restart();

			for (const auto &Condition : Conditions.toStdMap()) //Обход параметров запроса
			{
				if (SqlQuery->boundValues().contains(Condition.first)) //Если параметр найден
				{
					SqlQuery->bindValue(Condition.first, Condition.second);
				}
				else //Параметр не найден
				{
					IS_ASSERT(false, QString("Not bind value parameter: %1").arg(Condition.first));
				}
			}

			ISDebug::ShowDebugString("Executing query...");
			if (SqlQuery->exec())
			{
				ISDebug::ShowDebugString(QString("Executed query %1 msec.").arg(Time.GetElapsed()));

				if (Time.GetElapsed() > MAX_QUERY_TIME)
				{
					ISDebug::ShowDebugString(QString("Long query %1 msec: %2").arg(Time.GetElapsed()).arg(SqlQuery->lastQuery().simplified()));
				}

				emit ExecutedQuery();

				Time.Restart();
				ISDebug::ShowDebugString("Loading records...");
				while (SqlQuery->next()) //Зарузка записей
				{
					Records.push_back(SqlQuery->record());

					if (Step == 500) //Пауза при добавлении участка записей
					{
						QThread::currentThread()->msleep(1);
						Step = 0;
					}
					else
					{
						Step++;
					}
				}
				ISDebug::ShowDebugString(QString("Loaded records %1, %2 msec.").arg(Records.count()).arg(Time.GetElapsed()));

				emit Results(Records);
				emit Finished();
			}
			else
			{
				emit ErrorQuery(SqlQuery->lastError(), SqlQueryText);
			}
		}
		else
		{
			emit ErrorQuery(SqlQuery->lastError(), SqlQueryText);
		}

		delete SqlQuery;
		SqlQuery = nullptr;

		DB.close();
	}
	else
	{
		emit ErrorConnection(DB.lastError());
	}
}
//-----------------------------------------------------------------------------
