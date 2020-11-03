#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISQueryPool : public QObject
{
	Q_OBJECT

signals:
	void Runned();

public:
	static ISQueryPool& Instance();

	QString GetErrorString() const;
	bool Start(const QString &host, unsigned short port, const QString &database, const QString &login, const QString &password);
	void Shutdown();

	void AddQuery(const QString &SqlText, const ISStringToVariantMap &Parameters); //Добавить запрос в очередь

private:
	void StartWorker();

private:
	ISQueryPool();
	~ISQueryPool();
    ISQueryPool(ISQueryPool const &) : QObject() {};
	ISQueryPool& operator=(ISQueryPool const&) { return *this; };

private:
	QString ErrorString;
	QString Host;
	unsigned short Port;
	QString Database;
	QString Login;
	QString Password;
	bool IsRunning;
	bool IsFinished;
	ISCriticalSection CriticalSection;
	std::queue<ISQueryPoolObject> Queue;
};
//-----------------------------------------------------------------------------
