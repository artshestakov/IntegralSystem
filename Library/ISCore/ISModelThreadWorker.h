#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISModelThreadWorker : public QObject
{
	Q_OBJECT

signals:
	void Started();
	void Finished();
	void Results(const QList<QSqlRecord> &Records); //Передача данных
	void ExecutedQuery(); //Сигнал о выполнении запроса
	void ErrorConnection(const QSqlError &SqlError); //Сигнал об ошибке соединения к БД
	void ErrorQuery(const QSqlError &SqlError, const QString &QueryText); //Сигнал об ошибке в запросе

public:
	ISModelThreadWorker(QObject *parent = 0);
	virtual ~ISModelThreadWorker();

	void Execute(const QString &SqlQueryText, const QVariantMap &Conditions);

private:
	ISUuid ConnectionName;
	QSqlDatabase DB;
};
//-----------------------------------------------------------------------------
