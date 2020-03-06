#include "ISModelThreadWorker.h"
#include "ISDatabase.h"
#include "ISAssert.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISModelThreadWorker::ISModelThreadWorker(QObject *parent)
	: QObject(parent),
	DB(QSqlDatabase::cloneDatabase(ISDatabase::GetInstance().GetDefaultDB(), CONNECTION_MODEL_THREAD))
{
	
}
//-----------------------------------------------------------------------------
ISModelThreadWorker::~ISModelThreadWorker()
{
	
}
//-----------------------------------------------------------------------------
void ISModelThreadWorker::Execute(const QString &SqlQueryText, const QVariantMap &Conditions)
{
	if (DB.open())
	{
		QSqlQuery *SqlQuery = new QSqlQuery(DB);
		if (SqlQuery->prepare(SqlQueryText))
		{
			for (const auto &Condition : Conditions.toStdMap()) //����� ���������� �������
			{
				if (SqlQuery->boundValues().contains(Condition.first)) //���� �������� ������
				{
					SqlQuery->bindValue(Condition.first, Condition.second);
				}
				else //�������� �� ������
				{
					IS_ASSERT(false, QString("Not bind value parameter: %1").arg(Condition.first));
				}
			}
			if (SqlQuery->exec())
			{
				emit ExecutedQuery();
				
				std::vector<QSqlRecord> Records(SqlQuery->isSelect() ? SqlQuery->size() : 0);
				size_t Step = 0, Index = 0;
				while (SqlQuery->next()) //������� �������
				{
					Records[Index] = SqlQuery->record();
					if (Step == 500) //����� ��� ���������� ������� �������
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
				emit ErrorQuery(SqlQuery->lastError(), SqlQueryText);
			}
		}
		else
		{
			emit ErrorQuery(SqlQuery->lastError(), SqlQueryText);
		}
		delete SqlQuery;
		DB.close();
	}
	else
	{
		emit ErrorConnection(DB.lastError());
	}
}
//-----------------------------------------------------------------------------
