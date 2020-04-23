#include "ISModelThreadWorker.h"
#include "ISDatabase.h"
#include "ISAssert.h"
#include "ISConstants.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISModelThreadWorker::ISModelThreadWorker(QObject *parent)
	: QObject(parent)
{

}
//-----------------------------------------------------------------------------
ISModelThreadWorker::~ISModelThreadWorker()
{
	
}
//-----------------------------------------------------------------------------
void ISModelThreadWorker::Execute(const QString &SqlQueryText, const QVariantMap &Conditions)
{
	ISUuid ConnectionName = ISSystem::GenerateUuid();
	ISConnectOptionDB ConnectOption = ISDatabase::Instance().GetOption(CONNECTION_DEFAULT);
	if (ISDatabase::Instance().Connect(ConnectionName, ConnectOption.Host, ConnectOption.Port, ConnectOption.Name, ConnectOption.Login, ConnectOption.Password))
	{
		{
			QSqlQuery SqlQuery = QSqlQuery(ISDatabase::Instance().GetDB(ConnectionName));
			if (SqlQuery.prepare(SqlQueryText))
			{
				for (const auto &Condition : Conditions.toStdMap()) //Обход параметров запроса
				{
					//Если параметр найден - добавляем его в запрос, иначе - ассерт
					SqlQuery.boundValues().contains(Condition.first) ?
						SqlQuery.bindValue(Condition.first, Condition.second) :
						IS_ASSERT(false, QString("Not bind value parameter: %1").arg(Condition.first));
				}

				if (SqlQuery.exec())
				{
					emit ExecutedQuery();

					std::vector<QSqlRecord> Records(SqlQuery.isSelect() ? SqlQuery.size() : 0);
					size_t Step = 0, Index = 0;
					while (SqlQuery.next()) //Зарузка записей
					{
						Records[Index] = SqlQuery.record();
						if (Step == 500) //Пауза при добавлении участка записей
						{
							QThread::currentThread()->msleep(10);
							Step = 0;
						}
						else
						{
							++Step;
						}
						++Index;
					}
					emit Results(Records);
					emit Finished();
				}
				else
				{
					emit ErrorQuery(SqlQuery.lastError(), SqlQueryText);
				}
			}
			else
			{
				emit ErrorQuery(SqlQuery.lastError(), SqlQueryText);
			}
		}
		ISDatabase::Instance().Disconnect(ConnectionName);
	}
	else
	{
		emit ErrorConnection(ISDatabase::Instance().GetErrorString());
	}
}
//-----------------------------------------------------------------------------
