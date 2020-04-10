#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
#include <queue>
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryPool
{
public:
	static ISQueryPool& Instance();

	QString GetErrorString() const;
	void Start();
	void Shutdown();

	void AddQuery(const QString &SqlText); //Добавить запрос в очередь
	void AddQuery(const QString &SqlText, const ISStringToVariantMap &Parameters); //Добавить запрос в очередь

private:
	void StartWorker();

private:
	ISQueryPool();
	~ISQueryPool();
	ISQueryPool(ISQueryPool const &) {};
	ISQueryPool& operator=(ISQueryPool const&) { return *this; };

private:
	QString ErrorString;
	bool IsRunning;
	bool IsFinished;
	std::mutex Mutex;
	std::queue<ISQueryPoolObject> Queue;
};
//-----------------------------------------------------------------------------
