#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQueryPoolObject.h"
//-----------------------------------------------------------------------------
class ISQueryPool : public QObject
{
	Q_OBJECT

public:
	ISQueryPool(const ISQueryPool &) = delete;
	ISQueryPool(ISQueryPool &&) = delete;
	ISQueryPool &operator=(const ISQueryPool &) = delete;
	ISQueryPool &operator=(ISQueryPool &&) = delete;
	~ISQueryPool();

	static ISQueryPool& GetInstance();

	void AddQuery(const QString &SqlText); //Добавить запрос в очередь
	void AddQuery(const QString &SqlText, const QVariantMap &Parameters); //Добавить запрос в очередь

protected:
	void StartExecuting();
	void ExecuteQuery();

private:
	ISQueryPool(QObject *parent = 0);
	QSqlDatabase DB;
	QFutureWatcher<void> *FutureWatcher;
	QQueue<ISQueryPoolObject> Queue;
};
//-----------------------------------------------------------------------------
