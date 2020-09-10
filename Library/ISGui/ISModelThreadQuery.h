#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISModelThreadWorker.h"
//-----------------------------------------------------------------------------
class ISModelThreadQuery : public QThread
{
	Q_OBJECT

signals:
	void Started();
	void Finished();
	void ExecuteQuery(const QString &SqlQuery, const QVariantMap &Conditions);
	void Results(const std::vector<QSqlRecord> &Records, const std::vector<QString> &Fields); //Вывод результатов из потока
	void ExecutedQuery(); //Сигнал о выполнении запроса
	void ErrorConnection(const QSqlError &SqlError); //Сигнал об ошибке соединения к БД
	void ErrorQuery(const QSqlError &SqlError, const QString &QueryText); //Сигнал об ошибке в запросе

public:
	ISModelThreadQuery(QObject *parent = 0);
	virtual ~ISModelThreadQuery();

	void Execute(const QString &SqlQuery, const QVariantMap &Conditions);

protected:
	void run();

private:
	ISModelThreadWorker *ModelWorker;
};
//-----------------------------------------------------------------------------
