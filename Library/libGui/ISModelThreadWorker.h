#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISModelThreadWorker : public QObject
{
	Q_OBJECT

signals:
	void Started();
	void Finished();
	void Results(const std::vector<QSqlRecord> &Records); //Передача данных
	void ExecutedQuery(); //Сигнал о выполнении запроса
	void ErrorConnection(const QSqlError &SqlError); //Сигнал об ошибке соединения к БД
	void ErrorQuery(const QSqlError &SqlError, const QString &QueryText); //Сигнал об ошибке в запросе

public:
	ISModelThreadWorker(QObject *parent = 0);
	virtual ~ISModelThreadWorker();

	void Execute(const QString &SqlQueryText, const QVariantMap &Conditions);
};
//-----------------------------------------------------------------------------
