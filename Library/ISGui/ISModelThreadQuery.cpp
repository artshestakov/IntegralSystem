#include "ISModelThreadQuery.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
ISModelThreadQuery::ISModelThreadQuery(QObject *parent)
	: QThread(parent),
	ModelWorker(nullptr)
{
	
}
//-----------------------------------------------------------------------------
ISModelThreadQuery::~ISModelThreadQuery()
{
	POINTER_DELETE(ModelWorker);
}
//-----------------------------------------------------------------------------
void ISModelThreadQuery::Execute(const QString &SqlQuery, const QVariantMap &Conditions)
{
	emit Started();

	//�������� �������� �������� ������ ������
	while (!ModelWorker)
	{
		QThread::currentThread()->msleep(10);
	}
	emit ExecuteQuery(SqlQuery, Conditions);
}
//-----------------------------------------------------------------------------
void ISModelThreadQuery::run()
{
	ModelWorker = new ISModelThreadWorker();
	connect(this, &ISModelThreadQuery::ExecuteQuery, ModelWorker, &ISModelThreadWorker::Execute);
	connect(ModelWorker, &ISModelThreadWorker::Started, this, &ISModelThreadQuery::Started);
	connect(ModelWorker, &ISModelThreadWorker::Finished, this, &ISModelThreadQuery::Finished);
	connect(ModelWorker, &ISModelThreadWorker::Results, this, &ISModelThreadQuery::Results);
	connect(ModelWorker, &ISModelThreadWorker::ExecutedQuery, this, &ISModelThreadQuery::ExecutedQuery);
	connect(ModelWorker, &ISModelThreadWorker::ErrorConnection, this, &ISModelThreadQuery::ErrorConnection);
	connect(ModelWorker, &ISModelThreadWorker::ErrorQuery, this, &ISModelThreadQuery::ErrorQuery);
	exec();
}
//-----------------------------------------------------------------------------
